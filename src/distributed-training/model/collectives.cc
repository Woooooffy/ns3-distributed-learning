#include "collectives.h"

#define MSCCL_MAX_ITER 65536

// flags are a 3-tuple of (workindex, gridoffset_iter, step) and it follows a lexicographical order. a threadblock is ahead of another iff its flag is ahead
#define COMPUTE_FLAG(__WORKINDEX__,__GRIDOFFSET_ITER__,__STEP__) \
 	 MSCCL_MAX_ITER*MSCCL_MAX_NUM_STEPS*(uint64_t)__WORKINDEX__ + ((uint64_t)__GRIDOFFSET_ITER__ * MSCCL_MAX_NUM_STEPS + (uint64_t)__STEP__)

#define GET_WORKINDEX_FROM_FLAG(__FLAG__) \
  	(__FLAG__) / (MSCCL_MAX_ITER*MSCCL_MAX_NUM_STEPS)

namespace ns3 {

	NS_LOG_COMPONENT_DEFINE("CollectivesApplication");
	
	NS_OBJECT_ENSURE_REGISTERED(CollectivesApplication);
	MscclChannel::MscclChannel(){}
	MscclChannel::MscclChannel(int id, Ptr<CollectivesApplication> app): m_id(id), m_dataType(app->GetDataType()), m_socketType(app->GetSocketTypeId()), m_app(app){}
	MscclChannel::~MscclChannel(){}

/*	void MscclChannel::SetupListener() {
		m_listenSocket = Socket::CreateSocket(m_app->GetNode(), PacketSocketFactory::GetTypeId());
    // Accept callback: first decide whether to accept the connection
    m_listenSocket->SetAcceptCallback(
        MakeCallback(&MscclChannel::CanAcceptConnection, this),
        MakeCallback(&MscclChannel::OnNewConnection, this)
    );
		PacketSocketAddress addr;
    InetSocketAddress localAddr(Ipv4Address::GetAny(), m_app->GetPort());
    m_listenSocket->Bind(localAddr);
    m_listenSocket->Listen();
	} */

	void MscclChannel::ConnectSendPeer(int peerId){
		Ptr<Socket> sock = Socket::CreateSocket(m_app->GetNode(), m_socketType);
		PacketSocketAddress addr;
		addr.SetSingleDevice(m_app->GetSendDevicePeer(peerId, m_id)->GetIfIndex());
		addr.SetPhysicalAddress(m_app->GetPeerAddr(peerId, m_id));
		addr.SetProtocol(COLLECTIVES_PROTOCOL);

		sock->Bind(addr);
		sock->Connect(addr);
		// sock->SetRecvCallback(MakeCallback(&MscclChannel::RecvCallback, this));
		sock->SetDataSentCallback(MakeCallback(&MscclChannel::SendCallback, this));
		m_sendPeerSockets[peerId] = sock;
	}	

	void MscclChannel::SetupRecvPeer(int peerId) {
		Ptr<Socket> sock = Socket::CreateSocket(m_app->GetNode(), m_socketType);
		PacketSocketAddress addr;
		addr.SetSingleDevice(m_app->GetRecvDevicePeer(peerId, m_id)->GetIfIndex());
		addr.SetProtocol(COLLECTIVES_PROTOCOL);
		sock->Bind(addr);
		m_recvSocketPeers[sock] = peerId;
		sock->SetRecvCallback(MakeCallback(&MscclChannel::RecvCallback, this));
		// sock->SetDataSentCallback(MakeCallback(&MscclChannel::SendCallback, this));
	}

	void MscclChannel::SendCallback(Ptr<Socket> sock, uint32_t bytes){
		uint32_t sendSize = bytes;
		std::queue<PendingTransfer>& sendQueue = m_pendingSends.at(sock);
		while (sendSize > 0 && !sendQueue.empty()){
			auto& cur = sendQueue.front();
			uint32_t take = std::min(sendSize, cur.pendingBytes);
			cur.pendingBytes -= take;
			sendSize -= take;
			if (cur.pendingBytes == 0){
				// logical packet fully sent
				sendQueue.pop();
				// send should always be last op
				Simulator::ScheduleNow(&CollectivesApplication::StepCompletionCallback, m_app, cur.bid, cur.sid);
			}
		}
	}

	void MscclChannel::RecvCallback(Ptr<Socket> sock){
		while (sock->GetRxAvailable() > 0){
			Address from;
      Ptr<Packet> packet = sock->RecvFrom(from);
			uint32_t recvSize = packet->GetSize();
			MscclHeader hdr;
			if (recvSize >= hdr.GetSerializedSize()){
				packet->RemoveHeader(hdr);
				recvSize = packet->GetSize();
			}
			else NS_FATAL_ERROR("Received packet with incomplete header");
			if (hdr.GetSrcGpu() == 2 && hdr.GetDstGpu() == 0){
				NS_LOG_INFO("Debug line: received packet from 2 to 0");
			}
			// TODO: properly consider cross recv op boundary packets
			// tmp buffer and offset for cross recv op boundary packets
			uint8_t* tmp = (uint8_t*) malloc(recvSize);
			packet->CopyData(tmp, recvSize);
			size_t tmp_offset = 0;
			// peer
			uint16_t peerId = static_cast<uint16_t>(m_recvSocketPeers.at(sock));
			if (peerId != hdr.GetSrcGpu() || m_app->GetNode()->GetId() != hdr.GetDstGpu()){
					// debug prints for now
					// TODO: forwarding not yet handled
				NS_LOG_INFO("Ignoring packet from " << hdr.GetSrcGpu() << " to " << hdr.GetDstGpu() << ", expecting " << peerId << " to " << m_app->GetNode()->GetId());
				break;
			}
			if (m_id != hdr.GetChannel()){
				// expected to happen sometimes
				// TODO: handle multiple links channel-socket-device assignments
				NS_LOG_INFO("Ignoring packet for another channel");
				break;
			}
			// copy data
			// TODO: handle app level chunking properly
			std::pair<uint16_t, uint16_t> dstInfo(hdr.GetDstBuf(), hdr.GetDstOff());
			void* dst = m_app->GetBufferPtr(dstInfo.first, dstInfo.second);
			memcpy(dst, tmp + tmp_offset, recvSize);
			// if waiting on this recv
			if (m_pendingRecvByBufferRegion.contains(dstInfo)){
				auto& cur = m_pendingRecvByBufferRegion[dstInfo];
				// TODO: same as above
				switch (cur.op){
					case MSCCL_RECV:
						Simulator::ScheduleNow(&CollectivesApplication::StepCompletionCallback, m_app, cur.bid, cur.sid);
							break;
						// case MSCCL_RECV_COPY_SEND:
						// case MSCCL_RECV_REDUCE_SEND:
						default:
							NS_FATAL_ERROR("not implemented");
				}
				m_pendingRecvByBufferRegion.erase(dstInfo);
				free(tmp);
				return;
			}
			// otherwise do not yet know what to do; post this ready recv
			if (m_recvReadyByBufferRegion.contains(dstInfo) && m_recvReadyByBufferRegion[dstInfo] == true) NS_FATAL_ERROR("Received multiple packets for same dst region. Check scheduling bugs");
			m_recvReadyByBufferRegion[dstInfo] = true;
			/* std::queue<PendingTransfer>& recvQueue = m_pendingRecvs.at(peerId);	
			while (recvSize > 0 && !recvQueue.empty()){
				auto& cur = recvQueue.front();
				uint32_t take = std::min(recvSize, cur.pendingBytes);
				cur.pendingBytes -= take;
				void* dst = ((uint8_t*) m_app->GetBufferPtr(cur.dstBuf, cur.dstOffset)) + cur.receivedBytes;
				memcpy(dst, tmp + tmp_offset, take);
				cur.receivedBytes += take;
				tmp_offset += take;
				recvSize -= take;
				if (cur.pendingBytes == 0){
					// logical packet fully received
					recvQueue.pop();
					switch (cur.op){
						case MSCCL_RECV:
							Simulator::ScheduleNow(&CollectivesApplication::StepCompletionCallback, m_app, cur.bid, cur.sid);
							break;
						// case MSCCL_RECV_COPY_SEND:
						// case MSCCL_RECV_REDUCE_SEND:
						default:
							NS_FATAL_ERROR("not implemented");
					}
				}
			} */
			free(tmp);
		}
	}

	void MscclChannel::SetPendingRecv(uint16_t dstbuf, int16_t dstoff, PendingTransfer recv){
		if (dstoff < 0) NS_FATAL_ERROR("Invalid dst offset");
		m_pendingRecvByBufferRegion[std::make_pair(dstbuf, static_cast<uint16_t>(dstoff))] = recv;
	}

	void MscclChannel::PushPendingSend(Ptr<Socket> sock, PendingTransfer send){
		m_pendingSends[sock].push(send);
	}

	void MscclChannel::Send(int8_t bid, int16_t sid, int16_t sendpeer, uint32_t nElems, uint16_t srcbuf, int16_t srcoff, uint16_t dstbuf, int16_t dstoff){
		if (sendpeer < 0){
			NS_FATAL_ERROR("Send peer is negative in Send");
		}
		if (dstoff < 0){
			NS_FATAL_ERROR("Invalid dst offset in Send");
		}
		uint32_t bytes = nElems * DataType::GetSizeBytes(m_dataType); 
		Ptr<Socket> sock = m_sendPeerSockets.at(sendpeer);
		if (sendpeer == 0 && m_app->GetNode()->GetId() == 1){
			NS_LOG_INFO("Debug line: calling Send from n1 to n0 here.");
		}
		if (sendpeer == 1 && m_app->GetNode()->GetId() == 0){
			NS_LOG_INFO("Debug line: calling Send from n0 to n1 here.");
		}
		Ptr<Packet> pkt = Create<ns3::Packet>((uint8_t*) m_app->GetBufferPtr(srcbuf, srcoff), bytes);
		MscclHeader header(m_app->GetNode()->GetId(), static_cast<uint16_t>(sendpeer), static_cast<uint16_t>(m_id), dstbuf, static_cast<uint16_t>(dstoff), bytes);
		pkt->AddHeader(header);
		PendingTransfer send(bid, sid, bytes + header.GetSerializedSize(), MSCCL_SEND, srcbuf, srcoff, dstbuf, dstoff);
		m_pendingSends[sock].push(send);
		sock->Send(pkt, 0);
		// sock->Send((uint8_t*) m_app->GetBufferPtr(srcbuf, srcoff), bytes, 0);
		// callback triggered by socket send completion
		// m_app->StepCompletionCallback(bid, sid);
	}

	void MscclChannel::Recv(int8_t bid, int16_t sid, int16_t recvpeer, uint32_t nElems, uint16_t dstbuf, int16_t dstoff){
		if (recvpeer == 2 && m_app->GetNode()->GetId() == 0){
			NS_LOG_INFO("Debug line: calling recv from n2 on n0 here.");
		}
		if (dstoff < 0) NS_FATAL_ERROR("Invalid offset");
		PendingTransfer recv(bid, sid, nElems * DataType::GetSizeBytes(m_dataType), MSCCL_RECV, 0, -1, dstbuf, dstoff);
		std::pair<uint16_t, uint16_t> dstinfo(dstbuf, static_cast<uint16_t> (dstoff));
		// if we have already received
		if (m_recvReadyByBufferRegion.contains(dstinfo) && m_recvReadyByBufferRegion[dstinfo] == true){
			Simulator::ScheduleNow(&CollectivesApplication::StepCompletionCallback, m_app, bid, sid);
			m_recvReadyByBufferRegion[dstinfo] = false;
			return;
		}
		// else note this pending recv
		SetPendingRecv(dstbuf, dstoff, recv);
		// StepCompletion triggered during recv callback
	}

	void MscclChannel::RecvCpSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems){
	NS_FATAL_ERROR("RecvCpSend not yet implemented");
	}

	void MscclChannel::RecvRedSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems){
	NS_FATAL_ERROR("RecvRedSend not yet implemented");
	}
	
	void MscclChannel::RecvRedCp(int8_t bid, int16_t sid, int16_t recvpeer, uint32_t nElems, uint16_t dstbuf, int16_t dstoff){
		NS_FATAL_ERROR("RecvRedCp not yet implemented");
		PendingTransfer recv(bid, sid, nElems * DataType::GetSizeBytes(m_dataType), MSCCL_RECV_REDUCE_COPY, 0, -1, dstbuf, dstoff);
		SetPendingRecv(dstbuf, dstoff, recv);
	}

	void MscclChannel::RecvRedCpSend(int8_t bid, int16_t sid, int16_t sendpeer, int16_t recvpeer, uint32_t nElems){
	NS_FATAL_ERROR("RecvRedCpSend not yet implemented");
	}

	void MscclChannel::Close(){
		// check for unfinished sends
		for (auto& sendQueue : m_pendingSends){
			if (!sendQueue.second.empty()){
				NS_FATAL_ERROR("BUG: has pending send on node " << m_app->GetNode()->GetId() << " channel " << m_id << " at application close.");
			}
		}
		// unfinished recvs
		/*for (auto& recvQueue : m_pendingRecvs){
			if (!recvQueue.second.empty()){
				NS_FATAL_ERROR("BUG: has pending recv on node " << m_app->GetNode()->GetId() << " channel " << m_id << " at application close.");
			}
		}*/
		// socket close	
		if (m_listenSocket) m_listenSocket->Close();
		for (auto& pair: m_sendPeerSockets){
			pair.second->Close();
		}
		for (auto& pair: m_recvSocketPeers){
			pair.first->Close();
		}
	}

	//////////////////////////////////////////////
	TypeId CollectivesApplication::GetTypeId()
	{
		static TypeId tid =
			TypeId("ns3::CollectivesApplication")
				.SetParent<Application>()
				.SetGroupName("Applications")
				.AddConstructor<CollectivesApplication>()
				.AddAttribute(
					"DataType",
					"Element datatype used in the collective operation",
					EnumValue(DataType::INT32),
					MakeEnumAccessor<DataType::Type>(&CollectivesApplication::m_dataType),
					MakeEnumChecker(	
						DataType::FLOAT32, "FLOAT32",
						DataType::FLOAT64, "FLOAT64",
						DataType::INT32,   "INT32",
						DataType::INT16,   "INT16"))
				.AddAttribute(
					"Protocol",
					"The type of protocol to use. This should be a subclass of ns3::SocketFactory",
					TypeIdValue(PacketSocketFactory::GetTypeId()),
					MakeTypeIdAccessor(&CollectivesApplication::m_socket_tid),
					MakeTypeIdChecker())
				.AddAttribute(
					"ChunkSize", "Number of elements in a chunk",
				UintegerValue(1024),
				MakeUintegerAccessor(&CollectivesApplication::m_currChunkSize),
				MakeUintegerChecker<uint32_t>());
		return tid;
	}


	CollectivesApplication::CollectivesApplication(): Application(){}

	CollectivesApplication::~CollectivesApplication(){
		free(m_srcBuf.dataBuffer);
		free(m_dstBuf.dataBuffer);
		free(m_scratchBuf.dataBuffer);
	}

	void CollectivesApplication::SetAlgo(mscclAlgorithm* algo){
		m_algo = algo;
	}

	void CollectivesApplication::SetCurrChunkSize(uint32_t chunksize){
		m_currChunkSize = chunksize;
	}

	Address CollectivesApplication::GetPeerAddr(int16_t peer, int ind){
		// TODO fix hard coding node downcast type
		return DynamicCast<GPU>(GetNode())->GetPeerAddr(peer, ind);
	}

	Ptr<NetDevice> CollectivesApplication::GetSendDevicePeer(int16_t peer, int ind){
		// TODO same as above
		return DynamicCast<GPU>(GetNode())->GetSendDevicePeer(peer, ind);
	}

	Ptr<NetDevice> CollectivesApplication::GetRecvDevicePeer(int16_t peer, int ind){
		// TODO same as above
		return DynamicCast<GPU>(GetNode())->GetRecvDevicePeer(peer, ind);
	}

	int CollectivesApplication::GetPort(){
		return m_port;
	}

	DataType::Type CollectivesApplication::GetDataType(){
		return m_dataType;
	}

	TypeId CollectivesApplication::GetSocketTypeId(){
		return m_socket_tid;
	}

/*	inline TransferState* CollectivesApplication::GetTransferState(int8_t bid, int16_t sid){
		return &m_transferStates[std::make_pair(bid, sid)];	
	}*/

	DataBuffer* CollectivesApplication::GetSrcBuffer(){
		return &m_srcBuf;
	}

	DataBuffer* CollectivesApplication::GetDstBuffer(){
		return &m_dstBuf;
	}

	DataBuffer* CollectivesApplication::GetScratchBuffer(){
		return &m_scratchBuf;
	}

	void CollectivesApplication::AllocBuffer(size_t size, DataBuffer* buf){
		size_t bytes = size * DataType::GetSizeBytes(m_dataType);
		buf->dataBuffer = malloc(bytes);
		buf->len = size;
	}

	Time CollectivesApplication::GetLocalOpDelay(int8_t op){
		// TODO: add realistic delay
		return MilliSeconds(0);
	}

	void CollectivesApplication::NonTransferHandler(int8_t bid, int16_t sid, uint16_t srcbuf, int16_t srcoff, uint16_t dstbuf, int16_t dstoff, uint32_t nElems, int8_t op){
		uint32_t bytes = nElems * DataType::GetSizeBytes(m_dataType); 
		switch (op){
			case MSCCL_LOCAL_COPY:
				memcpy(GetBufferPtr(dstbuf, dstoff), GetBufferPtr(srcbuf, srcoff), bytes);
				break;
			default:
				NS_FATAL_ERROR("Not implemented.");
		}
		// TODO: add some fixed delay for different ops
		Simulator::Schedule(GetLocalOpDelay(op), &CollectivesApplication::StepCompletionCallback, this, bid, sid);
	}

	void CollectivesApplication::RunStep(int8_t bid, int16_t sid){
		// TODO: add realistic packet size modeling
		// Count rounds loop logic based on msccl scheduling? skipped for now
		mscclThreadBlock* tb = &(m_algo->mscclTBs[bid]);
		int8_t chanId = tb->channelId;
		MscclChannel* chan = &m_channels[chanId];
		mscclTransfer* tran = &tb->transfers[sid];
		uint16_t sendPeer = tb->sendpeer;
		uint16_t recvPeer = tb->recvpeer;
		uint32_t nElems = ((uint32_t) tran->count) * m_currChunkSize; 
		uint16_t srcbuf = tran->srcbuffer;
		uint16_t dstbuf = tran->dstbuffer;
		int16_t srcoff = tran->srcoffset;
		int16_t dstoff = tran->dstoffset;
		switch (tran->type){
			case MSCCL_SEND:
				chan->Send(bid, sid, sendPeer, nElems, srcbuf, srcoff, dstbuf, dstoff);
				break;
			case MSCCL_RECV:
				chan->Recv(bid, sid, recvPeer, nElems, dstbuf, dstoff);
				break;
			case MSCCL_RECV_COPY_SEND:
				chan->RecvCpSend(bid, sid, sendPeer, recvPeer, nElems);
				break;
			case MSCCL_RECV_REDUCE_SEND:
				chan->RecvRedSend(bid, sid, sendPeer, recvPeer, nElems);
				break;
			case MSCCL_RECV_REDUCE_COPY:
				chan->RecvRedCp(bid, sid, recvPeer, nElems, dstbuf, dstoff);
				break;
			case MSCCL_RECV_REDUCE_COPY_SEND:
				chan->RecvRedCpSend(bid, sid, sendPeer, recvPeer, nElems);
				break;
			case MSCCL_LOCAL_COPY:
				NonTransferHandler(bid, sid, srcbuf, srcoff, dstbuf, dstoff, nElems, MSCCL_LOCAL_COPY);
				break;
			case MSCCL_REDUCE:
				// TODO: need to change if count loop implemented
				m_TBStates[bid].global_step += tran->numReductions - 1;
				NonTransferHandler(bid, sid, srcbuf, srcoff, dstbuf, dstoff, nElems, MSCCL_REDUCE);
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
		//TransferState* tState = GetTransferState(bid, sid);
		mscclTransfer* tran = &tb->transfers[sid];
		// int16_t nDeps = tState->nPendingDeps;
		int16_t nDeps = tran->numDependences;
		// int16_t firstDepId = tState->firstPendingDep;
		int16_t firstDepId = tran->depencePointer;
		if (nDeps > 0) {
			for (int dep = firstDepId; dep < firstDepId + nDeps; ++dep){
				int16_t depbid = tb->dependentBid[dep];
				int16_t depsid = tb->dependentStep[dep]; // depsid is global step
				int64_t depflag = COMPUTE_FLAG(m_currWorkId, m_currIter, depsid);
				TBState* depTB = &m_TBStates[depbid];
				if (depflag > depTB->flag) {
					// tell depTB to try reschedule when its flag changes
					depTB->tryReschedule.insert(bid);
					return; // cannot schedule
				}
				// no need to re-check previous deps next time
				// commented out for correct global_step update 
				// tState->firstPendingDep++;
				// tState->nPendingDeps--;
			}
			tbState->global_step += nDeps - 1; // tracks step, don't update flag until completion of step

		}
		Simulator::ScheduleNow(&CollectivesApplication::RunStep, this, bid, sid);
		m_TBStates[bid].busy = true; // set flag to prevent multiple schedulings
	}

	void CollectivesApplication::StepCompletionCallback(int8_t bid, int16_t sid){
		// TransferState* tState = GetTransferState(bid, sid);
		mscclTransfer* trans = &m_algo->mscclTBs[bid].transfers[sid];
		// update TBState
		TBState* tbState = &m_TBStates[bid];
		tbState->busy = false;
		tbState->local_step++;
		tbState->flag = (uint64_t) COMPUTE_FLAG(m_currWorkId, m_currIter, tbState->global_step); // flag update
		tbState->global_step++;
		Simulator::ScheduleNow(&CollectivesApplication::TryScheduleNextStep, this, bid);
		if (trans->has_dependence){
			for (int8_t depTB : tbState->tryReschedule){	
				Simulator::ScheduleNow(&CollectivesApplication::TryScheduleNextStep, this, depTB);
			}
			tbState->tryReschedule.clear();
		}
	}

	void CollectivesApplication::InterpretAlgo(){
		if (!m_algo->isValid){
			NS_FATAL_ERROR("No valid algorithm found.");
		}
		Bootstrap();
		for (int8_t bid = 0; bid < m_algo->nBlocks; ++bid){
			// mscclThreadBlock* tb = &m_algo->mscclTBs[bid];
			m_TBStates.emplace(bid, TBState(bid)); // built TBStates
			/* for (int16_t local_step = 0; local_step < tb->nsteps; ++local_step){
				mscclTransfer* trans = &tb->transfers[local_step];
				int nDeps = trans->numDependences;
				TransferState* tState = GetTransferState(bid, local_step); 
				tState->firstPendingDep = trans->depencePointer;
				tState->nPendingDeps = nDeps;
				if (nDeps > 0) {
					for (int dep = trans->depencePointer; dep < trans->depencePointer + nDeps; ++dep){
						int8_t depbid = tb->dependentBid[dep];
						int16_t depsid = tb->dependentStep[dep]; // depsid is global step
						tState->dependentTBs.insert(depbid); 
					}
				}
				// m_transferStates.emplace(std::make_pair(bid, local_step), TransferState(trans->depencePointer, trans->numDependences));
			} // build transferStates */
			// try scheduling first step for every TB
			Simulator::ScheduleNow(&CollectivesApplication::TryScheduleNextStep, this, bid);
		}
	}



	void CollectivesApplication::Bootstrap(){
		for (int i = 0; i < m_algo->nChannels; ++i){
			mscclChannelInfo* chanInfo = &(m_algo->mscclChannels[i]);	
			m_channels.emplace(i, MscclChannel(i, this));
			MscclChannel* chan = &m_channels[i];
			//chan->SetupListener();
			for (int r = 0; r < chanInfo->nRecvPeers; ++r){
				chan->SetupRecvPeer(chanInfo->recvPeerInfo[r].peer);
			}
			for (int s = 0; s < chanInfo->nSendPeers; ++s){
				chan->ConnectSendPeer(chanInfo->sendPeerInfo[s].peer);
			}
		}
	}

	void* CollectivesApplication::GetBufferPtrRawBytes(uint16_t buf, size_t offset){
		DataBuffer buffer;
		switch (buf){
			case MSCCL_INPUT_BUFFER:
				buffer = m_srcBuf;
				break;
			case MSCCL_OUTPUT_BUFFER:
				buffer = m_dstBuf;
				break;
			case MSCCL_SCRATCH_BUFFER:
				buffer = m_scratchBuf;
				break;
			default:
				NS_FATAL_ERROR("Unrecognized buffer type");
		}
		if (offset < 0 || offset > buffer.len * DataType::GetSizeBytes(m_dataType)) NS_FATAL_ERROR("Invalid offset");
		return ((uint8_t*)buffer.dataBuffer) + offset;
	}

	void* CollectivesApplication::GetBufferPtr(uint16_t buf, int16_t offset){
		return GetBufferPtrRawBytes(buf, offset * m_currChunkSize * (DataType::GetSizeBytes(m_dataType)));
	}

	void CollectivesApplication::DumpBuffer(DataBuffer* buf, std::ofstream& log_txt){
		void* data = buf->dataBuffer;
		size_t count = buf->len;
		log_txt << "On Node " << GetNode() << ":\n";
		log_txt << "Dumping " << std::dec << count << " elements\n";
    log_txt << std::hex << std::setfill('0');

    for (size_t i = 0; i < count; ++i) {

        log_txt << "Index " << std::dec << i << " | ";

        switch (m_dataType) {

        case DataType::INT16: {
            const int16_t* ptr = static_cast<const int16_t*>(data);
            int16_t val = ptr[i];
            log_txt << "INT16 | Addr: "
                    << static_cast<const void*>(&ptr[i])
                    << " | Val: " << std::dec << val
                    << " | Hex: 0x"
                    << std::setw(4) << std::hex
                    << static_cast<uint16_t>(val)
                    << "\n";
            break;
        }

        case DataType::INT32: {
            const int32_t* ptr = static_cast<const int32_t*>(data);
            int32_t val = ptr[i];
            log_txt << "INT32 | Addr: "
                    << static_cast<const void*>(&ptr[i])
                    << " | Val: " << std::dec << val
                    << " | Hex: 0x"
                    << std::setw(8) <<std::hex
                    << static_cast<uint32_t>(val)
                    << "\n";
            break;
        }

        case DataType::FLOAT32: {
            const float* ptr = static_cast<const float*>(data);
            float val = ptr[i];

            uint32_t bits;
            std::memcpy(&bits, &val, sizeof(bits));

            log_txt << "FLOAT32 | Addr: "
                    << static_cast<const void*>(&ptr[i])
                    << " | Val: " << std::dec << val
                    << " | Hex: 0x"
                    << std::hex << std::setw(8) << bits
                    << "\n";
            break;
        }

        case DataType::FLOAT64: {
            const double* ptr = static_cast<const double*>(data);
            double val = ptr[i];

            uint64_t bits;
            std::memcpy(&bits, &val, sizeof(bits));

            log_txt << "FLOAT64 | Addr: "
                    << static_cast<const void*>(&ptr[i])
                    << " | Val: " << std::dec << val
                    << " | Hex: 0x"
                    << std::hex << std::setw(16) << bits
                    << "\n";
            break;
        }
        }
    }

    log_txt << "---- End Dump ----\n";
	}


	void CollectivesApplication::StartApplication(){
		InterpretAlgo();
	}

	void CollectivesApplication::StopApplication(){
		// channels cleanup
		for (auto& pair : m_channels){	
			pair.second.Close();
		}
		// tb states checks
		for (auto& pair : m_TBStates){
			mscclThreadBlock* tb = &m_algo->mscclTBs[pair.first];
			if (pair.second.busy || pair.second.local_step < tb->nsteps){
				NS_FATAL_ERROR("BUG: TB " << pair.first << " on node " << GetNode()->GetId() << " not finished at application close. Has " << tb->nsteps << " steps, at step " << pair.second.local_step);
			}
		}
	}

} // namespace ns3
