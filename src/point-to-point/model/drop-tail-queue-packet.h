#ifndef DROP_TAIL_QUEUE_PACKET_H
#define DROP_TAIL_QUEUE_PACKET_H

#include "ns3/drop-tail-queue.h"
#include "ns3/packet.h"

namespace ns3 {

// Alias for convenience; BEgressQueue uses this to create per-priority sub-queues.
using DropTailQueuePacket = DropTailQueue<Packet>;

} // namespace ns3

#endif /* DROP_TAIL_QUEUE_PACKET_H */
