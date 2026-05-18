#include "collectives.h"

#define MSCCL_MAX_ITER 65536

// flags are a 3-tuple of (workindex, gridoffset_iter, step) and it follows a lexicographical order. a threadblock is ahead of another iff its flag is ahead
#define COMPUTE_FLAG(__WORKINDEX__,__GRIDOFFSET_ITER__,__STEP__) \
 	 MSCCL_MAX_ITER*MSCCL_MAX_NUM_STEPS*(uint64_t)__WORKINDEX__ + ((uint64_t)__GRIDOFFSET_ITER__ * MSCCL_MAX_NUM_STEPS + (uint64_t)__STEP__)

#define GET_WORKINDEX_FROM_FLAG(__FLAG__) \
  	(__FLAG__) / (MSCCL_MAX_ITER*MSCCL_MAX_NUM_STEPS)

namespace ns3 {
	NS_LOG_COMPONENT_DEFINE("CollectivesApplication");

	TypeId CollectivesApplication::GetTypeId(){
    static TypeId tid =
        TypeId("ns3::CollectivesApplication")
            .SetParent<Application>()
            .SetGroupName("Applications");
						.AddConstructor<CollectivesApplication> ()
						.AddAttribute (
      				"DataType",
      				"Element datatype used in the collective operation",
      				EnumValue (DataType::FLOAT32),   // default
      				MakeEnumAccessor (&CollectivesApplication::m_dataType),
      				MakeEnumChecker (
							DatatType::FLOAT15, "FLOAT16",
        			DataType::FLOAT32, "FLOAT32",
        			DataType::FLOAT64, "FLOAT64",
        			DataType::INT32,   "INT32",
        			DataType::INT16,   "INT16"
				      )
						);
		return tid;
	}
	CollectivesApplication::CollectivesApplication(): Application(), m_dataType(DataType::FLOAT32){}
	CollectivesApplication::~CollectivesApplication(){}

	void CollectivesApplication::SetAlgo(mscclAlgorithm* algo){
		m_algo = algo;
	}

	void CollectivesApplication::SetCurrChunkSize(uint32_t chunksize){
		m_currChunkSize = chunksize;
	}

	inline TransferState* CollectivesApplication::GetTransferState(int8_t bid, int16_t sid){
		return &m_transferStates[std::make_pair(bid, sid)];	
	}

	void CollectivesApplication::Send(int8_t bid, int16_t sid, int16_t sendpeer, uint32_t nElems, std::pair<uint8_t, int16_t> dst){
		Ptr<Socket> sock = m_peerSockets[sendpeer];
		Ptr<Packet> pkt = Create<Packet>(nElems * DataType::GetSizeBytes(m_dataType));
		MscclHeader hdr(dst);
		pkt->AddHeader(hdr);
		PendingTransfer send(bid, sid, nElems, MSCCL_SEND);
		m_pendingSends[sock].push(send);
		sock->Send(pkt);
		// StepCompletion triggered during send callback
	}

	void CollectivesApplication::Recv(int8_t bid, int16_t sid, int16_t recvpeer, uint32_t nElems, std::pair<uint8_t, int16_t> dst){
		PendingTransfer recv(bid, sid, nElems, MSCCL_RECV);
		m_pendingRecvs[dst] = recv; 
		// StepCompletion triggered during recv callback
	}

	void CollectivesApplication::RecvCpSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems){
	}

	void CollectivesApplication::RecvRedSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems){
	}
	
	void CollectivesApplication::RecvRedCp(int8_t bid, int16_t sid, int16_t recvpeer, uint32_t nElems){
	}

	void CollectivesApplication::RecvRedCpSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems){
	}

	Time GetLocalOpDelay(int8_t op){
		// TODO: add realistic delay
		retun MilliSeconds(0);
	}

	void CollectivesApplication::LocalStepHandler(int8_t bid, int16_t sid, int8_t op){
		// TODO: add some fixed delay for different ops
		Simulator::Schedule(GetLocalOpDelayStep(op), &CompletionCallback, this, bid, sid);
	}

	void CollectivesApplication::RunStep(int8_t bid, int16_t sid){
		// TODO: add realistic packet size modeling
		// Count rounds loop logic based on msccl scheduling? skipped for now
		mscclThreadBlock* tb = &(m_algo->mscclTBs[bid]);
		mscclTransfer* tran = &tb->transfers[sid];
		uint16_t sendPeer = tb->sendpeer;
		uint16_t recvPeer = tb->recvpeer;
		uint8_t srcbuf = tran->srcbuffer;
		int16_t srcoff = tran->srcoffset;
		uint8_t dstbuf = tran->dstbuffer;
		int16_t dstoff = tran->dstoffset;
		std::pair<uint8_t, int16_t> src(srcbuf, srcoff);
		std::pair<uint8_t, int16_t> dst(dstbuf, dstoff);
		uint32_t nElems = ((uint32_t) tran->count) * m_currChunkSize; 
		switch (tran->type){
			case MSCCL_SEND:
				Send(bid, sid, sendPeer, nElems, dst);
				break;
			case MSCCL_RECV:
				Recv(bid, sid, recvPeer, nElems, dst);
				break;
			case MSCCL_RECV_COPY_SEND:
				RecvCpSend(bid, sid, sendPeer, recvPeer, nElems, src, dst);
				break;
			case MSCCL_RECV_REDUCE_SEND:
				RecvRedSend(bid, sid, sendPeer, recvPeer, nElems, src, dst);
				break;
			case MSCCL_RECV_REDUCE_COPY:
				RecvRedCp(bid, sid, recvPeer, nElems, dst);
				break;
			case MSCCL_RECV_REDUCE_COPY_SEND:
				RecvRedCpSend(bid, sid, sendPeer, recvPeer, nElems, src, dst);
				break;
			case MSCCL_LOCAL_COPY:
				LocalStepHandler(bid, sid);
				break;
			case MSCCL_REDUCE:
				// TODO: need to change if count loop implemented
				m_TBStates[bid].global_step += tran->numReductions;
				LocalStepHandler(bid, sid);
				break;
			default:
				return;
		}
	}

	void CollectivesApplication::TryScheduleNextStep(int8_t bid){
		mscclThreadBlock* tb = &m_algo->mscclTBs[bid];
		TBState* tbState = &m_TBStates[bid];
		if (tbState->busy) return; // already scheduled next step
		int16_t sid = tbState->local_step; // sid is local step
		if (sid == tb->nsteps) return; // no more steps
		TransferState* tState = GetTransferState(bid, sid);
		int16_t nDeps = tState->nPendingDeps;
		int16_t firstDepId = tState->firstPendingDep;
		if (nDeps > 0) {
			for (int dep = firstDepId; dep < firstDepId + nDeps; ++dep){
				int16_t depbid = tb->dependentBid[dep];
				int16_t depsid = tb->dependentStep[dep]; // depsid is global step
				int64_t depflag = COMPUTE_FLAG(m_currWorkId, m_currIter, depsid);
				TBState* depTB = &m_TBStates[depbid];
				if (depflag <= depTB->flag) return; // cannot schedule
				// no need to re-check previous deps next time
				tState->firstPendingDep++;
				tState->nPendingDeps--;
			}
		}
		tbState->global_step += nDeps - 1; // tracks step, don't update flag until completion of step
		Simulator::ScheduleNow(&CollectivesApplication::RunStep, this, bid, sid);
		m_TBStates[bid].busy = true; // set flag to prevent multiple schedulings
	}

	void CollectivesApplication::StepCompletionCallback(int8_t bid, int16_t sid){
		TransferState* tState = GetTransferState(bid, sid);
		mscclTransfer* trans = &m_algo->mscclTBs[bid].transfers[sid];
		// update TBState
		TBState* tbState = &m_TBStates[bid];
		tbState->busy = false;
		tbState->local_step++;
		tbState->global_step++;
		tbState->flag = (uint64_t) COMPUTE_FLAG(m_currWorkId, m_currIter, tbState->global_step); // flag update
		Simulator::ScheduleNow(&CollectivesApplication::TryScheduleNextStep, this, bid);
		if (trans->has_dependence){
			for (int8_t depTB : tState->dependentTBs){	
				Simulator::ScheduleNow(&CollectivesApplication::TryScheduleNextStep, this, depTB);
			}
		}
	}

	void CollectivesApplication::InterpretAlgo(){
		if (!m_algo->isValid){
			NS_FATAL_ERROR("No valid algorithm found.");
		}
		for (int8_t bid = 0; bid < m_algo->nBlocks; ++bid){
			mscclThreadBlock* tb = &m_algo->mscclTBs[bid];
			m_TBStates.emplace(bid, TBState(bid)); // built TBStates
			for (int16_t local_step = 0; local_step < tb->nsteps; ++local_step){
				mscclTransfer* trans = &tb->transfers[local_step];
				m_transferStates.emplace(std::make_pair(bid, local_step), TransferState(trans->depencePointer, trans->numDependences));
			} // build transferStates
			// try scheduling first step for every TB
			Simulator::ScheduleNow(&CollectivesApplication::TryScheduleNextStep, this, bid);
		}
	}

	void CollectivesApplication::SetupListener() {
		m_listenSocket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());

    // Accept callback: first decide whether to accept the connection
    m_listenSocket->SetAcceptCallback(
        MakeCallback(&CollectivesApplication::CanAcceptConnection, this),
        MakeCallback(&CollectivesApplication::OnNewConnection, this)
    );

    InetSocketAddress localAddr(Ipv4Address::GetAny(), m_port);
    m_listenSocket->Bind(localAddr);
    m_listenSocket->Listen();
	}

	bool CollectivesApplication::CanAcceptConnection(Ptr<Socket> sock, const Address& from) {
    NS_LOG_INFO("Incoming connection from " << InetSocketAddress::ConvertFrom(from).GetIpv4());
    return true; // accept all
	}

	void CollectivesApplication::OnNewConnection(Ptr<Socket> newSock, const Address& from) {
    Ipv4Address remoteIp = InetSocketAddress::ConvertFrom(from).GetIpv4();
    int16_t peerId = -1;
    // Find the peer ID for this IP
    for (const auto &entry : m_peerIpv4Addr) {
        if (entry.second == remoteIp) {
            peerId = entry.first;
            break;
        }
    }
    if (peerId >= 0) {
        NS_LOG_INFO("Accepted connection from peer " << int(peerId));
        m_peerSockets[peerId] = newSock;

        newSock->SetRecvCallback(MakeCallback(&CollectivesApplication::RecvCallback, this));
				newSock->SetDataSentCallback(MakeCallback(&CollectivesApplication::SendCallback, this));
    } else {
        NS_LOG_WARN("Unknown incoming connection from " << remoteIp);
        newSock->Close();
    }
}

	void CollectivesApplication::Bootstrap(){
		SetupListener();
		for (auto entry = m_peerIpv4Addr.begin(); entry != m_peerIpv4Addr.end(); ++entry){
			int16_t peer = entry->first;
			Ipv4Address addr = entry->second;
			if (GetNode()->GetId() < (uint32_t) peer) {
        // Initiate connection
				Ptr<Socket> sock = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        sock->Connect(InetSocketAddress(addr, m_port));
        sock->SetRecvCallback(MakeCallback(&CollectivesApplication::RecvCallback, this));
				sock->SetDataSentCallback(MakeCallback(&CollectivesApplication::SendCallback, this));
        m_peerSockets[peer] = sock;
    	}
			else {
				// handled by listener logic
			}
		}
	}

	void CollectivesApplication::SendCallback(Ptr<Socket> sock, uint32_t bytes){
		PendingTransfer send = m_pendingSends[sock].pop();
		if (send.nElems * DataType::GetSizeBytes(m_dataType) != bytes){
			NS_ERROR_FATAL("Sent bytes mismatch");
		}
		// can complete since SEND is always final OP
		StepCompletionCallback(send.bid, send.sid);
	}

	void CollectivesApplication::RecvCallback(Ptr<Socket> sock){
		while (sock->GetRxAvailable() > 0){
			Address from;
      Ptr<Packet> packet = sock->RecvFrom(from);
			MscclHeader hdr;
			packet->RemoveHeader(hdr);
     	PendingTransfer recv = m_pendingRecvs[hdr.GetDstbufInfo()];
			if (recv.bid == -1) NS_ERROR_FATAL("Could not find pending recv");

			switch (recv.nextOp){
				case MSCCL_RECV:
					StepCompletionCallback(recv.bid, recv.sid);
					break;
				case MSCCL_RECV_COPY_SEND:
					Simulator::Schedule(GetLocalOpDelayStep(MSCCL_LOCAL_COPY), &Send, this, bid, sid,);
					break;
				case MSCCL_RECV_REDUCE_SEND
			}
	}

	void CollectivesApplication::StartApplication(){
		InterpretAlgo();
	}

	void CollectivesApplication::StopApplication(){
	}

} // namespace ns3
