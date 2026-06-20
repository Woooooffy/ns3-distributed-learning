#include "mpi-receiver.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("MpiReceiver");
NS_OBJECT_ENSURE_REGISTERED(MpiReceiver);

TypeId MpiReceiver::GetTypeId()
{
  static TypeId tid = TypeId("ns3::MpiReceiver")
    .SetParent<Object>()
    .AddConstructor<MpiReceiver>();
  return tid;
}

MpiReceiver::~MpiReceiver() {}

void MpiReceiver::DoDispose()
{
  m_rxCallback = MakeNullCallback<void, Ptr<Packet>>();
  Object::DoDispose();
}

void MpiReceiver::Receive(Ptr<Packet> p)
{
  NS_ASSERT(!m_rxCallback.IsNull());
  m_rxCallback(p);
}

void MpiReceiver::SetReceiveCallback(Callback<void, Ptr<Packet>> callback)
{
  m_rxCallback = callback;
}

} // namespace ns3
