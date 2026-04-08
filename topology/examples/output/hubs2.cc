#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/bridge-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    NodeContainer gpunodes;
    NodeContainer swtches;
    
    gpunodes.Create<GPU>(8);
    swtches.Create(2);
    CsmaHelper csma0;
    csma0.SetChannelAttribute("Delay", StringValue("300ns"));
    csma0.SetChannelAttribute("DataRate", StringValue("125Gbps"));
    
    CsmaHelper csma1;
    csma1.SetChannelAttribute("Delay", StringValue("1500ns"));
    csma1.SetChannelAttribute("DataRate", StringValue("9Gbps"));
    
    NodeContainer nc0_0;
    nc0_0.Add(gpunodes.Get(0));
    nc0_0.Add(swtches.Get(0));
    NetDeviceContainer devs0_0 = csma0.Install(nc0_0);
    
    for (int i = 0; i < 8; ++i){
        if (i != 0){
            gpunodes.Get(0)->PushSendPeerDevice(i, devs0_0.Get(0));
        }
    }
    NodeContainer nc0_1;
    nc0_1.Add(gpunodes.Get(1));
    nc0_1.Add(swtches.Get(0));
    NetDeviceContainer devs0_1 = csma0.Install(nc0_1);
    
    for (int i = 0; i < 8; ++i){
        if (i != 1){
            gpunodes.Get(1)->PushSendPeerDevice(i, devs0_1.Get(0));
        }
    }
    NodeContainer nc0_2;
    nc0_2.Add(gpunodes.Get(2));
    nc0_2.Add(swtches.Get(0));
    NetDeviceContainer devs0_2 = csma0.Install(nc0_2);
    
    for (int i = 0; i < 8; ++i){
        if (i != 2){
            gpunodes.Get(2)->PushSendPeerDevice(i, devs0_2.Get(0));
        }
    }
    NodeContainer nc0_3;
    nc0_3.Add(gpunodes.Get(3));
    nc0_3.Add(swtches.Get(0));
    NetDeviceContainer devs0_3 = csma0.Install(nc0_3);
    
    for (int i = 0; i < 8; ++i){
        if (i != 3){
            gpunodes.Get(3)->PushSendPeerDevice(i, devs0_3.Get(0));
        }
    }
    NodeContainer nc0_4;
    nc0_4.Add(gpunodes.Get(4));
    nc0_4.Add(swtches.Get(1));
    NetDeviceContainer devs0_4 = csma0.Install(nc0_4);
    
    for (int i = 0; i < 8; ++i){
        if (i != 4){
            gpunodes.Get(4)->PushSendPeerDevice(i, devs0_4.Get(0));
        }
    }
    NodeContainer nc0_5;
    nc0_5.Add(gpunodes.Get(5));
    nc0_5.Add(swtches.Get(1));
    NetDeviceContainer devs0_5 = csma0.Install(nc0_5);
    
    for (int i = 0; i < 8; ++i){
        if (i != 5){
            gpunodes.Get(5)->PushSendPeerDevice(i, devs0_5.Get(0));
        }
    }
    NodeContainer nc0_6;
    nc0_6.Add(gpunodes.Get(6));
    nc0_6.Add(swtches.Get(1));
    NetDeviceContainer devs0_6 = csma0.Install(nc0_6);
    
    for (int i = 0; i < 8; ++i){
        if (i != 6){
            gpunodes.Get(6)->PushSendPeerDevice(i, devs0_6.Get(0));
        }
    }
    NodeContainer nc0_7;
    nc0_7.Add(gpunodes.Get(7));
    nc0_7.Add(swtches.Get(1));
    NetDeviceContainer devs0_7 = csma0.Install(nc0_7);
    
    for (int i = 0; i < 8; ++i){
        if (i != 7){
            gpunodes.Get(7)->PushSendPeerDevice(i, devs0_7.Get(0));
        }
    }
    NodeContainer nc1_8;
    nc1_8.Add(swtches.Get(0));
    nc1_8.Add(swtches.Get(1));
    NetDeviceContainer devs1_8 = csma1.Install(nc1_8);
    
    
    // Bridge setup for switches
    NetDeviceContainer bridgePorts_c1_sw;
    bridgePorts_c1_sw.Add(devs0_0.Get(1));
    bridgePorts_c1_sw.Add(devs0_1.Get(1));
    bridgePorts_c1_sw.Add(devs0_2.Get(1));
    bridgePorts_c1_sw.Add(devs0_3.Get(1));
    bridgePorts_c1_sw.Add(devs1_8.Get(0));
    BridgeHelper bridge_c1_sw;
    bridge_c1_sw.Install(swtches.Get(0), bridgePorts_c1_sw);
    
    NetDeviceContainer bridgePorts_c2_sw;
    bridgePorts_c2_sw.Add(devs0_4.Get(1));
    bridgePorts_c2_sw.Add(devs0_5.Get(1));
    bridgePorts_c2_sw.Add(devs0_6.Get(1));
    bridgePorts_c2_sw.Add(devs0_7.Get(1));
    bridgePorts_c2_sw.Add(devs1_8.Get(1));
    BridgeHelper bridge_c2_sw;
    bridge_c2_sw.Install(swtches.Get(1), bridgePorts_c2_sw);
    
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}