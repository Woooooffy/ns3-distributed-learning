#ifndef COLL_H
#define COLL_H

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "gpu.h"
#include "msccl.h"
#include "data.h"
#include "msccl-channel.h"
#include "msccl-header.h"

#include <map>
#include <vector>
#include <utility>
#include <unordered_set>
#include <queue>

/*
TODO: model work index & iter for sequences of ops & reuse of algorithm over larger grid
Note: msccl uses (work_index, iter, step) flag for dependency tracking, where step is tracked UNIVERSALLY across all TBs- 
if TB1 step0 and TB2 step0 both have same dependiences on TB0 step2, and TB0 steps have no dependencies on other TBs,
then TB1 step0 and TB2 step0 both have universal step id 3.
this is reflected by the flags here and global_step vars. Local step is also used for map keys; sid when used for indexing/key refers to local step
*/
namespace ns3 {

	struct TBState {
		int8_t bid;
		int64_t global_step;
		int16_t local_step;
		int64_t flag;
		bool busy;
		TBState(): bid(-1), global_step(0), local_step(0), flag(0), busy(false){}
		TBState(int8_t id): bid(id), global_step(0), local_step(0), 
      flag(0), busy(false){}
	};

	struct TransferState{
		int16_t firstPendingDep;
		int16_t nPendingDeps;
		std::unordered_set<int8_t> dependentTBs; // needing retry scheduling
		TransferState(): firstPendingDep(-1), nPendingDeps(-1){}
		TransferState(int16_t firstDep, int16_t nDeps): firstPendingDep(firstDep), nPendingDeps(nDeps){} 
	};


	class CollectivesApplication : public Application {

		public:
			static TypeId GetTypeId();
			CollectivesApplication();
			~CollectivesApplication();
			void SetAlgo(mscclAlgorithm* algo);
			void SetCurrChunkSize(uint32_t chunksize);

		protected:
  		void StartApplication() override;
  		void StopApplication() override;
			inline TransferState* GetTransferState(int8_t bid, int16_t sid);
			void Send(int8_t bid, int16_t sid, int16_t sendpeer, uint32_t nElems, std::pair<uint8_t, int16_t> dst);
			void Recv(int8_t bid, int16_t sid, int16_t recvpeer, uint32_t nElems, std::pair<uint8_t, int16_t> dst);
			void RecvCpSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems, std::pair<uint8_t, int16_t> src, std::pair<uint8_t, int16_t> dst);
			void RecvRedSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems, std::pair<uint8_t, int16_t> src, std::pair<uint8_t, int16_t> dst); 
			void RecvRedCp(int8_t bid, int16_t sid, int16_t recvpeer, uint32_t nElems, std::pair<uint8_t, int16_t> dst);
			void RecvRedCpSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems, std::pair<uint8_t, int16_t> src, std::pair<uint8_t, int16_t> dst);
			Time GetLocalOpDelay(int8_t op);
			void LocalStepHandler(int8_t bid, int16_t sid); // add some fixed delay
			void RunStep(int8_t bid, int16_t sid);
			void TryScheduleNextStep(int8_t bid);
			void StepCompletionCallback(int8_t bid, int16_t sid);
			void InterpretAlgo();
			void SetupListener();
			void OnNewConnection(Ptr<Socket> newSock, const Address& from);
			bool CanAcceptConnection(Ptr<Socket> sock, const Address& from);
			void Bootstrap();
			void SendCallback(Ptr<Socket> sock, uint32_t bytes);
			void RecvCallback(Ptr<Socket> sock);
		private:
			DataType::Type m_dataType;
			mscclAlgorithm* m_algo;
			std::map<int8_t, TBState> m_TBStates; // maps tbId to last completed step
			std::map<std::pair<int8_t, int16_t>, TransferState> m_transferStates; // transfer (tbId, sId) -> pending dependences info
			uint32_t m_currChunkSize;
			uint32_t m_currWorkId = 0;
			uint32_t m_currIter = 0;
			int m_port = 5000;
			Ptr<Socket> m_listenSocket;
			std::map<int16_t, Ptr<Socket>> m_peerSockets;
			std::map<int16_t, Ipv4Address> m_peerIpv4Addr;
			std::map<std::pair<uint8_t, int16_t>, PendingTransfer> m_pendingRecvs;
			std::map<Ptr<Socket>, std::queue<PendingTransfer>> m_pendingSends;
	};
} // namespace ns3
#endif
