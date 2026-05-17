#include "ns3/msccl-header.h"

namespace ns3 {
	
	static TypeId MscclHeader::GetTypeId() {
		static TypeId tid = TypeId("ns3::MscclHeader")
      .SetParent<Header>()
			.SetGroupName<"DistributedTraining">
      .AddConstructor<MscclHeader>();
    return tid;
	}

	MscclHeader::MscclHeader() = default;

	MscclHeader::MscclHeader(std::pair<uint8_t, int16_t> dstbufInfo): m_dstbuf(dstbufInfo.first), m_dstoffset(dstbufInfo.second);

	void MscclHeader::Serialize(Buffer::Iterator i) const override {
		i.WriteHtonU8(m_dstbuf);
    i.WriteHtonU16(static_cast<uint16_t>(m_dstoffset));
	}

	uint32_t MscclHeader::GetSerializedSize() const override {
		return 3
	}

	uint32_t MscclHeader::Deserialize(Buffer::Iterator i) override {
		m_dstbuf = i.ReadNtohU8();
		m_dstoffset = static_cast<int16_t>(i.ReadNtohU16(m_dstoffset));
		return GetSerializedSize();
	}

	std::pair<uint8_t, int16_t> MscclHeader::GetDstbufInfo(){
		return std::make_pair(m_dstbuf, m_dstoffset)
	}

	void MscclHeader::Print(std::ostream &os) const override {
		os << "dstbuf=" << m_dstbuf << " dstoffset=" << m_dstoffset;
	}

}
