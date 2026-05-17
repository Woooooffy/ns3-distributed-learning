#include "ns3/msccl-header.h"

namespace ns3 {
	
	TypeId
	MscclHeader::GetTypeId()
	{
		static TypeId tid = TypeId("ns3::MscclHeader")
			.SetParent<Header>()
			.SetGroupName("DistributedTraining");
		return tid;
	}

	TypeId
	MscclHeader::GetInstanceTypeId() const
	{
		return GetTypeId();
	}

	MscclHeader::MscclHeader() = default;

	MscclHeader::MscclHeader(uint16_t src, uint16_t dst, uint16_t chan, uint16_t dstBuf, uint16_t dstOff, uint32_t bytes, uint32_t flowid): m_srcGpu(src), m_dstGpu(dst), m_channel(chan), m_dstBuf(dstBuf), m_dstOff(dstOff), m_flowId(flowid), m_bytes(bytes){}

	void MscclHeader::Serialize(Buffer::Iterator i) const {
		i.WriteHtonU16(m_srcGpu);
		i.WriteHtonU16(m_dstGpu);
		i.WriteHtonU16(m_channel);
		i.WriteHtonU16(m_dstBuf);
		i.WriteHtonU16(m_dstOff);
		i.WriteHtonU32(m_flowId);
		i.WriteHtonU32(m_bytes);
	}

	uint32_t MscclHeader::GetSerializedSize() const {
		return 18;
	}

	uint32_t MscclHeader::Deserialize(Buffer::Iterator i) {
		m_srcGpu = i.ReadNtohU16();
		m_dstGpu = i.ReadNtohU16();
		m_channel = i.ReadNtohU16();
		m_dstBuf = i.ReadNtohU16();
		m_dstOff = i.ReadNtohU16();
		m_flowId = i.ReadNtohU32();
		m_bytes = i.ReadNtohU32();
		return GetSerializedSize();
	}

	uint16_t MscclHeader::GetSrcGpu(){
		return m_srcGpu;
	}

	uint16_t MscclHeader::GetDstGpu(){
		return m_dstGpu;
	}

	uint16_t MscclHeader::GetChannel(){
		return m_channel;
	}

	uint16_t MscclHeader::GetDstBuf(){
		return m_dstBuf;
	}

	uint16_t MscclHeader::GetDstOff(){
		return m_dstOff;
	}

	uint32_t MscclHeader::GetFlowId(){
		return m_flowId;
	}

	void MscclHeader::Print(std::ostream &os) const {
		os << "srcGpu=" << m_srcGpu << " dstGpu=" << m_dstGpu;
	}

}
