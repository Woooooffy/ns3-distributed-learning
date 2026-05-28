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
    swtches.Create(2);
    SwitchedEthernetHelper link_helper0;
    link_helper0.SetDeviceAttribute("Mtu", UintegerValue(9000));
    link_helper0.SetChannelAttribute("Delay", StringValue("300ns"));
    link_helper0.SetChannelAttribute("DataRate", StringValue("125Gbps"));
    
    SwitchedEthernetHelper link_helper1;
    link_helper1.SetDeviceAttribute("Mtu", UintegerValue(9000));
    link_helper1.SetChannelAttribute("Delay", StringValue("1500ns"));
    link_helper1.SetChannelAttribute("DataRate", StringValue("9Gbps"));
    
    sw_helper.SetDeviceAttribute("Mtu", UintegerValue(9000);
    NetDeviceContainer sw_dev0 = sw_helper.Install(swtches.Get(0));
    Ptr<P4SwitchNetDevice> sw0 = DynamicCast<P4SwitchNetDevice>(sw_dev0.Get(0));
    NetDeviceContainer sw_dev1 = sw_helper.Install(swtches.Get(1));
    Ptr<P4SwitchNetDevice> sw1 = DynamicCast<P4SwitchNetDevice>(sw_dev1.Get(0));
    
    NetDeviceContainer devs0_0 = link_helper0.Install(sw0, gpunodes.Get(0));
    
    for (int i = 0; i < 8; ++i){
        if (i != 0){
            DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(i, devs0_0.Get(0));
            DynamicCast<GPU>(gpunodes.Get(0))->PushRecvPeerDevice(i, devs0_0.Get(0));
        }
    }
    
    NetDeviceContainer devs0_1 = link_helper0.Install(sw0, gpunodes.Get(1));
    
    for (int i = 0; i < 8; ++i){
        if (i != 1){
            DynamicCast<GPU>(gpunodes.Get(1))->PushSendPeerDevice(i, devs0_1.Get(0));
            DynamicCast<GPU>(gpunodes.Get(1))->PushRecvPeerDevice(i, devs0_1.Get(0));
        }
    }
    
    NetDeviceContainer devs0_2 = link_helper0.Install(sw0, gpunodes.Get(2));
    
    for (int i = 0; i < 8; ++i){
        if (i != 2){
            DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(i, devs0_2.Get(0));
            DynamicCast<GPU>(gpunodes.Get(2))->PushRecvPeerDevice(i, devs0_2.Get(0));
        }
    }
    
    NetDeviceContainer devs0_3 = link_helper0.Install(sw0, gpunodes.Get(3));
    
    for (int i = 0; i < 8; ++i){
        if (i != 3){
            DynamicCast<GPU>(gpunodes.Get(3))->PushSendPeerDevice(i, devs0_3.Get(0));
            DynamicCast<GPU>(gpunodes.Get(3))->PushRecvPeerDevice(i, devs0_3.Get(0));
        }
    }
    
    NetDeviceContainer devs0_4 = link_helper0.Install(sw1, gpunodes.Get(4));
    
    for (int i = 0; i < 8; ++i){
        if (i != 4){
            DynamicCast<GPU>(gpunodes.Get(4))->PushSendPeerDevice(i, devs0_4.Get(0));
            DynamicCast<GPU>(gpunodes.Get(4))->PushRecvPeerDevice(i, devs0_4.Get(0));
        }
    }
    
    NetDeviceContainer devs0_5 = link_helper0.Install(sw1, gpunodes.Get(5));
    
    for (int i = 0; i < 8; ++i){
        if (i != 5){
            DynamicCast<GPU>(gpunodes.Get(5))->PushSendPeerDevice(i, devs0_5.Get(0));
            DynamicCast<GPU>(gpunodes.Get(5))->PushRecvPeerDevice(i, devs0_5.Get(0));
        }
    }
    
    NetDeviceContainer devs0_6 = link_helper0.Install(sw1, gpunodes.Get(6));
    
    for (int i = 0; i < 8; ++i){
        if (i != 6){
            DynamicCast<GPU>(gpunodes.Get(6))->PushSendPeerDevice(i, devs0_6.Get(0));
            DynamicCast<GPU>(gpunodes.Get(6))->PushRecvPeerDevice(i, devs0_6.Get(0));
        }
    }
    
    NetDeviceContainer devs0_7 = link_helper0.Install(sw1, gpunodes.Get(7));
    
    for (int i = 0; i < 8; ++i){
        if (i != 7){
            DynamicCast<GPU>(gpunodes.Get(7))->PushSendPeerDevice(i, devs0_7.Get(0));
            DynamicCast<GPU>(gpunodes.Get(7))->PushRecvPeerDevice(i, devs0_7.Get(0));
        }
    }
    
    link_helper1.ConnectSwitches(sw0, sw1);
    
    
    /*
    */
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}