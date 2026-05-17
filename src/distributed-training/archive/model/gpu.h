#ifndef GPU_H
#define GPU_H

#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/type-id.h"
#include "msccl.h"
#include <ostream>

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
		std::ostream& DumpAlgo(std::ostream& oss);

		private:
		int m_maxNChannels;
		struct mscclAlgorithm m_algo;
	};
}
#endif 
