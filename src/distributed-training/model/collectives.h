#ifndef COLL_H
#define COLL_H

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "gpu.h"
#include "msccl.h"
#include "utils.h"
#include "msccl-channel.h"
#include "msccl-header.h"

#include <map>
#include <vector>
#include <utility>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <iostream>

/*
TODO: model work index & iter for sequences of ops & reuse of algorithm over larger grid
Note: msccl uses (work_index, iter, step) flag for dependency tracking, where step is tracked UNIVERSALLY across all TBs- 
if TB1 step0 and TB2 step0 both have same dependiences on TB0 step2, and TB0 steps have no dependencies on other TBs,
then TB1 step0 and TB2 step0 both have universal step id 3.
this is reflected by the flags here and global_step vars. Local step is also used for map keys; sid when used for indexing/key refers to local step
*/
namespace ns3 {
	class CollectivesApplication; // forward decl

	struct TBState {
		int8_t bid;
		int64_t global_step;
		int16_t local_step;
		int64_t flag;
		bool busy;
		std::unordered_set<int8_t> tryReschedule; // TBs that should try rescheduling when this TB reaches flag
		TBState(): bid(-1), global_step(0), local_step(0), flag(-1), busy(false){}
		TBState(int8_t id): bid(id), global_step(0), local_step(0), 
      flag(-1), busy(false){}
	};

/*	struct TransferState{
		int16_t firstPendingDep;
		int16_t nPendingDeps;
		std::unordered_set<int8_t> dependentTBs; // needing retry scheduling
		TransferState(): firstPendingDep(-1), nPendingDeps(-1){}
		TransferState(int16_t firstDep, int16_t nDeps): firstPendingDep(firstDep), nPendingDeps(nDeps){} 
	}; */

	struct PendingTransfer{
		int8_t bid;
		int16_t sid; //local sid
		uint32_t receivedBytes;
		uint32_t pendingBytes;
		int8_t op;
		uint16_t srcBuf;
		int16_t srcOffset;
		uint16_t dstBuf;
		int16_t dstOffset;
		PendingTransfer(): bid(-1), sid(-1), receivedBytes(0), pendingBytes(0), op(-1), srcBuf(3), srcOffset(-1), dstBuf(3), dstOffset(-1){}
		PendingTransfer(int8_t bId, int16_t sId, uint32_t bytes, int8_t Op, uint16_t srcbuf, uint16_t srcoff, uint16_t dstbuf, int16_t dstoff): bid(bId), sid(sId), 
										receivedBytes(0), pendingBytes(bytes), op(Op), srcBuf(srcbuf), srcOffset(srcoff), dstBuf(dstbuf), dstOffset(dstoff){}
	};

	// helper class for channel modeling
	class MscclChannel {
		public:
			MscclChannel();
			MscclChannel(int id, Ptr<CollectivesApplication> app);
			~MscclChannel();
			void ConnectSendPeer(int peerId);
			void SetupRecvPeer(int peerId);
			//void SetupListener();
			//void OnNewConnection(Ptr<Socket> newSock, const Address& from);
			//bool CanAcceptConnection(Ptr<Socket> sock, const Address& from);
			void SendCallback(Ptr<Socket> sock, uint32_t bytes);
			void RecvCallback(Ptr<Socket> sock);
			
			inline void SetPendingRecv(uint16_t dstBuf, int16_t dstOff, PendingTransfer recv);
			inline void PushPendingSend(Ptr<Socket> sendpeer, PendingTransfer send);
			void Send(int8_t bid, int16_t sid, int16_t sendPeer, uint32_t nElems, uint16_t srcbuf, int16_t srcoff, uint16_t dstbuf, int16_t dstoff);
			void Recv(int8_t bid, int16_t sid, int16_t recvPeer, uint32_t nElems, uint16_t dstbuf, int16_t dstoff);
			void RecvCpSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems);
			void RecvRedSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems); 
			void RecvRedCp(int8_t bid, int16_t sid, int16_t recvpeer, uint32_t nElems, uint16_t dstbuf, int16_t dstoff);
			void RecvRedCpSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems);
	
			void Close();
		private:
			int8_t m_id;
			DataType::Type m_dataType;
			TypeId m_socketType;
			Ptr<CollectivesApplication> m_app;
			Ptr<Socket> m_listenSocket;	
			std::map<int16_t, Ptr<Socket>> m_sendPeerSockets;
			std::map<Ptr<Socket>, int16_t> m_recvSocketPeers;
			// std::map<int16_t, std::queue<PendingTransfer>> m_pendingRecvs;
			std::map<std::pair<uint16_t, uint16_t>, PendingTransfer> m_pendingRecvByBufferRegion;
			std::map<std::pair<uint16_t, uint16_t>, bool> m_recvReadyByBufferRegion;
			std::map<Ptr<Socket>, std::queue<PendingTransfer>> m_pendingSends;
	};


	// application
	class CollectivesApplication : public Application {

		public:
			static TypeId GetTypeId();
			CollectivesApplication();
			~CollectivesApplication();
			void SetAlgo(mscclAlgorithm* algo);
			void SetCurrChunkSize(uint32_t chunksize);
			Address GetPeerAddr(int16_t peerId, int id);
			Ptr<NetDevice> GetSendDevicePeer(int16_t peerId, int id);
			Ptr<NetDevice> GetRecvDevicePeer(int16_t peerId, int id);
			int GetPort();
			DataType::Type GetDataType();
		  TypeId GetSocketTypeId();
			void StepCompletionCallback(int8_t bid, int16_t sid);
			DataBuffer* GetSrcBuffer();
			DataBuffer* GetDstBuffer();
			DataBuffer* GetScratchBuffer();
			void AllocBuffer(size_t size, DataBuffer* buf);
			void* GetBufferPtrRawBytes(uint16_t buf, size_t byte_offset);
			void* GetBufferPtr(uint16_t buf, int16_t offset);
			void DumpBuffer(DataBuffer* buf, std::ostream& log);

		protected:
  		void StartApplication() override;
  		void StopApplication() override;
			// inline TransferState* GetTransferState(int8_t bid, int16_t sid);
			Time GetLocalOpDelay(int8_t op);
			void NonTransferHandler(int8_t bid, int16_t sid, uint16_t srcbuf, int16_t srcoff, uint16_t dstbuf, int16_t dstoff, uint32_t nElems, int8_t op); // add some fixed delay
			void RunStep(int8_t bid, int16_t sid);
			void TryScheduleNextStep(int8_t bid);
			void InterpretAlgo();
			void Bootstrap();
		private:
			DataType::Type m_dataType;
			mscclAlgorithm* m_algo;
			TypeId m_socket_tid;
			std::map<int8_t, TBState> m_TBStates; // maps tbId to last completed step
			// std::map<std::pair<int8_t, int16_t>, TransferState> m_transferStates; // transfer (tbId, sId) -> pending dependences info
			uint32_t m_currChunkSize;
			uint32_t m_currWorkId = 0;
			uint32_t m_currIter = 0;
			int m_port = 5000;
			std::map<int, MscclChannel> m_channels;
			// std::map<int16_t, Address> m_peerAddr;
			// std::map<int16_t, Ptr<NetDevice>> m_deviceFromPeer;
			// std::map<std::pair<uint8_t, int16_t>, PendingTransfer> m_pendingRecvs;
			// std::map<Ptr<Socket>, std::queue<PendingTransfer>> m_pendingSends;
			DataBuffer m_srcBuf;
			DataBuffer m_dstBuf;
			DataBuffer m_scratchBuf;
	};
} // namespace ns3
#endif
