#include "ns3/custom-switch-impl.h"
#include "ns3/simulator.h"
#include "ns3/switched-ethernet-channel.h"
#include "ns3/switch-net-device.h"


namespace ns3{
	NS_LOG_COMPONENT_DEFINE("CustomSwitchImpl");
	NS_OBJECT_ENSURE_REGISTERED(CustomSwitchImpl);

	TypeId CustomSwitchImpl::GetTypeId(void){
		static TypeId tid = TypeId("ns3::CustomSwitchImpl")
		.SetParent<Object>()
		.SetGroupName("P4sim")
		.AddConstructor<CustomSwitchImpl>();
		return tid;
	}
	
	CustomSwitchImpl::CustomSwitchImpl(){}

	CustomSwitchImpl::~CustomSwitchImpl(){
		NS_LOG_DEBUG("Calling CustomSwitchImpl destructor.");
	}

	void CustomSwitchImpl::AddForwardingRule(uint32_t flowId, uint32_t port){
		m_forwarding_table[flowId] = port;
	}

	void CustomSwitchImpl::AddAddrForwarding(Address addr, uint32_t port){
		NS_LOG_INFO("Adding forwarding entry: " << addr << "; " << port);
		NS_LOG_INFO("Forwarding table size: " << m_addr_forwarding_table.size());
		m_addr_forwarding_table[addr] = port;
	}

	void CustomSwitchImpl::DisableLearning(){
		m_learning = false;
	}
	
	void CustomSwitchImpl::SetSwitchNetDevice(Ptr<P4SwitchNetDevice> netdev){
		m_switchNetDevice = netdev; 
	}

	void CustomSwitchImpl::SetSharedTraceMap(std::map<uint32_t, std::vector<int>>* map){
		m_shared_trace = map;
	}

	void CustomSwitchImpl::ReceivePacket(Ptr<Packet> packetIn, 
			int inPort, uint16_t protocol, const Address& destination, NetDevice::PacketType pktType){
    NS_LOG_DEBUG("Packet received by CustomSwitchImpl, Port: "
		<< inPort << ", Packet ID: " << packetIn->GetUid());
		// TODO: handle queueing
		// TODO: handle learning
		if (pktType == NetDevice::PACKET_BROADCAST){
			NS_LOG_DEBUG("Broadcasting packet ID: " << packetIn->GetUid());
			for (uint32_t i = 0; i < m_switchNetDevice->GetNPorts(); ++i){
				m_switchNetDevice->SendNs3Packet(packetIn, static_cast<int>(i), protocol, destination);
			}
			return;
		}
		if (auto entry = m_addr_forwarding_table.find(destination); entry != m_addr_forwarding_table.end()){
		m_switchNetDevice->SendNs3Packet(packetIn,
			static_cast<int>(entry->second), protocol, 
			entry->first);
		}
		else NS_LOG_INFO("Dropping packet " << packetIn->GetUid() << ": rule not found.");
	}	


}// namespace ns3
