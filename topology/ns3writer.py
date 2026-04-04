from ns3codegen import *

from typing import List

class NS3Writer:
	def __init__(self, filename, codegen):
		self.filename = filename
		self.gpus = codegen.gpus		  # dict[str, int]
		self.switches = codegen.switches  # dict[str, int]
		self.link_helpers = codegen.link_helpers # dict[tuple, int]
		self.insns = codegen.insns

		self.lines = []
		self.indent = 0
		self.container_uid = 0

	def emit(self, line=""):
		self.lines.append("    " * self.indent + line)

	def write(self):
		self._emit_headers()
		self._emit_main_start()

		for insn in self.insns:
			self._handle_insn(insn)

		self._emit_bridge_setup()
		self._emit_main_end()

		with open(self.filename, "w") as f:
			f.write("\n".join(self.lines))

	# --------------------------------------------------
	# Headers + main
	# --------------------------------------------------

	def _emit_headers(self):
		self.emit('#include "ns3/core-module.h"')
		self.emit('#include "ns3/network-module.h"')
		self.emit('#include "ns3/csma-module.h"')
		self.emit('#include "ns3/bridge-module.h"')
		self.emit("")
		self.emit("using namespace ns3;")
		self.emit("")

	def _emit_main_start(self):
		self.emit("int main(int argc, char *argv[]) {")
		self.indent += 1

		self.emit("NodeContainer gpunodes;")
		self.emit("NodeContainer swtches;")
		self.emit("")

	def _emit_main_end(self):
		self.emit("")
		self.emit("Simulator::Run();")
		self.emit("Simulator::Destroy();")
		self.emit("return 0;")
		self.indent -= 1
		self.emit("}")

	# --------------------------------------------------
	# Instruction handlers
	# --------------------------------------------------

	def _handle_insn(self, insn):
		from types import SimpleNamespace

		if insn.__class__.__name__ == "NS3MakeGPUs":
			self.emit(f"gpunodes.Create<{ 'GPU' }>({insn.n_gpus});")

		elif insn.__class__.__name__ == "NS3MakeSwitches":
			self.emit(f"swtches.Create({insn.n_switches});")

		elif insn.__class__.__name__ == "NS3MakeLinkHelper":
			self._emit_link_helper(insn)

		elif insn.__class__.__name__ == "NS3InstallLink":
			self._emit_link_install(insn)

		else:
			raise ValueError(f"Unknown instruction {insn}")

	# --------------------------------------------------
	# Link helpers
	# --------------------------------------------------

	def _emit_link_helper(self, insn):
		hid = insn.id
		delay_val, delay_unit = insn.delay
		bw_val, bw_unit = insn.data_rate

		self.emit(f"CsmaHelper csma{hid};")
		self.emit(f'csma{hid}.SetChannelAttribute("Delay", StringValue("{delay_val}{delay_unit}"));')
		self.emit(f'csma{hid}.SetChannelAttribute("DataRate", StringValue("{bw_val}{bw_unit}"));')
		self.emit("")

	# --------------------------------------------------
	# Link installation
	# --------------------------------------------------

	def _resolve_node(self, name):
		if name in self.gpus:
			return f"gpunodes.Get({self.gpus[name]})", "gpu"
		elif name in self.switches:
			return f"swtches.Get({self.switches[name]})", "switch"
		else:
			raise ValueError(f"Unknown node {name}")

	def _emit_link_install(self, insn):
		src_expr, src_type = self._resolve_node(insn.src)
		dst_expr, dst_type = self._resolve_node(insn.dst)

		hid = insn.link_helper

		self.emit(f"NodeContainer nc{hid}_{self.container_uid};")
		self.emit(f"nc{hid}_{self.container_uid}.Add({src_expr});")
		self.emit(f"nc{hid}_{self.container_uid}.Add({dst_expr});")
		self.emit(f"NetDeviceContainer devs{hid}_{self.container_uid} = csma{hid}.Install(nc{hid}_{self.container_uid});")
		self.emit("")

		# Track switch ports for bridging later
		if src_type == "switch":
			self._record_switch_port(insn.src, f"devs{hid}_{self.container_uid}.Get(0)")
		if dst_type == "switch":
			self._record_switch_port(insn.dst, f"devs{hid}_{self.container_uid}.Get(1)")	

		# GPU node programming
		if src_type == "gpu":
			if dst_type == "gpu":
				self._emit_push_send_device(src_expr, insns.dst, f"devs{hid}_{self.container_uid}.Get(0)")
				self._emit_push_recv_device(dst_expr, insns.src, f"devs{hid}_{self.container_uid}.Get(1)")
			elif dst_type == "switch":
				self._emit_loop_push_send_device(src_expr, insn.src, f"devs{hid}_{self.container_uid}.Get(0)")
		elif src_type == "switch" and dst_type == "gpu":
			self._emit_loop_push_recv_device(dst_expr, insn.dst, f"devs{hid}_{self.container_uid}.Get(1)")


		self.container_uid += 1
	
	def _emit_push_send_device(self, src_expr, dst_name, dev_expr):
		self.emit(f"{src_expr}->PushSendPeerDevice({self.gpus[dst_name]}, {dev_expr});")
	
	def _emit_push_recv_device(self, dst_expr, src_name, dev_expr):
		self.emit(f"{dst_expr}->PushRecvPeerDevice({self.gpus[src_name]}, {dev_expr});")
	
	def _emit_loop_push_send_device(self, src_expr, src_name, dev_expr):
		self.emit(f"for (int i = 0; i < {len(self.gpus)}; ++i)" + "{")
		self.indent += 1
		self.emit(f"if (i != {self.gpus[src_name]})" + "{")
		self.indent += 1
		self.emit(f"{src_expr}->PushSendPeerDevice(i, {dev_expr});")
		self.indent -= 1
		self.emit("}")
		self.indent -= 1
		self.emit("}")

	def _emit_loop_push_recv_device(self, dst_expr, dst_name, dev_expr):
		self.emit(f"for (int i = 0; i < {len(self.gpus)}; ++i)" + "{")
		self.indent += 1
		self.emit(f"if (i != {self.gpus[dst_name]})" + "{")
		self.indent += 1
		self.emit(f"{dst_expr}->PushRecvPeerDevice(i, {dev_expr});")
		self.indent -= 1
		self.emit("}")
		self.indent -= 1
		self.emit("}")

	# --------------------------------------------------
	# Bridge handling
	# --------------------------------------------------

	def _record_switch_port(self, sw_name, dev_expr):
		if not hasattr(self, "switch_ports"):
			self.switch_ports = {}
		self.switch_ports.setdefault(sw_name, []).append(dev_expr)

	def _emit_bridge_setup(self):
		if not hasattr(self, "switch_ports"):
			return

		self.emit("")
		self.emit("// Bridge setup for switches")

		for sw, ports in self.switch_ports.items():
			sw_idx = self.switches[sw]

			self.emit(f"NetDeviceContainer bridgePorts_{sw};")
			for p in ports:
				self.emit(f"bridgePorts_{sw}.Add({p});")

			self.emit(f"BridgeHelper bridge_{sw};")
			self.emit(f"bridge_{sw}.Install(swtches.Get({sw_idx}), bridgePorts_{sw});")
			self.emit("")
