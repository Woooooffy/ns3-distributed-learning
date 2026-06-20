#ifndef RED_QUEUE_H
#define RED_QUEUE_H

#include "ns3/drop-tail-queue.h"
#include "ns3/packet.h"

namespace ns3 {

// Stub: RedQueue behaves as a DropTailQueue in this simulation.
// The TypeId "ns3::RedQueue" is not registered because the factory
// (m_queueFactory) is never used to create queues in the current code.
using RedQueue = DropTailQueue<Packet>;

} // namespace ns3

#endif /* RED_QUEUE_H */
