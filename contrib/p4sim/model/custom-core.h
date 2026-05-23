#ifndef CUSTOM_SW_CORE_H
#define CUSTOM_SW_CORE_H
#include "ns3/p4-switch-core.h"

#include <map>
#include <vector>
#include <unordered_set>


namespace ns3 {
class CustomCore : public P4SwitchCore {
	public:
		CustomCore(P4SwitchNetDevice* netDevice,bool enableSwap, bool enableTracing);
		~CustomCore();
		void AddForwardingRule(uint32_t flowId, uint32_t port);
		void AddAddrForwarding(Address addr, uint32_t port);
		void DisableLearning();
		void SetSharedTraceMap(std::map<uint32_t, std::vector<int>>* map);
		void start_and_return_() override;
		int ReceivePacket(Ptr<Packet> packetIn,
                      int inPort,
                      uint16_t protocol,
                      const Address& destination) override;
		void HandleIngressPipeline() override;
    void Enqueue(uint32_t egress_port, std::unique_ptr<bm::Packet>&& packet) override;
    bool HandleEgressPipeline(size_t workerId) override;

	protected: 
	private:
		bool m_learning = true;
		std::map<uint32_t, uint32_t> m_forwarding_table;
		// write to a tracing table shared between all switches
		// for routes that are pre-programmed (not learned)
		std::map<Address, uint32_t> m_addr_forwarding_table; // fallback for learning or default
		std::map<uint32_t, std::vector<int>>* m_shared_trace;
		std::unordered_set<uint32_t> m_seen_packets;

}; // CustomCore
} // namespace ns3
#endif
