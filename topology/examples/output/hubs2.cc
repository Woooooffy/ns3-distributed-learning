#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/bridge-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    NodeContainer gpunodes;
    NodeContainer swtches;
    
    gpunodes.Create<GPU>(32);
    swtches.Create(4);
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
    
    for (int i = 0; i < 32; ++i){
        if (i != 0){
            gpunodes.Get(0)->PushSendPeerDevice(i, devs0_0.Get(0));
        }
    }
    NodeContainer nc0_1;
    nc0_1.Add(gpunodes.Get(1));
    nc0_1.Add(swtches.Get(0));
    NetDeviceContainer devs0_1 = csma0.Install(nc0_1);
    
    for (int i = 0; i < 32; ++i){
        if (i != 1){
            gpunodes.Get(1)->PushSendPeerDevice(i, devs0_1.Get(0));
        }
    }
    NodeContainer nc0_2;
    nc0_2.Add(gpunodes.Get(2));
    nc0_2.Add(swtches.Get(0));
    NetDeviceContainer devs0_2 = csma0.Install(nc0_2);
    
    for (int i = 0; i < 32; ++i){
        if (i != 2){
            gpunodes.Get(2)->PushSendPeerDevice(i, devs0_2.Get(0));
        }
    }
    NodeContainer nc0_3;
    nc0_3.Add(gpunodes.Get(3));
    nc0_3.Add(swtches.Get(0));
    NetDeviceContainer devs0_3 = csma0.Install(nc0_3);
    
    for (int i = 0; i < 32; ++i){
        if (i != 3){
            gpunodes.Get(3)->PushSendPeerDevice(i, devs0_3.Get(0));
        }
    }
    NodeContainer nc0_4;
    nc0_4.Add(gpunodes.Get(4));
    nc0_4.Add(swtches.Get(0));
    NetDeviceContainer devs0_4 = csma0.Install(nc0_4);
    
    for (int i = 0; i < 32; ++i){
        if (i != 4){
            gpunodes.Get(4)->PushSendPeerDevice(i, devs0_4.Get(0));
        }
    }
    NodeContainer nc0_5;
    nc0_5.Add(gpunodes.Get(5));
    nc0_5.Add(swtches.Get(0));
    NetDeviceContainer devs0_5 = csma0.Install(nc0_5);
    
    for (int i = 0; i < 32; ++i){
        if (i != 5){
            gpunodes.Get(5)->PushSendPeerDevice(i, devs0_5.Get(0));
        }
    }
    NodeContainer nc0_6;
    nc0_6.Add(gpunodes.Get(6));
    nc0_6.Add(swtches.Get(0));
    NetDeviceContainer devs0_6 = csma0.Install(nc0_6);
    
    for (int i = 0; i < 32; ++i){
        if (i != 6){
            gpunodes.Get(6)->PushSendPeerDevice(i, devs0_6.Get(0));
        }
    }
    NodeContainer nc0_7;
    nc0_7.Add(gpunodes.Get(7));
    nc0_7.Add(swtches.Get(0));
    NetDeviceContainer devs0_7 = csma0.Install(nc0_7);
    
    for (int i = 0; i < 32; ++i){
        if (i != 7){
            gpunodes.Get(7)->PushSendPeerDevice(i, devs0_7.Get(0));
        }
    }
    NodeContainer nc0_8;
    nc0_8.Add(gpunodes.Get(8));
    nc0_8.Add(swtches.Get(1));
    NetDeviceContainer devs0_8 = csma0.Install(nc0_8);
    
    for (int i = 0; i < 32; ++i){
        if (i != 8){
            gpunodes.Get(8)->PushSendPeerDevice(i, devs0_8.Get(0));
        }
    }
    NodeContainer nc0_9;
    nc0_9.Add(gpunodes.Get(9));
    nc0_9.Add(swtches.Get(1));
    NetDeviceContainer devs0_9 = csma0.Install(nc0_9);
    
    for (int i = 0; i < 32; ++i){
        if (i != 9){
            gpunodes.Get(9)->PushSendPeerDevice(i, devs0_9.Get(0));
        }
    }
    NodeContainer nc0_10;
    nc0_10.Add(gpunodes.Get(10));
    nc0_10.Add(swtches.Get(1));
    NetDeviceContainer devs0_10 = csma0.Install(nc0_10);
    
    for (int i = 0; i < 32; ++i){
        if (i != 10){
            gpunodes.Get(10)->PushSendPeerDevice(i, devs0_10.Get(0));
        }
    }
    NodeContainer nc0_11;
    nc0_11.Add(gpunodes.Get(11));
    nc0_11.Add(swtches.Get(1));
    NetDeviceContainer devs0_11 = csma0.Install(nc0_11);
    
    for (int i = 0; i < 32; ++i){
        if (i != 11){
            gpunodes.Get(11)->PushSendPeerDevice(i, devs0_11.Get(0));
        }
    }
    NodeContainer nc0_12;
    nc0_12.Add(gpunodes.Get(12));
    nc0_12.Add(swtches.Get(1));
    NetDeviceContainer devs0_12 = csma0.Install(nc0_12);
    
    for (int i = 0; i < 32; ++i){
        if (i != 12){
            gpunodes.Get(12)->PushSendPeerDevice(i, devs0_12.Get(0));
        }
    }
    NodeContainer nc0_13;
    nc0_13.Add(gpunodes.Get(13));
    nc0_13.Add(swtches.Get(1));
    NetDeviceContainer devs0_13 = csma0.Install(nc0_13);
    
    for (int i = 0; i < 32; ++i){
        if (i != 13){
            gpunodes.Get(13)->PushSendPeerDevice(i, devs0_13.Get(0));
        }
    }
    NodeContainer nc0_14;
    nc0_14.Add(gpunodes.Get(14));
    nc0_14.Add(swtches.Get(1));
    NetDeviceContainer devs0_14 = csma0.Install(nc0_14);
    
    for (int i = 0; i < 32; ++i){
        if (i != 14){
            gpunodes.Get(14)->PushSendPeerDevice(i, devs0_14.Get(0));
        }
    }
    NodeContainer nc0_15;
    nc0_15.Add(gpunodes.Get(15));
    nc0_15.Add(swtches.Get(1));
    NetDeviceContainer devs0_15 = csma0.Install(nc0_15);
    
    for (int i = 0; i < 32; ++i){
        if (i != 15){
            gpunodes.Get(15)->PushSendPeerDevice(i, devs0_15.Get(0));
        }
    }
    NodeContainer nc0_16;
    nc0_16.Add(gpunodes.Get(16));
    nc0_16.Add(swtches.Get(2));
    NetDeviceContainer devs0_16 = csma0.Install(nc0_16);
    
    for (int i = 0; i < 32; ++i){
        if (i != 16){
            gpunodes.Get(16)->PushSendPeerDevice(i, devs0_16.Get(0));
        }
    }
    NodeContainer nc0_17;
    nc0_17.Add(gpunodes.Get(17));
    nc0_17.Add(swtches.Get(2));
    NetDeviceContainer devs0_17 = csma0.Install(nc0_17);
    
    for (int i = 0; i < 32; ++i){
        if (i != 17){
            gpunodes.Get(17)->PushSendPeerDevice(i, devs0_17.Get(0));
        }
    }
    NodeContainer nc0_18;
    nc0_18.Add(gpunodes.Get(18));
    nc0_18.Add(swtches.Get(2));
    NetDeviceContainer devs0_18 = csma0.Install(nc0_18);
    
    for (int i = 0; i < 32; ++i){
        if (i != 18){
            gpunodes.Get(18)->PushSendPeerDevice(i, devs0_18.Get(0));
        }
    }
    NodeContainer nc0_19;
    nc0_19.Add(gpunodes.Get(19));
    nc0_19.Add(swtches.Get(2));
    NetDeviceContainer devs0_19 = csma0.Install(nc0_19);
    
    for (int i = 0; i < 32; ++i){
        if (i != 19){
            gpunodes.Get(19)->PushSendPeerDevice(i, devs0_19.Get(0));
        }
    }
    NodeContainer nc0_20;
    nc0_20.Add(gpunodes.Get(20));
    nc0_20.Add(swtches.Get(2));
    NetDeviceContainer devs0_20 = csma0.Install(nc0_20);
    
    for (int i = 0; i < 32; ++i){
        if (i != 20){
            gpunodes.Get(20)->PushSendPeerDevice(i, devs0_20.Get(0));
        }
    }
    NodeContainer nc0_21;
    nc0_21.Add(gpunodes.Get(21));
    nc0_21.Add(swtches.Get(2));
    NetDeviceContainer devs0_21 = csma0.Install(nc0_21);
    
    for (int i = 0; i < 32; ++i){
        if (i != 21){
            gpunodes.Get(21)->PushSendPeerDevice(i, devs0_21.Get(0));
        }
    }
    NodeContainer nc0_22;
    nc0_22.Add(gpunodes.Get(22));
    nc0_22.Add(swtches.Get(2));
    NetDeviceContainer devs0_22 = csma0.Install(nc0_22);
    
    for (int i = 0; i < 32; ++i){
        if (i != 22){
            gpunodes.Get(22)->PushSendPeerDevice(i, devs0_22.Get(0));
        }
    }
    NodeContainer nc0_23;
    nc0_23.Add(gpunodes.Get(23));
    nc0_23.Add(swtches.Get(2));
    NetDeviceContainer devs0_23 = csma0.Install(nc0_23);
    
    for (int i = 0; i < 32; ++i){
        if (i != 23){
            gpunodes.Get(23)->PushSendPeerDevice(i, devs0_23.Get(0));
        }
    }
    NodeContainer nc0_24;
    nc0_24.Add(gpunodes.Get(24));
    nc0_24.Add(swtches.Get(3));
    NetDeviceContainer devs0_24 = csma0.Install(nc0_24);
    
    for (int i = 0; i < 32; ++i){
        if (i != 24){
            gpunodes.Get(24)->PushSendPeerDevice(i, devs0_24.Get(0));
        }
    }
    NodeContainer nc0_25;
    nc0_25.Add(gpunodes.Get(25));
    nc0_25.Add(swtches.Get(3));
    NetDeviceContainer devs0_25 = csma0.Install(nc0_25);
    
    for (int i = 0; i < 32; ++i){
        if (i != 25){
            gpunodes.Get(25)->PushSendPeerDevice(i, devs0_25.Get(0));
        }
    }
    NodeContainer nc0_26;
    nc0_26.Add(gpunodes.Get(26));
    nc0_26.Add(swtches.Get(3));
    NetDeviceContainer devs0_26 = csma0.Install(nc0_26);
    
    for (int i = 0; i < 32; ++i){
        if (i != 26){
            gpunodes.Get(26)->PushSendPeerDevice(i, devs0_26.Get(0));
        }
    }
    NodeContainer nc0_27;
    nc0_27.Add(gpunodes.Get(27));
    nc0_27.Add(swtches.Get(3));
    NetDeviceContainer devs0_27 = csma0.Install(nc0_27);
    
    for (int i = 0; i < 32; ++i){
        if (i != 27){
            gpunodes.Get(27)->PushSendPeerDevice(i, devs0_27.Get(0));
        }
    }
    NodeContainer nc0_28;
    nc0_28.Add(gpunodes.Get(28));
    nc0_28.Add(swtches.Get(3));
    NetDeviceContainer devs0_28 = csma0.Install(nc0_28);
    
    for (int i = 0; i < 32; ++i){
        if (i != 28){
            gpunodes.Get(28)->PushSendPeerDevice(i, devs0_28.Get(0));
        }
    }
    NodeContainer nc0_29;
    nc0_29.Add(gpunodes.Get(29));
    nc0_29.Add(swtches.Get(3));
    NetDeviceContainer devs0_29 = csma0.Install(nc0_29);
    
    for (int i = 0; i < 32; ++i){
        if (i != 29){
            gpunodes.Get(29)->PushSendPeerDevice(i, devs0_29.Get(0));
        }
    }
    NodeContainer nc0_30;
    nc0_30.Add(gpunodes.Get(30));
    nc0_30.Add(swtches.Get(3));
    NetDeviceContainer devs0_30 = csma0.Install(nc0_30);
    
    for (int i = 0; i < 32; ++i){
        if (i != 30){
            gpunodes.Get(30)->PushSendPeerDevice(i, devs0_30.Get(0));
        }
    }
    NodeContainer nc0_31;
    nc0_31.Add(gpunodes.Get(31));
    nc0_31.Add(swtches.Get(3));
    NetDeviceContainer devs0_31 = csma0.Install(nc0_31);
    
    for (int i = 0; i < 32; ++i){
        if (i != 31){
            gpunodes.Get(31)->PushSendPeerDevice(i, devs0_31.Get(0));
        }
    }
    NodeContainer nc1_32;
    nc1_32.Add(swtches.Get(0));
    nc1_32.Add(swtches.Get(1));
    NetDeviceContainer devs1_32 = csma1.Install(nc1_32);
    
    NodeContainer nc1_33;
    nc1_33.Add(swtches.Get(0));
    nc1_33.Add(swtches.Get(2));
    NetDeviceContainer devs1_33 = csma1.Install(nc1_33);
    
    NodeContainer nc1_34;
    nc1_34.Add(swtches.Get(0));
    nc1_34.Add(swtches.Get(3));
    NetDeviceContainer devs1_34 = csma1.Install(nc1_34);
    
    NodeContainer nc1_35;
    nc1_35.Add(swtches.Get(1));
    nc1_35.Add(swtches.Get(0));
    NetDeviceContainer devs1_35 = csma1.Install(nc1_35);
    
    NodeContainer nc1_36;
    nc1_36.Add(swtches.Get(1));
    nc1_36.Add(swtches.Get(2));
    NetDeviceContainer devs1_36 = csma1.Install(nc1_36);
    
    NodeContainer nc1_37;
    nc1_37.Add(swtches.Get(1));
    nc1_37.Add(swtches.Get(3));
    NetDeviceContainer devs1_37 = csma1.Install(nc1_37);
    
    NodeContainer nc1_38;
    nc1_38.Add(swtches.Get(2));
    nc1_38.Add(swtches.Get(0));
    NetDeviceContainer devs1_38 = csma1.Install(nc1_38);
    
    NodeContainer nc1_39;
    nc1_39.Add(swtches.Get(2));
    nc1_39.Add(swtches.Get(1));
    NetDeviceContainer devs1_39 = csma1.Install(nc1_39);
    
    NodeContainer nc1_40;
    nc1_40.Add(swtches.Get(2));
    nc1_40.Add(swtches.Get(3));
    NetDeviceContainer devs1_40 = csma1.Install(nc1_40);
    
    NodeContainer nc1_41;
    nc1_41.Add(swtches.Get(3));
    nc1_41.Add(swtches.Get(0));
    NetDeviceContainer devs1_41 = csma1.Install(nc1_41);
    
    NodeContainer nc1_42;
    nc1_42.Add(swtches.Get(3));
    nc1_42.Add(swtches.Get(1));
    NetDeviceContainer devs1_42 = csma1.Install(nc1_42);
    
    NodeContainer nc1_43;
    nc1_43.Add(swtches.Get(3));
    nc1_43.Add(swtches.Get(2));
    NetDeviceContainer devs1_43 = csma1.Install(nc1_43);
    
    
    // Bridge setup for switches
    NetDeviceContainer bridgePorts_c1_sw;
    bridgePorts_c1_sw.Add(devs0_0.Get(1));
    bridgePorts_c1_sw.Add(devs0_1.Get(1));
    bridgePorts_c1_sw.Add(devs0_2.Get(1));
    bridgePorts_c1_sw.Add(devs0_3.Get(1));
    bridgePorts_c1_sw.Add(devs0_4.Get(1));
    bridgePorts_c1_sw.Add(devs0_5.Get(1));
    bridgePorts_c1_sw.Add(devs0_6.Get(1));
    bridgePorts_c1_sw.Add(devs0_7.Get(1));
    bridgePorts_c1_sw.Add(devs1_32.Get(0));
    bridgePorts_c1_sw.Add(devs1_33.Get(0));
    bridgePorts_c1_sw.Add(devs1_34.Get(0));
    bridgePorts_c1_sw.Add(devs1_35.Get(1));
    bridgePorts_c1_sw.Add(devs1_38.Get(1));
    bridgePorts_c1_sw.Add(devs1_41.Get(1));
    BridgeHelper bridge_c1_sw;
    bridge_c1_sw.Install(swtches.Get(0), bridgePorts_c1_sw);
    
    NetDeviceContainer bridgePorts_c2_sw;
    bridgePorts_c2_sw.Add(devs0_8.Get(1));
    bridgePorts_c2_sw.Add(devs0_9.Get(1));
    bridgePorts_c2_sw.Add(devs0_10.Get(1));
    bridgePorts_c2_sw.Add(devs0_11.Get(1));
    bridgePorts_c2_sw.Add(devs0_12.Get(1));
    bridgePorts_c2_sw.Add(devs0_13.Get(1));
    bridgePorts_c2_sw.Add(devs0_14.Get(1));
    bridgePorts_c2_sw.Add(devs0_15.Get(1));
    bridgePorts_c2_sw.Add(devs1_32.Get(1));
    bridgePorts_c2_sw.Add(devs1_35.Get(0));
    bridgePorts_c2_sw.Add(devs1_36.Get(0));
    bridgePorts_c2_sw.Add(devs1_37.Get(0));
    bridgePorts_c2_sw.Add(devs1_39.Get(1));
    bridgePorts_c2_sw.Add(devs1_42.Get(1));
    BridgeHelper bridge_c2_sw;
    bridge_c2_sw.Install(swtches.Get(1), bridgePorts_c2_sw);
    
    NetDeviceContainer bridgePorts_c3_sw;
    bridgePorts_c3_sw.Add(devs0_16.Get(1));
    bridgePorts_c3_sw.Add(devs0_17.Get(1));
    bridgePorts_c3_sw.Add(devs0_18.Get(1));
    bridgePorts_c3_sw.Add(devs0_19.Get(1));
    bridgePorts_c3_sw.Add(devs0_20.Get(1));
    bridgePorts_c3_sw.Add(devs0_21.Get(1));
    bridgePorts_c3_sw.Add(devs0_22.Get(1));
    bridgePorts_c3_sw.Add(devs0_23.Get(1));
    bridgePorts_c3_sw.Add(devs1_33.Get(1));
    bridgePorts_c3_sw.Add(devs1_36.Get(1));
    bridgePorts_c3_sw.Add(devs1_38.Get(0));
    bridgePorts_c3_sw.Add(devs1_39.Get(0));
    bridgePorts_c3_sw.Add(devs1_40.Get(0));
    bridgePorts_c3_sw.Add(devs1_43.Get(1));
    BridgeHelper bridge_c3_sw;
    bridge_c3_sw.Install(swtches.Get(2), bridgePorts_c3_sw);
    
    NetDeviceContainer bridgePorts_c4_sw;
    bridgePorts_c4_sw.Add(devs0_24.Get(1));
    bridgePorts_c4_sw.Add(devs0_25.Get(1));
    bridgePorts_c4_sw.Add(devs0_26.Get(1));
    bridgePorts_c4_sw.Add(devs0_27.Get(1));
    bridgePorts_c4_sw.Add(devs0_28.Get(1));
    bridgePorts_c4_sw.Add(devs0_29.Get(1));
    bridgePorts_c4_sw.Add(devs0_30.Get(1));
    bridgePorts_c4_sw.Add(devs0_31.Get(1));
    bridgePorts_c4_sw.Add(devs1_34.Get(1));
    bridgePorts_c4_sw.Add(devs1_37.Get(1));
    bridgePorts_c4_sw.Add(devs1_40.Get(1));
    bridgePorts_c4_sw.Add(devs1_41.Get(0));
    bridgePorts_c4_sw.Add(devs1_42.Get(0));
    bridgePorts_c4_sw.Add(devs1_43.Get(0));
    BridgeHelper bridge_c4_sw;
    bridge_c4_sw.Install(swtches.Get(3), bridgePorts_c4_sw);
    
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}