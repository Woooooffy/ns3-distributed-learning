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
    
    gpunodes.Create<GPU>(3);
    swtches.Create(1);
    SwitchedEthernetHelper link_helper0;
    link_helper0.SetDeviceAttribute("Mtu", UintegerValue(9000));
    link_helper0.SetChannelAttribute("Delay", StringValue("1us"));
    link_helper0.SetChannelAttribute("DataRate", StringValue("200Gbps"));
    
    sw_helper.SetDeviceAttribute("Mtu", UintegerValue(9000));
    NetDeviceContainer sw_dev0 = sw_helper.Install(swtches.Get(0));
    Ptr<P4SwitchNetDevice> sw0 = DynamicCast<P4SwitchNetDevice>(sw_dev0.Get(0));
    
    NetDeviceContainer devs0_0 = link_helper0.ConnectHost(sw0, gpunodes.Get(0));
    
    for (int i = 0; i < 3; ++i){
        if (i != 0){
            DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(i, devs0_0.Get(0));
            DynamicCast<GPU>(gpunodes.Get(0))->PushRecvPeerDevice(i, devs0_0.Get(0));
        }
    }
    
    for (int i = 0; i < 3; ++i){
        if (i != 0){
            DynamicCast<GPU>(gpunodes.Get(i))->PushPeerAddr(0, devs0_0.Get(0)->GetAddress());
        }
    }
    
    NetDeviceContainer devs0_1 = link_helper0.ConnectHost(sw0, gpunodes.Get(1));
    
    for (int i = 0; i < 3; ++i){
        if (i != 1){
            DynamicCast<GPU>(gpunodes.Get(1))->PushSendPeerDevice(i, devs0_1.Get(0));
            DynamicCast<GPU>(gpunodes.Get(1))->PushRecvPeerDevice(i, devs0_1.Get(0));
        }
    }
    
    for (int i = 0; i < 3; ++i){
        if (i != 1){
            DynamicCast<GPU>(gpunodes.Get(i))->PushPeerAddr(1, devs0_1.Get(0)->GetAddress());
        }
    }
    
    NetDeviceContainer devs0_2 = link_helper0.ConnectHost(sw0, gpunodes.Get(2));
    
    for (int i = 0; i < 3; ++i){
        if (i != 2){
            DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(i, devs0_2.Get(0));
            DynamicCast<GPU>(gpunodes.Get(2))->PushRecvPeerDevice(i, devs0_2.Get(0));
        }
    }
    
    for (int i = 0; i < 3; ++i){
        if (i != 2){
            DynamicCast<GPU>(gpunodes.Get(i))->PushPeerAddr(2, devs0_2.Get(0)->GetAddress());
        }
    }
    
    
    /*
    */
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}