from ns3codegen import *

from typing import List
from collections import deque

class NS3Writer:
	def __init__(self, filename, codegen):
		self.filename = filename
		self.gpus = codegen.gpus		     # dict[str, int]
		self.nvswitches = codegen.nvswitches # dict[str, int]  (P4 / NVSwitch model)
		self.reg_switches = codegen.reg_switches # dict[str, int] (SwitchNode ECMP model)
		self.link_helpers = codegen.link_helpers # dict[tuple, int]
		self.insns = codegen.insns

		self.lines = []
		self.indent = 0
		self.container_uid = 0

		self.container_map = {} # (src_name, dst_name) -> container_expr

		# Topology tracking for reg-switch routing computation
		# Maps (gpu_name, sw_name) → container_var where Get(0)=GPU device, Get(1)=SW device
		self.gpu_to_sw_links: dict[tuple, str] = {}
		# Maps (sw_a_name, sw_b_name) → container_var where Get(0)=sw_a device, Get(1)=sw_b device
		self.sw_to_sw_links: dict[tuple, str] = {}
		# GPU pairs that have direct P2P links (do NOT get PushPeerIpAddr)
		self.gpu_direct_pairs: set = set()

	def emit(self, line=""):
		self.lines.append("    " * self.indent + line)

	def write(self):
		self._emit_headers()
		self._emit_main_start()

		for insn in self.insns:
			self._handle_insn(insn)

		self._emit_forwarding_setup()
		self._emit_regswitch_setup()
		self._emit_gpu_setup()
		self._emit_main_end()

		with open(self.filename, "w") as f:
			f.write("\n".join(self.lines))

	# --------------------------------------------------
	# Headers + main
	# --------------------------------------------------

	def _emit_headers(self):
		self.emit('#include "ns3/core-module.h"')
		self.emit('#include "ns3/network-module.h"')
		self.emit('#include "ns3/internet-module.h"')
		self.emit('#include "ns3/point-to-point-module.h"')
		self.emit('#include "ns3/csma-module.h"')
		self.emit('#include "ns3/ethernet-switch-module.h"')
		self.emit('#include "ns3/distributed-training-module.h"')
		self.emit("")
		self.emit("using namespace ns3;")
		self.emit("")

	def _emit_main_start(self):
		self.emit("int main(int argc, char *argv[]) {")
		self.indent += 1

		self.emit("NodeContainer gpunodes;")
		self.emit("NodeContainer swtches;")     # NV switches (P4SwitchNetDevice)
		self.emit("NodeContainer regswtches;")  # ECMP switches (SwitchNode)
		self.emit("P4Helper sw_helper;")
		self.emit("sw_helper.SetDeviceAttribute(\"EnableCustomImpl\", BooleanValue(true));")
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

		elif insn.__class__.__name__ == "NS3MakeRegSwitches":
			self.emit(f"regswtches.Create<SwitchNode>({insn.n_regswitches});")

		elif insn.__class__.__name__ == "NS3MakeLinkHelper":
			self._emit_link_helper(insn)

		elif insn.__class__.__name__ == "NS3MakeSwitchHelper":
			self._emit_switch_helper(insn)

		elif insn.__class__.__name__ == "NS3InstallLink":
			self._emit_link_install(insn)

		else:
			raise ValueError(f"Unknown instruction {insn}")

	# --------------------------------------------------
	# Link and switch helpers
	# --------------------------------------------------

	def _emit_switch_helper(self, insn):
		self.emit(f"sw_helper.SetDeviceAttribute(\"Mtu\", UintegerValue({insn.mtu}));")
		for id in insn.switch_ids:
			self.emit(f"NetDeviceContainer sw_dev{id} = sw_helper.Install(swtches.Get({id}));")
			self.emit(f"Ptr<P4SwitchNetDevice> sw{id} = DynamicCast<P4SwitchNetDevice>(sw_dev{id}.Get(0));")
		self.emit("")

	def _emit_link_helper(self, insn):
		hid = insn.id
		delay_val, delay_unit = insn.delay
		bw_val, bw_unit = insn.data_rate
		mtu = insn.mtu

		match insn.type:
			case "eth":
				helper_name = "SwitchedEthernet"
			case "p2p":
				helper_name = "PointToPoint"
			case "default":
				helper_name = "Csma"
			case _:
				raise RuntimeError(f"Unrecognized link type {insn.type}.")

		self.emit(f"{helper_name}Helper link_helper{hid};")
		self.emit(f"link_helper{hid}.SetDeviceAttribute(\"Mtu\", UintegerValue({insn.mtu}));")
		self.emit(f'link_helper{hid}.SetChannelAttribute("Delay", StringValue("{delay_val}{delay_unit}"));')
		if insn.type == "p2p":
			# PointToPointNetDevice (unlike Csma/SwitchedEthernet) exposes DataRate
			# as a device attribute, not a channel attribute
			self.emit(f'link_helper{hid}.SetDeviceAttribute("DataRate", StringValue("{bw_val}{bw_unit}"));')
		else:
			self.emit(f'link_helper{hid}.SetChannelAttribute("DataRate", StringValue("{bw_val}{bw_unit}"));')
		self.emit("")

	# --------------------------------------------------
	# Link installation
	# --------------------------------------------------

	def _resolve_node(self, name):
		if name in self.gpus:
			return f"gpunodes.Get({self.gpus[name]})", "gpu"
		elif name in self.nvswitches:
			return f"swtches.Get({self.nvswitches[name]})", "nvswitch"
		elif name in self.reg_switches:
			return f"regswtches.Get({self.reg_switches[name]})", "regswitch"
		else:
			raise ValueError(f"Unknown node {name}")

	def _emit_link_install(self, insn):
		src_expr, src_type = self._resolve_node(insn.src)
		dst_expr, dst_type = self._resolve_node(insn.dst)

		hid = insn.link_helper

		# --------------------------------------------------
		# GPU <-> GPU : point-to-point
		# --------------------------------------------------

		if src_type == "gpu" and dst_type == "gpu":

			container_expr = f"devs{hid}_{self.container_uid}"

			self.emit(
				f"NetDeviceContainer {container_expr} = "
				f"link_helper{hid}.Install({src_expr}, {dst_expr});"
			)

			self.emit("")

			self.container_map[(insn.src, insn.dst)] = container_expr

			self._emit_push_send_device(
				src_expr,
				insn.dst,
				f"{container_expr}.Get(0)"
			)

			self._emit_push_recv_device(
				dst_expr,
				insn.src,
				f"{container_expr}.Get(1)"
			)

			# p2p links are full-duplex, so the same pair of devices also
			# serves the reverse direction (dst -> src)
			self._emit_push_send_device(
				dst_expr,
				insn.src,
				f"{container_expr}.Get(1)"
			)

			self._emit_push_recv_device(
				src_expr,
				insn.dst,
				f"{container_expr}.Get(0)"
			)

			# a single p2p link replaces what used to be 2 unidirectional CSMA
			# links, so register both peers' addresses here
			self.emit(
				f"DynamicCast<GPU>({src_expr})->PushPeerAddr("
				f"{self.gpus[insn.dst]}, ({container_expr}.Get(1))->GetAddress());"
			)
			self.emit(
				f"DynamicCast<GPU>({dst_expr})->PushPeerAddr("
				f"{self.gpus[insn.src]}, ({container_expr}.Get(0))->GetAddress());"
			)

			self.emit("")

			# Record direct link so these peers are NOT given PushPeerIpAddr
			self.gpu_direct_pairs.add(frozenset({insn.src, insn.dst}))

		# --------------------------------------------------
		# GPU <-> NVSwitch  (Ethernet / P4 path — unchanged)
		# --------------------------------------------------

		elif (src_type == "gpu" and dst_type == "nvswitch") or \
		     (src_type == "nvswitch" and dst_type == "gpu"):

			container_expr = f"devs{hid}_{self.container_uid}"

			if src_type == "gpu":
				sw_idx  = self.nvswitches[insn.dst]
				gpu_expr = src_expr
				gpu_name = insn.src
			else:
				sw_idx  = self.nvswitches[insn.src]
				gpu_expr = dst_expr
				gpu_name = insn.dst

			self.emit(
				f"NetDeviceContainer {container_expr} = "
				f"link_helper{hid}.ConnectHost(sw{sw_idx}, {gpu_expr});"
			)

			self.emit("")

			self._emit_loop_push_peer_device(
				gpu_expr,
				gpu_name,
				f"{container_expr}.Get(0)"
			)

			self._emit_loop_push_peer_addr(
				gpu_name,
				f"{container_expr}.Get(0)"
			)

			self._record_host_attachment(
				insn.dst if src_type == "gpu" else insn.src,
				gpu_name,
				f"{container_expr}.Get(0)"
			)

		# --------------------------------------------------
		# NVSwitch <-> NVSwitch  (Ethernet — unchanged)
		# --------------------------------------------------

		elif src_type == "nvswitch" and dst_type == "nvswitch":

			src_idx = self.nvswitches[insn.src]
			dst_idx = self.nvswitches[insn.dst]

			self.emit(
				f"link_helper{hid}.ConnectSwitches(sw{src_idx}, sw{dst_idx});"
			)

			self.emit("")

		# --------------------------------------------------
		# GPU <-> RegSwitch  (P2P — new ECMP/UDP path)
		# --------------------------------------------------

		elif (src_type == "gpu" and dst_type == "regswitch") or \
		     (src_type == "regswitch" and dst_type == "gpu"):

			container_expr = f"devs{hid}_{self.container_uid}"

			if src_type == "gpu":
				gpu_name = insn.src
				sw_name  = insn.dst
				# Install: src=GPU (Get(0)), dst=SW (Get(1))
				self.emit(
					f"NetDeviceContainer {container_expr} = "
					f"link_helper{hid}.Install({src_expr}, {dst_expr});"
				)
				self.gpu_to_sw_links[(gpu_name, sw_name)] = container_expr
			else:
				gpu_name = insn.dst
				sw_name  = insn.src
				# Install: src=SW (Get(0)), dst=GPU (Get(1))
				# Swap so GPU is always Get(0) in our bookkeeping by re-installing in right order
				self.emit(
					f"NetDeviceContainer {container_expr} = "
					f"link_helper{hid}.Install({dst_expr}, {src_expr});"
				)
				# In this case GPU is dst_expr but we installed as (gpu, sw) → GPU=Get(0), SW=Get(1)
				self.gpu_to_sw_links[(gpu_name, sw_name)] = container_expr

			self.emit("")

		# --------------------------------------------------
		# RegSwitch <-> RegSwitch  (P2P — new)
		# --------------------------------------------------

		elif src_type == "regswitch" and dst_type == "regswitch":

			container_expr = f"devs{hid}_{self.container_uid}"

			self.emit(
				f"NetDeviceContainer {container_expr} = "
				f"link_helper{hid}.Install({src_expr}, {dst_expr});"
			)

			self.emit("")

			self.sw_to_sw_links[(insn.src, insn.dst)] = container_expr

		else:
			raise RuntimeError(f"Unsupported link type: {src_type} <-> {dst_type}")

		self.container_uid += 1

	def _emit_push_send_device(self, src_expr, dst_name, dev_expr):
		self.emit(f"DynamicCast<GPU>({src_expr})->PushSendPeerDevice({self.gpus[dst_name]}, {dev_expr});")

	def _emit_push_recv_device(self, dst_expr, src_name, dev_expr):
		self.emit(f"DynamicCast<GPU>({dst_expr})->PushRecvPeerDevice({self.gpus[src_name]}, {dev_expr});")

	def _emit_loop_push_peer_device(self, gpu_expr, gpu_name, dev_expr):
		self.emit(f"for (int i = 0; i < {len(self.gpus)}; ++i)" + "{")
		self.indent += 1

		self.emit(f"if (i != {self.gpus[gpu_name]})" + "{")
		self.indent += 1

		self.emit(
			f"DynamicCast<GPU>({gpu_expr})->PushSendPeerDevice(i, {dev_expr});"
		)

		self.emit(
			f"DynamicCast<GPU>({gpu_expr})->PushRecvPeerDevice(i, {dev_expr});"
		)

		self.indent -= 1
		self.emit("}")

		self.indent -= 1
		self.emit("}")

		self.emit("")

	def _emit_loop_push_peer_addr(self, gpu_name, dev_expr):
		gpu_idx = self.gpus[gpu_name]

		self.emit(f"for (int i = 0; i < {len(self.gpus)}; ++i)" + "{")
		self.indent += 1

		self.emit(f"if (i != {gpu_idx})" + "{")
		self.indent += 1

		self.emit(
			f"DynamicCast<GPU>(gpunodes.Get(i))->PushPeerAddr({gpu_idx}, {dev_expr}->GetAddress());"
		)

		self.indent -= 1
		self.emit("}")

		self.indent -= 1
		self.emit("}")

		self.emit("")
	# --------------------------------------------------
	# NVSwitch handling (unchanged)
	# --------------------------------------------------

	def _record_host_attachment(self, switch_name, gpu_name, dev_expr):
		if not hasattr(self, "host_attachments"):
			self.host_attachments = {}

		self.host_attachments.setdefault(switch_name, []).append(
			(gpu_name, dev_expr)
		)

	def _emit_forwarding_setup(self):
		return
		if not hasattr(self, "host_attachments"):
			return

		self.emit("")
		self.emit("// Switch forwarding tables")

		for sw_name, hosts in self.host_attachments.items():

			sw_idx = self.nvswitches[sw_name]

			for port_idx, (gpu_name, dev_expr) in enumerate(hosts):

				for other_gpu_name in self.gpus.keys():

					self.emit(
						f"sw{sw_idx}->GetCustomImpl()->AddAddrForwarding("
						f"DynamicCast<GPU>(gpunodes.Get({self.gpus[other_gpu_name]}))"
						f"->GetAddress(), "
						f"{port_idx}"
						f");"
					)

			self.emit("")

	# --------------------------------------------------
	# RegSwitch routing setup (new)
	# --------------------------------------------------

	def _emit_regswitch_setup(self):
		"""
		After all links are installed:
		1. InternetStack on GPU and reg-switch nodes.
		2. Assign IP 10.0.0.(i+1)/8 to each GPU's switch-facing P2P interface.
		3. BFS-computed ECMP routing table entries for each SwitchNode.
		4. Static default routes on GPU nodes pointing at the switch fabric.
		5. PushSendPeerDevice + PushPeerIpAddr for switch-reachable GPU pairs.
		"""

		# Nothing to do if no reg-switch links were installed
		if not self.gpu_to_sw_links and not self.sw_to_sw_links:
			return

		self.emit("")
		self.emit("// ---- RegSwitch (SwitchNode / ECMP) setup ----")
		self.emit("")

		# 1. InternetStack
		self.emit("InternetStackHelper internetStack;")
		self.emit("internetStack.Install(gpunodes);")
		self.emit("internetStack.Install(regswtches);")
		self.emit("")

		# 2. Assign IP to GPU switch-facing interfaces.
		#    Each GPU may have at most one switch-facing P2P device per the typical topology.
		#    Track which GPUs have a switch-facing device.
		gpu_sw_dev: dict[str, str] = {}  # gpu_name → container.Get(0) expression
		for (gpu_name, sw_name), cvar in self.gpu_to_sw_links.items():
			gpu_sw_dev[gpu_name] = f"{cvar}.Get(0)"

		for gpu_name, dev_expr in gpu_sw_dev.items():
			gpu_idx = self.gpus[gpu_name]
			ip_str = f"10.0.0.{gpu_idx + 1}"
			self.emit("{")
			self.indent += 1
			self.emit(f"Ipv4AddressHelper _ipv4;")
			self.emit(f'_ipv4.SetBase("10.0.0.0", "255.0.0.0", "0.0.0.{gpu_idx + 1}");')
			self.emit(f"NetDeviceContainer _tmp;")
			self.emit(f"_tmp.Add({dev_expr});")
			self.emit(f"_ipv4.Assign(_tmp);")
			self.indent -= 1
			self.emit("}")
		self.emit("")

		# 3. BFS routing for SwitchNodes
		# Build adjacency: node_name → list of (neighbor_name, container_var, my_side)
		# my_side = 0 means this node is Get(0) in container, neighbor is Get(1)
		adj: dict[str, list] = {}
		for (gpu, sw), cvar in self.gpu_to_sw_links.items():
			adj.setdefault(gpu, []).append((sw, cvar, 0))
			adj.setdefault(sw, []).append((gpu, cvar, 1))
		for (sw_a, sw_b), cvar in self.sw_to_sw_links.items():
			adj.setdefault(sw_a, []).append((sw_b, cvar, 0))
			adj.setdefault(sw_b, []).append((sw_a, cvar, 1))

		# BFS from each destination GPU; for each reg switch reached, emit AddTableEntry
		self.emit("// SwitchNode routing tables")
		for dst_gpu, dst_idx in self.gpus.items():
			dst_ip = f"10.0.0.{dst_idx + 1}"
			if dst_gpu not in adj:
				continue  # GPU has no switch connections

			# BFS: track distance and ECMP parent links
			dist: dict[str, int] = {dst_gpu: 0}
			# parent_links[node] = list of (parent, cvar, my_side_toward_parent)
			# where "toward_parent" means toward dst_gpu direction
			parent_links: dict[str, list] = {dst_gpu: []}
			queue = deque([dst_gpu])

			while queue:
				node = queue.popleft()
				for neighbor, cvar, my_side in adj.get(node, []):
					neighbor_side = 1 - my_side
					if neighbor not in dist:
						dist[neighbor] = dist[node] + 1
						parent_links[neighbor] = [(node, cvar, neighbor_side)]
						queue.append(neighbor)
					elif dist[neighbor] == dist[node] + 1:
						# Equal-cost path: add for ECMP
						parent_links[neighbor].append((node, cvar, neighbor_side))

			# Emit AddTableEntry for each reg switch
			for sw_name in self.reg_switches:
				if sw_name not in parent_links:
					continue
				sw_idx_val = self.reg_switches[sw_name]
				for _parent, cvar, my_side in parent_links[sw_name]:
					self.emit("{")
					self.indent += 1
					self.emit(f'Ipv4Address _dst("{dst_ip}");')
					self.emit(
						f"DynamicCast<SwitchNode>(regswtches.Get({sw_idx_val}))"
						f"->AddTableEntry(_dst, {cvar}.Get({my_side})->GetIfIndex());"
					)
					self.indent -= 1
					self.emit("}")
		self.emit("")

		# 4. GPU static routes toward the switch fabric
		self.emit("// GPU static routes to switch fabric")
		for gpu_name, dev_expr in gpu_sw_dev.items():
			gpu_idx = self.gpus[gpu_name]
			self.emit("{")
			self.indent += 1
			self.emit("Ipv4StaticRoutingHelper _srh;")
			self.emit(
				f"Ptr<Ipv4StaticRouting> _sr = "
				f"_srh.GetStaticRouting(gpunodes.Get({gpu_idx})->GetObject<Ipv4>());"
			)
			self.emit(
				f"int32_t _ifIdx = gpunodes.Get({gpu_idx})->GetObject<Ipv4>()"
				f"->GetInterfaceForDevice({dev_expr});"
			)
			self.emit(
				f'_sr->AddNetworkRouteTo(Ipv4Address("10.0.0.0"), Ipv4Mask("255.0.0.0"), _ifIdx);'
			)
			self.indent -= 1
			self.emit("}")
		self.emit("")

		# 5. PushSendPeerDevice + PushPeerIpAddr for switch-reachable GPU pairs
		# A GPU pair is switch-reachable if both GPUs have switch-facing devices
		# AND they don't have a direct P2P link.
		self.emit("// PushSendPeerDevice and PushPeerIpAddr for switch-fabric pairs")
		sw_connected_gpus = set(gpu_sw_dev.keys())
		for src_gpu in sw_connected_gpus:
			src_idx = self.gpus[src_gpu]
			src_dev = gpu_sw_dev[src_gpu]
			for dst_gpu in sw_connected_gpus:
				if src_gpu == dst_gpu:
					continue
				if frozenset({src_gpu, dst_gpu}) in self.gpu_direct_pairs:
					continue  # Direct link exists; don't override with UDP path
				dst_idx = self.gpus[dst_gpu]
				dst_ip  = f"10.0.0.{dst_idx + 1}"
				# Register the switch-facing device as the send device for pacing
				self.emit(
					f"DynamicCast<GPU>(gpunodes.Get({src_idx}))"
					f"->PushSendPeerDevice({dst_idx}, {src_dev});"
				)
				# Register the peer's IP so collectives uses the UDP path
				self.emit(
					f"DynamicCast<GPU>(gpunodes.Get({src_idx}))"
					f'->PushPeerIpAddr({dst_idx}, Ipv4Address("{dst_ip}"));'
				)
		self.emit("")

	# --------------------------------------------------
	# GPU handling
	# --------------------------------------------------

	# For now just print container map
	def _emit_gpu_setup(self):
		self.emit("")
		self.emit("/*")
		self.indent += 1
		for pair, container in self.container_map.items():
			self.emit(f"{pair[0]} -> {pair[1]}: {container}")
		self.indent -= 1
		self.emit("*/")
