#ifndef GPU_H
#define GPU_H

#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/type-id.h"
#include "ns3/ipv4-address.h"
#include "ns3/rdma-driver.h"
#include "msccl.h"
#include <ostream>
#include <map>
#include <vector>

namespace ns3
{
	class GPU : public Node {

		public:
		static TypeId GetTypeId (void);
		GPU();
		GPU(int maxNChannels);
		~GPU() override;
		struct mscclAlgorithm* GetAlgo();
		void SetMaxNChannels(int maxNChannels);
		int GetMaxNChannels();
		Ptr<NetDevice> GetSendDevicePeer(int16_t peer, int ind);
		Ptr<NetDevice> GetRecvDevicePeer(int16_t peer, int ind);
		Address GetPeerAddr(int16_t peer, int ind);
		void PushRecvPeerDevice(int16_t peer, Ptr<NetDevice> dev);
		void PushSendPeerDevice(int16_t peer, Ptr<NetDevice> dev);
		void PushPeerAddr(int16_t peer, Address addr);
		// IP addresses for peers reachable via switch (RDMA path)
		void PushPeerIpAddr(int16_t peer, Ipv4Address addr);
		Ipv4Address GetPeerIpAddr(int16_t peer, int ind) const;
		// Returns peer ID whose primary IP matches addr, or -1 if not found
		int16_t GetPeerIdFromIp(Ipv4Address addr) const;
		std::ostream& DumpAlgo(std::ostream& oss);
		// This GPU's own IP address (used as sip in RDMA QPs)
		void SetMyIp(Ipv4Address ip);
		Ipv4Address GetMyIp() const;
		// RDMA driver associated with this GPU node
		void SetRdmaDriver(Ptr<RdmaDriver> drv);
		Ptr<RdmaDriver> GetRdmaDriver() const;
		// Called by RdmaHw when a data packet arrives on this GPU's NIC.
		// Routes the notification to the CollectivesApplication.
		void OnRdmaDataRecv(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport, uint16_t pg, uint32_t bytes);

		private:
		int m_maxNChannels;
		struct mscclAlgorithm m_algo;
		std::map<int16_t, std::vector<Ptr<NetDevice>>> m_recvDevicePeer;
		std::map<int16_t, std::vector<Ptr<NetDevice>>> m_sendDevicePeer;
		std::map<int16_t, std::vector<Address>> m_sendPeerAddr;
		std::map<int16_t, std::vector<Ipv4Address>> m_peerIpv4Addr;
		Ipv4Address m_myIp;
		Ptr<RdmaDriver> m_rdmaDriver;
	};
}
#endif 
