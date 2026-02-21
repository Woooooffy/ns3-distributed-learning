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
	
}
