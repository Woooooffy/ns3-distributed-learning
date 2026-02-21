#ifndef MSCCL_HEADER_H
#define MSCCL_HEADER_H

#include "ns3/network-module.h"
#include "ns3/core-module.h"

#include <utility>

namespace ns3 {
	class MscclHeader : public Header {

		public:
			static TypeId GetTypeId(); 
			MscclHeader();
			MscclHeader(std::pair<uint8_t, int16_t> dstbufInfo);
			void Serialize(Buffer::Iterator i) const override;
			uint32_t Deserialize(Buffer::Iterator i) override;
			uint32_t GetSerializedSize() const override;
			void Print(std::ostream &os) const override;
			std::pair<uint8_t, int16_t> GetDstbufInfo();

		private:
		  uint8_t m_dstbuf;
			int16_t m_dstoffset;
	};
} // namespace ns3
#endif
