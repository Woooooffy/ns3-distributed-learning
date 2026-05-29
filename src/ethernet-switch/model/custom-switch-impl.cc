#include "ns3/custom-switch-impl.h"
#include "ns3/simulator.h"
#include "ns3/switched-ethernet-channel.h"
#include "ns3/switch-net-device.h"
#include "ns3/msccl-header.h"
#include "ns3/utils.h"

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
		NS_LOG_DEBUG("Adding fowarding rule: " << flowId << "; " << port);
		m_forwarding_table[flowId] = port;
	}

	void CustomSwitchImpl::AddAddrForwarding(Address addr, uint32_t port){
		NS_LOG_DEBUG("Adding forwarding entry: " << addr << "; " << port);	
		m_addr_forwarding_table[addr] = port;
	}

	void CustomSwitchImpl::DisableLearning(){
		m_learning = false;
	}
	
	void CustomSwitchImpl::SetSwitchNetDevice(Ptr<P4SwitchNetDevice> netdev){
		m_switchNetDevice = netdev; 
		if (m_switchNetDevice != nullptr) NS_LOG_DEBUG("Impl has dev set.");
	}

	void CustomSwitchImpl::SetSharedTraceMap(std::map<uint32_t, std::vector<int>>* map){
		m_shared_trace = map;
	}

	void CustomSwitchImpl::ReceivePacket(Ptr<Packet> packetIn, 
			int inPort, uint16_t protocol, const Address& destination, NetDevice::PacketType pktType){
    NS_LOG_DEBUG("Packet received by CustomSwitchImpl, Port: "
		<< inPort << ", Packet ID: " << packetIn->GetUid());
		if (m_seen_packets.contains(packetIn->GetUid())){
			NS_LOG_WARN("Received duplicate packet " << packetIn->GetUid());
		}
		m_seen_packets.insert(packetIn->GetUid());
		// TODO: handle queueing
		// TODO: handle learning
		if (pktType == NetDevice::PACKET_BROADCAST){
			NS_LOG_DEBUG("Broadcasting packet ID: " << packetIn->GetUid());
			for (uint32_t i = 0; i < m_switchNetDevice->GetNPorts(); ++i){
				m_switchNetDevice->SendNs3Packet(packetIn, static_cast<int>(i), protocol, destination);
			}
			return;
		}
		// FlowId forwarding
		if (protocol == COLLECTIVES_PROTOCOL){	
			// Smart switch behavior for collectives
			// remove eth header
			Ptr<Packet> pkt = packetIn->Copy();
			EthernetHeader eth;
			pkt->RemoveHeader(eth);
			MscclHeader hdr;
			if (pkt->GetSize() < hdr.GetSerializedSize()) NS_FATAL_ERROR("Received packet with incomplete header.");
			pkt->PeekHeader(hdr);
			NS_LOG_DEBUG("Attempt to match flowId " << hdr.GetFlowId());
			if (auto search = m_forwarding_table.find(hdr.GetFlowId()); search != m_forwarding_table.end()){
			  int port = static_cast<int>(search->second);	
				if (port != inPort){
					NS_LOG_DEBUG("FlowId match found. Sending out port " << port);
					if (m_shared_trace) (*m_shared_trace)[packetIn->GetUid()].push_back(m_switchNetDevice->GetNode()->GetId());
					m_switchNetDevice->SendNs3Packet(packetIn, port, protocol, destination);
					return;
				}
				else NS_LOG_WARN("FlodId match instructs forwarding packet out of in-port.");
			}
		}
	
		
		// MAC-based forwarding
		if (auto entry = m_addr_forwarding_table.find(destination); entry != m_addr_forwarding_table.end()){
		m_switchNetDevice->SendNs3Packet(packetIn,
			static_cast<int>(entry->second), protocol, 
			entry->first);
		}
		else NS_LOG_INFO("Dropping packet " << packetIn->GetUid() << ": rule not found.");
	}	


}// namespace ns3
