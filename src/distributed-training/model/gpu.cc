#include "gpu.h"

namespace ns3
{
	NS_LOG_COMPONENT_DEFINE("GPU");

	TypeId GPU::GetTypeId (void){
		static TypeId tid = TypeId ("ns3::GPU")
			.SetParent<Node> ()
			.SetGroupName ("DistributedTraining")
			.AddConstructor<GPU> ();	
		return tid;
	}
	GPU::GPU(): Node(){};
	GPU::GPU(int maxNChannels): m_maxNChannels(maxNChannels){}
	GPU::~GPU(){}
	void GPU::SetMaxNChannels(int maxNChannels){
		m_maxNChannels = maxNChannels;
	}
	int GPU::GetMaxNChannels(){
		return m_maxNChannels;
	}
	struct mscclAlgorithm* GPU::GetAlgo(){
		return &m_algo;
	}
	std::ostream& GPU::DumpAlgo(std::ostream& oss){
		return oss << m_algo;
	}
	Ptr<NetDevice> GPU::GetSendDevicePeer(int16_t peer){
		return m_sendDevicePeer[peer];
	}
	Ptr<NetDevice> GPU::GetRecvDevicePeer(int16_t peer){
		return m_recvDevicePeer[peer];
	}
	Address GPU::GetPeerAddr(int16_t peer){
		return m_sendPeerAddr[peer];
	}
	void GPU::PushRecvPeerDevice(int16_t peer, Ptr<NetDevice> dev){
		m_recvDevicePeer[peer] = dev;
	}
	void GPU::PushSendPeerDevice(int16_t peer, Ptr<NetDevice> dev){
		m_sendDevicePeer[peer] = dev;
	}
	void GPU::PushPeerAddr(int16_t peer, Address addr){
		m_sendPeerAddr[peer] = addr;
	}
	
}
