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
    swtches.Create(0);
    PointToPointHelper link_helper0;
    link_helper0.SetDeviceAttribute("Mtu", UintegerValue(9000));
    link_helper0.SetChannelAttribute("Delay", StringValue("700ns"));
    link_helper0.SetDeviceAttribute("DataRate", StringValue("25GBps"));
    
    NetDeviceContainer devs0_0 = link_helper0.Install(gpunodes.Get(0), gpunodes.Get(1));
    
    DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(1, devs0_0.Get(0));
    DynamicCast<GPU>(gpunodes.Get(1))->PushRecvPeerDevice(0, devs0_0.Get(1));
    DynamicCast<GPU>(gpunodes.Get(1))->PushSendPeerDevice(0, devs0_0.Get(1));
    DynamicCast<GPU>(gpunodes.Get(0))->PushRecvPeerDevice(1, devs0_0.Get(0));
    DynamicCast<GPU>(gpunodes.Get(0))->PushPeerAddr(1, (devs0_0.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(1))->PushPeerAddr(0, (devs0_0.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_1 = link_helper0.Install(gpunodes.Get(1), gpunodes.Get(2));
    
    DynamicCast<GPU>(gpunodes.Get(1))->PushSendPeerDevice(2, devs0_1.Get(0));
    DynamicCast<GPU>(gpunodes.Get(2))->PushRecvPeerDevice(1, devs0_1.Get(1));
    DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(1, devs0_1.Get(1));
    DynamicCast<GPU>(gpunodes.Get(1))->PushRecvPeerDevice(2, devs0_1.Get(0));
    DynamicCast<GPU>(gpunodes.Get(1))->PushPeerAddr(2, (devs0_1.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(1, (devs0_1.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_2 = link_helper0.Install(gpunodes.Get(0), gpunodes.Get(2));
    
    DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(2, devs0_2.Get(0));
    DynamicCast<GPU>(gpunodes.Get(2))->PushRecvPeerDevice(0, devs0_2.Get(1));
    DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(0, devs0_2.Get(1));
    DynamicCast<GPU>(gpunodes.Get(0))->PushRecvPeerDevice(2, devs0_2.Get(0));
    DynamicCast<GPU>(gpunodes.Get(0))->PushPeerAddr(2, (devs0_2.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(0, (devs0_2.Get(0))->GetAddress());
    
    
    /*
        n0 -> n1: devs0_0
        n1 -> n2: devs0_1
        n0 -> n2: devs0_2
    */
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}