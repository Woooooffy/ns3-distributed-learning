#include "ns3/custom-core.h"
#include "ns3/simulator.h"
#include "ns3/switched-ethernet-channel.h"

NS_LOG_COMPONENT_DEFINE("CustomCore");

namespace ns3{
	CustomCore::CustomCore(P4SwitchNetDevice* netDevice, bool enableSwap, bool enableTracing): P4SwitchCore(netDevice, enableSwap, enableTracing){}

	void CustomCore::AddForwardingRule(uint32_t flowId, uint32_t port){
		m_forwarding_table[flowId] = port;
	}

	void CustomCore::AddAddrForwarding(Address addr, uint32_t port){
		NS_LOG_INFO("Adding forwarding entry: " << addr << "; " << port);
		NS_LOG_INFO("Forwarding table size: " << m_addr_forwarding_table.size());
		m_addr_forwarding_table[addr] = port;
	}

	void CustomCore::DisableLearning(){
		m_learning = false;
	}

	void CustomCore::SetSharedTraceMap(std::map<uint32_t, std::vector<int>>* map){
		m_shared_trace = map;
	}

	void CustomCore::start_and_return_(){
		// Nothing for now
		NS_LOG_DEBUG("Starting custom switch.");
	}

	int CustomCore::ReceivePacket(Ptr<Packet> packetIn, 
			int inPort, uint16_t protocol, const Address& destination){
    NS_LOG_DEBUG("Packet received by CustomCore, Port: "
		<< inPort << ", Packet ID: " << packetIn->GetUid());
		// TODO: handle queueing
		// TODO: handle learning
		if (auto entry = m_addr_forwarding_table.find(destination); entry != m_addr_forwarding_table.end()){
		m_switchNetDevice->SendNs3Packet(packetIn,
			static_cast<int>(entry->second), protocol, 
			entry->first);
		}
		else NS_LOG_INFO("Dropping packet " << packetIn->GetUid() << ": rule not found.");
		return 0;
	}

	void CustomCore::HandleIngressPipeline(){
	}

	void CustomCore::Enqueue(uint32_t egress_port, std::unique_ptr<bm::Packet>&& packet){
	}

	bool CustomCore::HandleEgressPipeline(size_t workerId){
		return true;
	}


}// namespace ns3
