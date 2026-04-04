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
    
    NodeContainer nc0_21;
    nc0_22.Add(gpunodes.Get(0));
    nc0_23.Add(swtches.Get(0));
    NetDeviceContainer devs0_24 = csma0.Install(nc0_24);
    
    NodeContainer nc0_26;
    nc0_27.Add(gpunodes.Get(1));
    nc0_28.Add(swtches.Get(0));
    NetDeviceContainer devs0_29 = csma0.Install(nc0_29);
    
    NodeContainer nc0_31;
    nc0_32.Add(gpunodes.Get(2));
    nc0_33.Add(swtches.Get(0));
    NetDeviceContainer devs0_34 = csma0.Install(nc0_34);
    
    NodeContainer nc0_36;
    nc0_37.Add(gpunodes.Get(3));
    nc0_38.Add(swtches.Get(0));
    NetDeviceContainer devs0_39 = csma0.Install(nc0_39);
    
    NodeContainer nc0_41;
    nc0_42.Add(gpunodes.Get(4));
    nc0_43.Add(swtches.Get(1));
    NetDeviceContainer devs0_44 = csma0.Install(nc0_44);
    
    NodeContainer nc0_46;
    nc0_47.Add(gpunodes.Get(5));
    nc0_48.Add(swtches.Get(1));
    NetDeviceContainer devs0_49 = csma0.Install(nc0_49);
    
    NodeContainer nc0_51;
    nc0_52.Add(gpunodes.Get(6));
    nc0_53.Add(swtches.Get(1));
    NetDeviceContainer devs0_54 = csma0.Install(nc0_54);
    
    NodeContainer nc0_56;
    nc0_57.Add(gpunodes.Get(7));
    nc0_58.Add(swtches.Get(1));
    NetDeviceContainer devs0_59 = csma0.Install(nc0_59);
    
    NodeContainer nc1_61;
    nc1_62.Add(swtches.Get(0));
    nc1_63.Add(swtches.Get(1));
    NetDeviceContainer devs1_64 = csma1.Install(nc1_64);
    
    
    // Bridge setup for switches
    NetDeviceContainer bridgePorts_c1_sw;
    bridgePorts_c1_sw.Add(devs0_26.Get(1));
    bridgePorts_c1_sw.Add(devs0_31.Get(1));
    bridgePorts_c1_sw.Add(devs0_36.Get(1));
    bridgePorts_c1_sw.Add(devs0_41.Get(1));
    bridgePorts_c1_sw.Add(devs1_66.Get(0));
    BridgeHelper bridge_c1_sw;
    bridge_c1_sw.Install(swtches.Get(0), bridgePorts_c1_sw);
    
    NetDeviceContainer bridgePorts_c2_sw;
    bridgePorts_c2_sw.Add(devs0_46.Get(1));
    bridgePorts_c2_sw.Add(devs0_51.Get(1));
    bridgePorts_c2_sw.Add(devs0_56.Get(1));
    bridgePorts_c2_sw.Add(devs0_61.Get(1));
    bridgePorts_c2_sw.Add(devs1_66.Get(1));
    BridgeHelper bridge_c2_sw;
    bridge_c2_sw.Install(swtches.Get(1), bridgePorts_c2_sw);
    
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}