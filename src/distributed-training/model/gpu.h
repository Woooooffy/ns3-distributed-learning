#ifndef GPU_H
#define GPU_H

#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/type-id.h"
#include "msccl.h"
#include <ostream>
#include <map>

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
		Ptr<NetDevice> GetDeviceFromPeer(int16_t peer);
		Address GetPeerAddr(int16_t peer);
		void PushPeerDevice(int16_t peer, Ptr<NetDevice> dev);
		void PushPeerAddr(int16_t peer, Address addr);
		std::ostream& DumpAlgo(std::ostream& oss);

		private:
		int m_maxNChannels;
		struct mscclAlgorithm m_algo;
		std::map<int16_t, Ptr<NetDevice>> m_deviceFromPeer;
		std::map<int16_t, Address> m_peerAddr;
	};
}
#endif 
