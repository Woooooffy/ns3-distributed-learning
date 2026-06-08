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
    swtches.Create(0);
    PointToPointHelper link_helper0;
    link_helper0.SetDeviceAttribute("Mtu", UintegerValue(9000));
    link_helper0.SetChannelAttribute("Delay", StringValue("700ns"));
    link_helper0.SetDeviceAttribute("DataRate", StringValue("25GBps"));
    
    NetDeviceContainer devs0_0 = link_helper0.Install(gpunodes.Get(0), gpunodes.Get(1));
    
    DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(1, devs0_0.Get(0));
    DynamicCast<GPU>(gpunodes.Get(1))->PushRecvPeerDevice(0, devs0_0.Get(1));
    DynamicCast<GPU>(gpunodes.Get(0))->PushPeerAddr(1, (devs0_0.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(1))->PushPeerAddr(0, (devs0_0.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_1 = link_helper0.Install(gpunodes.Get(2), gpunodes.Get(3));
    
    DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(3, devs0_1.Get(0));
    DynamicCast<GPU>(gpunodes.Get(3))->PushRecvPeerDevice(2, devs0_1.Get(1));
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(3, (devs0_1.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(3))->PushPeerAddr(2, (devs0_1.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_2 = link_helper0.Install(gpunodes.Get(1), gpunodes.Get(3));
    
    DynamicCast<GPU>(gpunodes.Get(1))->PushSendPeerDevice(3, devs0_2.Get(0));
    DynamicCast<GPU>(gpunodes.Get(3))->PushRecvPeerDevice(1, devs0_2.Get(1));
    DynamicCast<GPU>(gpunodes.Get(1))->PushPeerAddr(3, (devs0_2.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(3))->PushPeerAddr(1, (devs0_2.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_3 = link_helper0.Install(gpunodes.Get(0), gpunodes.Get(1));
    
    DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(1, devs0_3.Get(0));
    DynamicCast<GPU>(gpunodes.Get(1))->PushRecvPeerDevice(0, devs0_3.Get(1));
    DynamicCast<GPU>(gpunodes.Get(0))->PushPeerAddr(1, (devs0_3.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(1))->PushPeerAddr(0, (devs0_3.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_4 = link_helper0.Install(gpunodes.Get(2), gpunodes.Get(3));
    
    DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(3, devs0_4.Get(0));
    DynamicCast<GPU>(gpunodes.Get(3))->PushRecvPeerDevice(2, devs0_4.Get(1));
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(3, (devs0_4.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(3))->PushPeerAddr(2, (devs0_4.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_5 = link_helper0.Install(gpunodes.Get(1), gpunodes.Get(3));
    
    DynamicCast<GPU>(gpunodes.Get(1))->PushSendPeerDevice(3, devs0_5.Get(0));
    DynamicCast<GPU>(gpunodes.Get(3))->PushRecvPeerDevice(1, devs0_5.Get(1));
    DynamicCast<GPU>(gpunodes.Get(1))->PushPeerAddr(3, (devs0_5.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(3))->PushPeerAddr(1, (devs0_5.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_6 = link_helper0.Install(gpunodes.Get(0), gpunodes.Get(2));
    
    DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(2, devs0_6.Get(0));
    DynamicCast<GPU>(gpunodes.Get(2))->PushRecvPeerDevice(0, devs0_6.Get(1));
    DynamicCast<GPU>(gpunodes.Get(0))->PushPeerAddr(2, (devs0_6.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(0, (devs0_6.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_7 = link_helper0.Install(gpunodes.Get(0), gpunodes.Get(3));
    
    DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(3, devs0_7.Get(0));
    DynamicCast<GPU>(gpunodes.Get(3))->PushRecvPeerDevice(0, devs0_7.Get(1));
    DynamicCast<GPU>(gpunodes.Get(0))->PushPeerAddr(3, (devs0_7.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(3))->PushPeerAddr(0, (devs0_7.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_8 = link_helper0.Install(gpunodes.Get(2), gpunodes.Get(1));
    
    DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(1, devs0_8.Get(0));
    DynamicCast<GPU>(gpunodes.Get(1))->PushRecvPeerDevice(2, devs0_8.Get(1));
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(1, (devs0_8.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(1))->PushPeerAddr(2, (devs0_8.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_9 = link_helper0.Install(gpunodes.Get(4), gpunodes.Get(5));
    
    DynamicCast<GPU>(gpunodes.Get(4))->PushSendPeerDevice(5, devs0_9.Get(0));
    DynamicCast<GPU>(gpunodes.Get(5))->PushRecvPeerDevice(4, devs0_9.Get(1));
    DynamicCast<GPU>(gpunodes.Get(4))->PushPeerAddr(5, (devs0_9.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(5))->PushPeerAddr(4, (devs0_9.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_10 = link_helper0.Install(gpunodes.Get(6), gpunodes.Get(7));
    
    DynamicCast<GPU>(gpunodes.Get(6))->PushSendPeerDevice(7, devs0_10.Get(0));
    DynamicCast<GPU>(gpunodes.Get(7))->PushRecvPeerDevice(6, devs0_10.Get(1));
    DynamicCast<GPU>(gpunodes.Get(6))->PushPeerAddr(7, (devs0_10.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(7))->PushPeerAddr(6, (devs0_10.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_11 = link_helper0.Install(gpunodes.Get(5), gpunodes.Get(7));
    
    DynamicCast<GPU>(gpunodes.Get(5))->PushSendPeerDevice(7, devs0_11.Get(0));
    DynamicCast<GPU>(gpunodes.Get(7))->PushRecvPeerDevice(5, devs0_11.Get(1));
    DynamicCast<GPU>(gpunodes.Get(5))->PushPeerAddr(7, (devs0_11.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(7))->PushPeerAddr(5, (devs0_11.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_12 = link_helper0.Install(gpunodes.Get(4), gpunodes.Get(5));
    
    DynamicCast<GPU>(gpunodes.Get(4))->PushSendPeerDevice(5, devs0_12.Get(0));
    DynamicCast<GPU>(gpunodes.Get(5))->PushRecvPeerDevice(4, devs0_12.Get(1));
    DynamicCast<GPU>(gpunodes.Get(4))->PushPeerAddr(5, (devs0_12.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(5))->PushPeerAddr(4, (devs0_12.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_13 = link_helper0.Install(gpunodes.Get(6), gpunodes.Get(7));
    
    DynamicCast<GPU>(gpunodes.Get(6))->PushSendPeerDevice(7, devs0_13.Get(0));
    DynamicCast<GPU>(gpunodes.Get(7))->PushRecvPeerDevice(6, devs0_13.Get(1));
    DynamicCast<GPU>(gpunodes.Get(6))->PushPeerAddr(7, (devs0_13.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(7))->PushPeerAddr(6, (devs0_13.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_14 = link_helper0.Install(gpunodes.Get(5), gpunodes.Get(7));
    
    DynamicCast<GPU>(gpunodes.Get(5))->PushSendPeerDevice(7, devs0_14.Get(0));
    DynamicCast<GPU>(gpunodes.Get(7))->PushRecvPeerDevice(5, devs0_14.Get(1));
    DynamicCast<GPU>(gpunodes.Get(5))->PushPeerAddr(7, (devs0_14.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(7))->PushPeerAddr(5, (devs0_14.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_15 = link_helper0.Install(gpunodes.Get(4), gpunodes.Get(6));
    
    DynamicCast<GPU>(gpunodes.Get(4))->PushSendPeerDevice(6, devs0_15.Get(0));
    DynamicCast<GPU>(gpunodes.Get(6))->PushRecvPeerDevice(4, devs0_15.Get(1));
    DynamicCast<GPU>(gpunodes.Get(4))->PushPeerAddr(6, (devs0_15.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(6))->PushPeerAddr(4, (devs0_15.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_16 = link_helper0.Install(gpunodes.Get(4), gpunodes.Get(7));
    
    DynamicCast<GPU>(gpunodes.Get(4))->PushSendPeerDevice(7, devs0_16.Get(0));
    DynamicCast<GPU>(gpunodes.Get(7))->PushRecvPeerDevice(4, devs0_16.Get(1));
    DynamicCast<GPU>(gpunodes.Get(4))->PushPeerAddr(7, (devs0_16.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(7))->PushPeerAddr(4, (devs0_16.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_17 = link_helper0.Install(gpunodes.Get(6), gpunodes.Get(5));
    
    DynamicCast<GPU>(gpunodes.Get(6))->PushSendPeerDevice(5, devs0_17.Get(0));
    DynamicCast<GPU>(gpunodes.Get(5))->PushRecvPeerDevice(6, devs0_17.Get(1));
    DynamicCast<GPU>(gpunodes.Get(6))->PushPeerAddr(5, (devs0_17.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(5))->PushPeerAddr(6, (devs0_17.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_18 = link_helper0.Install(gpunodes.Get(0), gpunodes.Get(4));
    
    DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(4, devs0_18.Get(0));
    DynamicCast<GPU>(gpunodes.Get(4))->PushRecvPeerDevice(0, devs0_18.Get(1));
    DynamicCast<GPU>(gpunodes.Get(0))->PushPeerAddr(4, (devs0_18.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(4))->PushPeerAddr(0, (devs0_18.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_19 = link_helper0.Install(gpunodes.Get(2), gpunodes.Get(6));
    
    DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(6, devs0_19.Get(0));
    DynamicCast<GPU>(gpunodes.Get(6))->PushRecvPeerDevice(2, devs0_19.Get(1));
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(6, (devs0_19.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(6))->PushPeerAddr(2, (devs0_19.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_20 = link_helper0.Install(gpunodes.Get(0), gpunodes.Get(4));
    
    DynamicCast<GPU>(gpunodes.Get(0))->PushSendPeerDevice(4, devs0_20.Get(0));
    DynamicCast<GPU>(gpunodes.Get(4))->PushRecvPeerDevice(0, devs0_20.Get(1));
    DynamicCast<GPU>(gpunodes.Get(0))->PushPeerAddr(4, (devs0_20.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(4))->PushPeerAddr(0, (devs0_20.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_21 = link_helper0.Install(gpunodes.Get(2), gpunodes.Get(6));
    
    DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(6, devs0_21.Get(0));
    DynamicCast<GPU>(gpunodes.Get(6))->PushRecvPeerDevice(2, devs0_21.Get(1));
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(6, (devs0_21.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(6))->PushPeerAddr(2, (devs0_21.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_22 = link_helper0.Install(gpunodes.Get(1), gpunodes.Get(5));
    
    DynamicCast<GPU>(gpunodes.Get(1))->PushSendPeerDevice(5, devs0_22.Get(0));
    DynamicCast<GPU>(gpunodes.Get(5))->PushRecvPeerDevice(1, devs0_22.Get(1));
    DynamicCast<GPU>(gpunodes.Get(1))->PushPeerAddr(5, (devs0_22.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(5))->PushPeerAddr(1, (devs0_22.Get(0))->GetAddress());
    
    NetDeviceContainer devs0_23 = link_helper0.Install(gpunodes.Get(2), gpunodes.Get(6));
    
    DynamicCast<GPU>(gpunodes.Get(2))->PushSendPeerDevice(6, devs0_23.Get(0));
    DynamicCast<GPU>(gpunodes.Get(6))->PushRecvPeerDevice(2, devs0_23.Get(1));
    DynamicCast<GPU>(gpunodes.Get(2))->PushPeerAddr(6, (devs0_23.Get(1))->GetAddress());
    DynamicCast<GPU>(gpunodes.Get(6))->PushPeerAddr(2, (devs0_23.Get(0))->GetAddress());
    
    
    /*
        c1_n0 -> c1_n1: devs0_3
        c1_n2 -> c1_n3: devs0_4
        c1_n1 -> c1_n3: devs0_5
        c1_n0 -> c1_n2: devs0_6
        c1_n0 -> c1_n3: devs0_7
        c1_n2 -> c1_n1: devs0_8
        c2_n0 -> c2_n1: devs0_12
        c2_n2 -> c2_n3: devs0_13
        c2_n1 -> c2_n3: devs0_14
        c2_n0 -> c2_n2: devs0_15
        c2_n0 -> c2_n3: devs0_16
        c2_n2 -> c2_n1: devs0_17
        c1_n0 -> c2_n0: devs0_20
        c1_n2 -> c2_n2: devs0_23
        c1_n1 -> c2_n1: devs0_22
    */
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}