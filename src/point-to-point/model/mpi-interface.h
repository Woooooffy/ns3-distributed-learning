/* Stub MPI interface for builds without MPI enabled.
 * When ENABLE_MPI=ON the real ns3/mpi module provides this header instead.
 */

#ifndef NS3_MPI_INTERFACE_H
#define NS3_MPI_INTERFACE_H

#include "ns3/nstime.h"
#include "ns3/packet.h"

namespace ns3 {

class MpiInterface
{
public:
  static void Destroy() {}
  static uint32_t GetSystemId() { return 0; }
  static uint32_t GetSize() { return 1; }
  static bool IsEnabled() { return false; }
  static void Enable(int*, char***) {}
  static void Disable() {}
  // Never called when IsEnabled() == false; body is unreachable at runtime.
  static void SendPacket(Ptr<Packet>, const Time&, uint32_t, uint32_t) {}
};

} // namespace ns3

#endif /* NS3_MPI_INTERFACE_H */
