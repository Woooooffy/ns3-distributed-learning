/* Stub MpiReceiver for builds without MPI enabled.
 * When ENABLE_MPI=ON the real ns3/mpi module provides this header instead.
 */

#ifndef NS3_MPI_RECEIVER_H
#define NS3_MPI_RECEIVER_H

#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/callback.h"

namespace ns3 {

class MpiReceiver : public Object
{
public:
  static TypeId GetTypeId();
  ~MpiReceiver() override;

  void Receive(Ptr<Packet> p);
  void SetReceiveCallback(Callback<void, Ptr<Packet>> callback);

private:
  void DoDispose() override;
  Callback<void, Ptr<Packet>> m_rxCallback;
};

} // namespace ns3

#endif /* NS3_MPI_RECEIVER_H */
