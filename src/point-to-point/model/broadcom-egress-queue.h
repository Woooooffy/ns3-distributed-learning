/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 Georgia Tech Research Corporation, INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef BROADCOM_EGRESS_H
#define BROADCOM_EGRESS_H

#include <vector>
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/traced-callback.h"
#include "ns3/event-id.h"
#include "drop-tail-queue-packet.h"

namespace ns3 {

class BEgressQueue : public Object {
public:
    static TypeId GetTypeId(void);
    static const unsigned fCnt = 128; // max queues for NICs
    static const unsigned qCnt = 8;   // max queues for switches

    BEgressQueue();
    virtual ~BEgressQueue();

    // Enqueue into a specific priority queue
    bool Enqueue(Ptr<Packet> p, uint32_t qIndex);
    // Dequeue using round-robin across unpaused queues
    Ptr<Packet> DequeueRR(bool paused[]);

    uint32_t GetNBytes(uint32_t qIndex) const;
    uint32_t GetNBytesTotal() const;
    uint32_t GetLastQueue();

    // Per-queue traced callbacks (connected externally)
    TracedCallback<Ptr<const Packet>, uint32_t> m_traceBeqEnqueue;
    TracedCallback<Ptr<const Packet>, uint32_t> m_traceBeqDequeue;

protected:
    // Standard queue trace sources (wired up in TypeId so ascii tracing works)
    TracedCallback<Ptr<const Packet>> m_traceEnqueue;
    TracedCallback<Ptr<const Packet>> m_traceDequeue;
    TracedCallback<Ptr<const Packet>> m_traceDrop;

private:
    bool DoEnqueue(Ptr<Packet> p, uint32_t qIndex);
    Ptr<Packet> DoDequeueRR(bool paused[]);

    double m_maxBytes;
    uint32_t m_bytesInQueue[fCnt];
    uint32_t m_bytesInQueueTotal;
    uint32_t m_rrlast;
    uint32_t m_qlast;
    std::vector<Ptr<DropTailQueue<Packet>>> m_queues;
};

} // namespace ns3

#endif /* BROADCOM_EGRESS_H */
