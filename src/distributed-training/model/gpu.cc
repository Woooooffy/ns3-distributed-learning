#include "gpu.h"
#include "collectives.h"

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
	Ptr<NetDevice> GPU::GetSendDevicePeer(int16_t peer, int ind){
		auto& devices = m_sendDevicePeer[peer];
		// TODO consider other assignment policies
		return devices.at(ind % devices.size());
	}
	Ptr<NetDevice> GPU::GetRecvDevicePeer(int16_t peer, int ind){
		auto& devices = m_recvDevicePeer[peer];
		return devices.at(ind % devices.size());
	}
	Address GPU::GetPeerAddr(int16_t peer, int ind){
		auto& addresses = m_sendPeerAddr[peer];
		return addresses.at(ind % addresses.size());
	}
	void GPU::PushRecvPeerDevice(int16_t peer, Ptr<NetDevice> dev){
		m_recvDevicePeer[peer].push_back(dev);
	}
	void GPU::PushSendPeerDevice(int16_t peer, Ptr<NetDevice> dev){
		m_sendDevicePeer[peer].push_back(dev);
	}
	void GPU::PushPeerAddr(int16_t peer, Address addr){
		m_sendPeerAddr[peer].push_back(addr);
	}

	void GPU::PushPeerIpAddr(int16_t peer, Ipv4Address addr){
		m_peerIpv4Addr[peer].push_back(addr);
	}

	Ipv4Address GPU::GetPeerIpAddr(int16_t peer, int ind) const {
		auto it = m_peerIpv4Addr.find(peer);
		if (it == m_peerIpv4Addr.end() || it->second.empty()) return Ipv4Address();
		return it->second.at(ind % it->second.size());
	}

	int16_t GPU::GetPeerIdFromIp(Ipv4Address addr) const {
		for (auto& kv : m_peerIpv4Addr) {
			for (auto& ip : kv.second) {
				if (ip == addr) return kv.first;
			}
		}
		return -1;
	}

	void GPU::SetMyIp(Ipv4Address ip) { m_myIp = ip; }
	Ipv4Address GPU::GetMyIp() const { return m_myIp; }

	void GPU::SetRdmaDriver(Ptr<RdmaDriver> drv) { m_rdmaDriver = drv; }
	Ptr<RdmaDriver> GPU::GetRdmaDriver() const { return m_rdmaDriver; }

	void GPU::OnRdmaDataRecv(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport, uint16_t pg, uint32_t bytes) {
		for (uint32_t i = 0; i < GetNApplications(); i++) {
			Ptr<CollectivesApplication> app = DynamicCast<CollectivesApplication>(GetApplication(i));
			if (app) {
				app->OnRdmaData(sip, sport, dport, bytes);
				return;
			}
		}
	}

}
