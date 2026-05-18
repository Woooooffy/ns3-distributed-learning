#ifndef MSCCL_CHANNEL_H
#define MSCCL_CHANNEL_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"

#include<map>
#include<queue>

namespace ns3{
	struct PendingTransfer{
		int8_t bid;
		int16_t sid; //local
		uint32_t nElems;
		int8_t op;
		PendingTransfer(): bid(-1), sid(-1), nElems(0), op(-1){}
		PendingTransfer(int8_t bId, int16_t sId, uint32_t nelems, int8_t Op): bid(bId), sid(sId), nElems(nelems), op(Op){}
	};


	class MscclChannel {
		public:
			MscclChannel();
			~MscclChannel();
			void PushPendingRecv(int16_t peer, PendingTransfer recv);
		private:
			std::map<int16_t, Ptr<Socket>> m_sendPeerSockets;
			std::map<Ptr<Socket>, int16_t> m_recvSocketPeers;
			std::map<int16_t, std::queue<PendingTransfer>> m_pendingRecvs;
	};
}// namespace ns3
#endif
