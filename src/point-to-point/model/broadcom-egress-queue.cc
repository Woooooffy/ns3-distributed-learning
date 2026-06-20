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
#include <iostream>
#include <stdio.h>
#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/simulator.h"
#include "ns3/queue-size.h"
#include "broadcom-egress-queue.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("BEgressQueue");
NS_OBJECT_ENSURE_REGISTERED(BEgressQueue);

TypeId BEgressQueue::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::BEgressQueue")
        .SetParent<Object>()
        .AddConstructor<BEgressQueue>()
        .AddAttribute("MaxBytes",
            "The total byte limit across all sub-queues.",
            DoubleValue(1000.0 * 1024 * 1024),
            MakeDoubleAccessor(&BEgressQueue::m_maxBytes),
            MakeDoubleChecker<double>())
        .AddTraceSource("BeqEnqueue", "Enqueue a packet (with queue index)",
            MakeTraceSourceAccessor(&BEgressQueue::m_traceBeqEnqueue),
            "ns3::BEgressQueue::PacketQueueTracedCallback")
        .AddTraceSource("BeqDequeue", "Dequeue a packet (with queue index)",
            MakeTraceSourceAccessor(&BEgressQueue::m_traceBeqDequeue),
            "ns3::BEgressQueue::PacketQueueTracedCallback")
        .AddTraceSource("Enqueue", "Enqueue a packet",
            MakeTraceSourceAccessor(&BEgressQueue::m_traceEnqueue),
            "ns3::Packet::TracedCallback")
        .AddTraceSource("Dequeue", "Dequeue a packet",
            MakeTraceSourceAccessor(&BEgressQueue::m_traceDequeue),
            "ns3::Packet::TracedCallback")
        .AddTraceSource("Drop", "Drop a packet",
            MakeTraceSourceAccessor(&BEgressQueue::m_traceDrop),
            "ns3::Packet::TracedCallback")
        ;
    return tid;
}

BEgressQueue::BEgressQueue()
{
    NS_LOG_FUNCTION_NOARGS();
    m_bytesInQueueTotal = 0;
    m_rrlast = 0;
    m_qlast = 0;
    m_maxBytes = 1000.0 * 1024 * 1024;
    for (uint32_t i = 0; i < fCnt; i++)
    {
        m_bytesInQueue[i] = 0;
        auto q = CreateObject<DropTailQueue<Packet>>();
        q->SetAttribute("MaxSize", QueueSizeValue(QueueSize("1000000p")));
        m_queues.push_back(q);
    }
}

BEgressQueue::~BEgressQueue()
{
    NS_LOG_FUNCTION_NOARGS();
}

bool
BEgressQueue::Enqueue(Ptr<Packet> p, uint32_t qIndex)
{
    NS_LOG_FUNCTION(this << p);
    bool retval = DoEnqueue(p, qIndex);
    if (retval)
    {
        m_traceEnqueue(p);
        m_traceBeqEnqueue(p, qIndex);
    }
    return retval;
}

Ptr<Packet>
BEgressQueue::DequeueRR(bool paused[])
{
    NS_LOG_FUNCTION(this);
    Ptr<Packet> packet = DoDequeueRR(paused);
    if (packet)
    {
        m_traceDequeue(packet);
    }
    return packet;
}

bool
BEgressQueue::DoEnqueue(Ptr<Packet> p, uint32_t qIndex)
{
    NS_LOG_FUNCTION(this << p);
    if (m_bytesInQueueTotal + p->GetSize() < m_maxBytes)
    {
        m_queues[qIndex]->Enqueue(p);
        m_bytesInQueueTotal += p->GetSize();
        m_bytesInQueue[qIndex] += p->GetSize();
        return true;
    }
    m_traceDrop(p);
    return false;
}

Ptr<Packet>
BEgressQueue::DoDequeueRR(bool paused[])
{
    NS_LOG_FUNCTION(this);
    if (m_bytesInQueueTotal == 0)
    {
        NS_LOG_LOGIC("Queue empty");
        return 0;
    }

    bool found = false;
    uint32_t qIndex = 0;

    // Queue 0 is highest priority
    if (m_queues[0]->GetNPackets() > 0)
    {
        found = true;
        qIndex = 0;
    }
    else
    {
        for (uint32_t j = 1; j <= qCnt; j++)
        {
            uint32_t idx = (j + m_rrlast) % qCnt;
            if (!paused[idx] && m_queues[idx]->GetNPackets() > 0)
            {
                found = true;
                qIndex = idx;
                break;
            }
        }
    }

    if (found)
    {
        Ptr<Packet> p = m_queues[qIndex]->Dequeue();
        m_traceBeqDequeue(p, qIndex);
        m_bytesInQueueTotal -= p->GetSize();
        m_bytesInQueue[qIndex] -= p->GetSize();
        if (qIndex != 0)
            m_rrlast = qIndex;
        m_qlast = qIndex;
        NS_LOG_LOGIC("Popped " << p);
        NS_LOG_LOGIC("Number bytes " << m_bytesInQueueTotal);
        return p;
    }

    NS_LOG_LOGIC("Nothing can be sent");
    return 0;
}

uint32_t
BEgressQueue::GetNBytes(uint32_t qIndex) const
{
    return m_bytesInQueue[qIndex];
}

uint32_t
BEgressQueue::GetNBytesTotal() const
{
    return m_bytesInQueueTotal;
}

uint32_t
BEgressQueue::GetLastQueue()
{
    return m_qlast;
}

} // namespace ns3
