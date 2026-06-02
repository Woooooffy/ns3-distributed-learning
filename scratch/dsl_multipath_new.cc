#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/ethernet-switch-module.h"
#include "ns3/distributed-training-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    NodeContainer gpunodes;
    NodeContainer swtches;
    P4Helper sw_helper;
    sw_helper.SetDeviceAttribute("EnableCustomImpl", BooleanValue(true));
    
    gpunodes.Create<GPU>(8);
    swtches.Create(6);
    SwitchedEthernetHelper link_helper0;
    link_helper0.SetDeviceAttribute("Mtu", UintegerValue(9000));
    link_helper0.SetChannelAttribute("Delay", StringValue("300ns"));
    link_helper0.SetChannelAttribute("DataRate", StringValue("125Gbps"));
    
    SwitchedEthernetHelper link_helper1;
    link_helper1.SetDeviceAttribute("Mtu", UintegerValue(9000));
    link_helper1.SetChannelAttribute("Delay", StringValue("1500ns"));
    link_helper1.SetChannelAttribute("DataRate", StringValue("9Gbps"));
    
    sw_helper.SetDeviceAttribute("Mtu", UintegerValue(9000));
    NetDeviceContainer sw_dev0 = sw_helper.Install(swtches.Get(0));
    Ptr<P4SwitchNetDevice> sw0 = DynamicCast<P4SwitchNetDevice>(sw_dev0.Get(0));
    NetDeviceContainer sw_dev1 = sw_helper.Install(swtches.Get(1));
    Ptr<P4SwitchNetDevice> sw1 = DynamicCast<P4SwitchNetDevice>(sw_dev1.Get(0));
    NetDeviceContainer sw_dev2 = sw_helper.Install(swtches.Get(2));
    Ptr<P4SwitchNetDevice> sw2 = DynamicCast<P4SwitchNetDevice>(sw_dev2.Get(0));
    NetDeviceContainer sw_dev3 = sw_helper.Install(swtches.Get(3));
    Ptr<P4SwitchNetDevice> sw3 = DynamicCast<P4SwitchNetDevice>(sw_dev3.Get(0));
    NetDeviceContainer sw_dev4 = sw_helper.Install(swtches.Get(4));
    Ptr<P4SwitchNetDevice> sw4 = DynamicCast<P4SwitchNetDevice>(sw_dev4.Get(0));
    NetDeviceContainer sw_dev5 = sw_helper.Install(swtches.Get(5));
    Ptr<P4SwitchNetDevice> sw5 = DynamicCast<P4SwitchNetDevice>(sw_dev5.Get(0));
    
    NetDeviceContainer devs0_0 = link_helper0.ConnectHost(sw0, gpunodes.Get(0));
    
    for (int i = 0; i < 8; ++i){
        if (i != 0){
            DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(i, devs0_0.Get(0));
            DynamicCast<GPU>(gpunodes.Get(0))->PushRecvPeerDevice(i, devs0_0.Get(0));
        }
    }
    
    NetDeviceContainer devs0_1 = link_helper0.ConnectHost(sw0, gpunodes.Get(1));
    
    for (int i = 0; i < 8; ++i){
        if (i != 1){
            DynamicCast<GPU>(gpunodes.Get(1))->PushSendPeerDevice(i, devs0_1.Get(0));
            DynamicCast<GPU>(gpunodes.Get(1))->PushRecvPeerDevice(i, devs0_1.Get(0));
        }
    }
    
    NetDeviceContainer devs0_2 = link_helper0.ConnectHost(sw0, gpunodes.Get(2));
    
    for (int i = 0; i < 8; ++i){
        if (i != 2){
            DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(i, devs0_2.Get(0));
            DynamicCast<GPU>(gpunodes.Get(2))->PushRecvPeerDevice(i, devs0_2.Get(0));
        }
    }
    
    NetDeviceContainer devs0_3 = link_helper0.ConnectHost(sw0, gpunodes.Get(3));
    
    for (int i = 0; i < 8; ++i){
        if (i != 3){
            DynamicCast<GPU>(gpunodes.Get(3))->PushSendPeerDevice(i, devs0_3.Get(0));
            DynamicCast<GPU>(gpunodes.Get(3))->PushRecvPeerDevice(i, devs0_3.Get(0));
        }
    }
    
    NetDeviceContainer devs0_4 = link_helper0.ConnectHost(sw1, gpunodes.Get(4));
    
    for (int i = 0; i < 8; ++i){
        if (i != 4){
            DynamicCast<GPU>(gpunodes.Get(4))->PushSendPeerDevice(i, devs0_4.Get(0));
            DynamicCast<GPU>(gpunodes.Get(4))->PushRecvPeerDevice(i, devs0_4.Get(0));
        }
    }
    
    NetDeviceContainer devs0_5 = link_helper0.ConnectHost(sw1, gpunodes.Get(5));
    
    for (int i = 0; i < 8; ++i){
        if (i != 5){
            DynamicCast<GPU>(gpunodes.Get(5))->PushSendPeerDevice(i, devs0_5.Get(0));
            DynamicCast<GPU>(gpunodes.Get(5))->PushRecvPeerDevice(i, devs0_5.Get(0));
        }
    }
    
    NetDeviceContainer devs0_6 = link_helper0.ConnectHost(sw1, gpunodes.Get(6));
    
    for (int i = 0; i < 8; ++i){
        if (i != 6){
            DynamicCast<GPU>(gpunodes.Get(6))->PushSendPeerDevice(i, devs0_6.Get(0));
            DynamicCast<GPU>(gpunodes.Get(6))->PushRecvPeerDevice(i, devs0_6.Get(0));
        }
    }
    
    NetDeviceContainer devs0_7 = link_helper0.ConnectHost(sw1, gpunodes.Get(7));
    
    for (int i = 0; i < 8; ++i){
        if (i != 7){
            DynamicCast<GPU>(gpunodes.Get(7))->PushSendPeerDevice(i, devs0_7.Get(0));
            DynamicCast<GPU>(gpunodes.Get(7))->PushRecvPeerDevice(i, devs0_7.Get(0));
        }
    }
    
    link_helper1.ConnectSwitches(sw0, sw2);
    
    link_helper1.ConnectSwitches(sw1, sw4);
    
    link_helper1.ConnectSwitches(sw2, sw3);
    
    link_helper1.ConnectSwitches(sw3, sw4);
    
    link_helper1.ConnectSwitches(sw4, sw5);
    
    link_helper1.ConnectSwitches(sw2, sw5);
    
    
    /*
    */

		const std::string LOG_FILE = "/data/commit/graphit/wangyj05/workspace/gloo-ns3-examples/logs/Allgather_DSL_test.txt";
// const std::string LOG_FILE = "/data/commit/graphit/wangyj05/workspace/gloo-ns3-examples/logs/Allgather_n_8_-DGX1-steps_3_rounds_7_chunks_6.txt";
		std::string XML_ALGO = ns3::SystemPath::Append(ns3::SystemPath::FindSelfDirectory(), "../../scratch/test1.xml");

		constexpr int N_NODES = 8;

		constexpr int CHUNK_SIZE = 512;
		constexpr int N_CHUNKS = 2;

		// TODO: unsafe stuff
		std::array<NetDeviceContainer*, N_NODES> recv_addr = {&devs0_0, &devs0_1, &devs0_2, &devs0_3, &devs0_4, &devs0_5, &devs0_6, &devs0_7};
		for (int i = 0; i < N_NODES; ++i){
			for (int j = 0; j < N_NODES; ++j){
				if (i != j) DynamicCast<GPU>(gpunodes.Get(i))->PushPeerAddr(j, recv_addr[j]->Get(1)->GetAddress());
			}
		}

		PacketSocketHelper packetSocket;
		packetSocket.Install(gpunodes);

		TopoNodeSet topo(gpunodes);
		AlgoParseResult result = ParseAlgoFromXml(XML_ALGO.c_str(), topo); 
		if (result != AlgoParseResult::ALGO_PARSE_SUCCESS) NS_LOG_ERROR("Encountered issue in parsing XML algorithm, error code " << result);

		static std::ofstream logtxt;

		// log file
		logtxt.open(LOG_FILE);
		if (!logtxt.is_open()){
    	NS_FATAL_ERROR("Failed to log file");
		}
		chmod(LOG_FILE.c_str(), 0666);
		
		// install apps
		CollectivesApplicationHelper app_helper;
		app_helper.SetAttribute("DataType", EnumValue(DataType::INT32));
		app_helper.SetAttribute("ChunkSize", UintegerValue(CHUNK_SIZE));
		ApplicationContainer apps = app_helper.Install<GPU>(gpunodes);

		// Testing with flow tables
		std::map<std::pair<int, int>, uint32_t> flowIds;
		uint32_t c = 0;
		for (int i = 0; i < N_NODES; ++i){
			for (int j = 0; j < N_NODES; ++j){
				if (i != j)flowIds[std::make_pair(i, j)] = c;
				++c;
			}
		}
		for (uint32_t i = 0; i < apps.GetN(); ++i){
			DynamicCast<CollectivesApplication>(apps.Get(i))->StoreFlowIdTable(&flowIds);
		}

		
		#define SCHEME 1
		std::cout << "Testing scheme " << SCHEME << std::endl;
		std::map<uint32_t, std::vector<int>> shared_trace;
	
	
		Ptr<Node> c1_sw_node = swtches.Get(0);
		Ptr<SmartSwitch> c1_sw = DynamicCast<SmartSwitch>(
				c1_sw_node->GetDevice(c1_sw_node->GetNDevices() - 1));
		c1_sw->DisableLearning();
		c1_sw->SetSharedTraceMap(&shared_trace);
	
		Ptr<Node> c2_sw_node = swtches.Get(1);
		Ptr<SmartSwitch> c2_sw = DynamicCast<SmartSwitch>(
				c2_sw_node->GetDevice(c2_sw_node->GetNDevices() - 1));
		c2_sw->DisableLearning();
		c2_sw->SetSharedTraceMap(&shared_trace);
		
		for (auto cur = flowIds.begin(); cur != flowIds.end(); ++cur){
			int dst = cur->first.second;
			uint32_t flow = cur->second;
			switch (dst){
				case 0:
					c1_sw->AddForwardingRule(flow, toGpu0);
					c2_sw->AddForwardingRule(flow, devs1_17.Get(0));
					break;
				case 1:
					c1_sw->AddForwardingRule(flow, toGpu1);
					c2_sw->AddForwardingRule(flow, devs1_17.Get(0));
					break;
				case 2:
					c1_sw->AddForwardingRule(flow, toGpu2);
					c2_sw->AddForwardingRule(flow, devs1_17.Get(0));
					break;
				case 3:
					c1_sw->AddForwardingRule(flow, toGpu3);
					c2_sw->AddForwardingRule(flow, devs1_17.Get(0));
					break;
				case 4:
					c2_sw->AddForwardingRule(flow, toGpu4);
					c1_sw->AddForwardingRule(flow, devs1_16.Get(0));
					break;
				case 5:
					c2_sw->AddForwardingRule(flow, toGpu5);
					c1_sw->AddForwardingRule(flow, devs1_16.Get(0));
					break;
				case 6:
					c2_sw->AddForwardingRule(flow, toGpu6);
					c1_sw->AddForwardingRule(flow, devs1_16.Get(0));
					break;
				case 7:
					c2_sw->AddForwardingRule(flow, toGpu7);
					c1_sw->AddForwardingRule(flow, devs1_16.Get(0));
					break;
			}
		}

		Ptr<Node> sw1_node = swtches.Get(2);
		Ptr<SmartSwitch> sw1 = DynamicCast<SmartSwitch>(
				sw1_node->GetDevice(sw1_node->GetNDevices() - 1));
		sw1->DisableLearning();
		sw1->SetSharedTraceMap(&shared_trace);

		Ptr<Node> sw2_node = swtches.Get(3);
		Ptr<SmartSwitch> sw2 = DynamicCast<SmartSwitch>(
				sw2_node->GetDevice(sw2_node->GetNDevices() - 1));
		sw2->DisableLearning();
		sw2->SetSharedTraceMap(&shared_trace);

		Ptr<Node> sw3_node = swtches.Get(4);
		Ptr<SmartSwitch> sw3 = DynamicCast<SmartSwitch>(
				sw3_node->GetDevice(sw3_node->GetNDevices() - 1));
		sw3->DisableLearning();
		sw3->SetSharedTraceMap(&shared_trace);

		Ptr<Node> sw4_node = swtches.Get(5);
		Ptr<SmartSwitch> sw4 = DynamicCast<SmartSwitch>(
				sw4_node->GetDevice(sw4_node->GetNDevices() - 1));
		sw4->DisableLearning();
		sw4->SetSharedTraceMap(&shared_trace);
	  
		#if SCHEME == 0
		for (auto cur = flowIds.begin(); cur != flowIds.end(); ++cur){
			int src = cur->first.first;
			int dst = cur->first.second;
			uint32_t flow = cur->second;

			if (src < 4 && dst >= 4){
				sw1->AddForwardingRule(flow, devs1_19.Get(0));
				sw2->AddForwardingRule(flow, devs1_20.Get(0));
				sw3->AddForwardingRule(flow, devs1_17.Get(1));
			}
			else if (src >= 4 && dst < 4){
				sw1->AddForwardingRule(flow, devs1_16.Get(1));
				// sw2->AddForwardingRule(flow, devs1_19.Get(1));
				sw3->AddForwardingRule(flow, devs1_21.Get(0));
				sw4->AddForwardingRule(flow, devs1_18.Get(1));
			}
		}
		#elif SCHEME == 1
		for (auto cur = flowIds.begin(); cur != flowIds.end(); ++cur){
			int src = cur->first.first;
			int dst = cur->first.second;
			uint32_t flow = cur->second;

			if (src < 4 && dst >= 4){
				sw1->AddForwardingRule(flow, devs1_19.Get(0));
				sw2->AddForwardingRule(flow, devs1_20.Get(0));
				sw3->AddForwardingRule(flow, devs1_17.Get(1));
			}
			else if (src >= 4 && dst < 4){
				sw1->AddForwardingRule(flow, devs1_16.Get(1));
				sw2->AddForwardingRule(flow, devs1_19.Get(1));
				sw3->AddForwardingRule(flow, devs1_20.Get(1));
				// sw4->AddForwardingRule(flow, devs1_18.Get(1));
			}
		}

		#elif SCHEME == 2
		for (auto cur = flowIds.begin(); cur != flowIds.end(); ++cur){
			int src = cur->first.first;
			int dst = cur->first.second;
			uint32_t flow = cur->second;

			if (src < 4 && dst >= 4){
				sw3->AddForwardingRule(flow, devs1_17.Get(1));
				if ((src % 2) == 0) {
					sw1->AddForwardingRule(flow, devs1_19.Get(0));
					sw2->AddForwardingRule(flow, devs1_20.Get(0));	
				}
				else {
					sw1->AddForwardingRule(flow, devs1_18.Get(0));
					sw4->AddForwardingRule(flow, devs1_21.Get(1));				
				}
			}
			else if (src >= 4 && dst < 4){
				sw1->AddForwardingRule(flow, devs1_16.Get(1));
				if ((dst % 2) == 0) {
					sw2->AddForwardingRule(flow, devs1_19.Get(1));
					sw3->AddForwardingRule(flow, devs1_20.Get(1));
				}	
				else{
					sw3->AddForwardingRule(flow, devs1_21.Get(0));
					sw4->AddForwardingRule(flow, devs1_18.Get(1));
				}
			}
		}
		#endif

		// Allgather test
		CollectiveTester tester(apps, true, logtxt);
		tester.SetupAllgather(CHUNK_SIZE * N_CHUNKS, N_CHUNKS);
		// Run
		std::cout << "Starting simulation:" << std::endl;
    Simulator::Run();
		// Time and verify
		Time simTime = Simulator::Now();
		std::cout << "Total simulated time: "
          << simTime.GetNanoSeconds() << " nanoseconds" << std::endl;

		CollectiveTestResult allgather_res = tester.VerifyAllgather(CHUNK_SIZE * N_CHUNKS, N_CHUNKS);
		
		if (allgather_res == CollectiveTestResult::TEST_OK) std::cout << "Allgather verified." << std::endl;
		else std::cout << "Allgather incorrect." << std::endl;


    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
