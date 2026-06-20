/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005 INRIA
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

#ifndef CUSTOM_HEADER_H
#define CUSTOM_HEADER_H

#include "ns3/header.h"
#include "ns3/int-header.h"

namespace ns3 {

class CustomHeader : public Header
{
public:
  CustomHeader ();
  CustomHeader (uint32_t _headerType);

  enum EcnType
    {
      ECN_NotECT = 0x00,
      ECN_ECT1 = 0x01,
      ECN_ECT0 = 0x02,
      ECN_CE = 0x03
    };

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

  uint32_t brief, headerType, getInt;
  enum HeaderType{
	L2_Header = 1,
	L3_Header = 2,
	L4_Header = 4
  };

  // ppp header
  uint16_t pppProto;

  // IPv4 header
  enum FlagsE {
    DONT_FRAGMENT = (1<<0),
    MORE_FRAGMENTS = (1<<1)
  };
  uint16_t m_payloadSize;
  uint16_t ipid;
  uint32_t m_tos : 8;
  uint32_t m_ttl : 8;
  uint32_t l3Prot: 8;
  uint32_t ipv4Flags : 3;
  uint16_t m_fragmentOffset;
  uint32_t sip;
  uint32_t dip;
  uint16_t m_checksum;
  uint16_t m_headerSize;

  union {
	  struct {
		  uint16_t sport;
		  uint16_t dport;
		  uint32_t seq;
		  uint32_t ack;
		  uint8_t length;
		  uint8_t tcpFlags;
		  uint16_t windowSize;
		  uint16_t urgentPointer;
		  uint8_t optionBuf[32];
	  } tcp;
	  struct {
		  uint16_t sport;
		  uint16_t dport;
		  uint16_t payload_size;
		  uint16_t pg;
		  uint32_t seq;
		  IntHeader ih;
	  } udp;
	  // CnHeader
	  struct {
		  uint16_t fid;
		  uint8_t qIndex;
		  uint16_t qfb;
		  uint8_t ecnBits;
		  uint16_t total;
	  } cnp;
	  // qbbHeader
	  struct {
		  uint16_t sport, dport;
		  uint16_t flags;
		  uint16_t pg;
		  uint32_t seq;
		  IntHeader ih;
	  } ack;
	  // PauseHeader
	  struct {
		  uint32_t time;
		  uint32_t qlen;
		  uint8_t qIndex;
	  } pfc;
  };

  uint8_t GetIpv4EcnBits (void) const;
  static uint32_t GetAckSerializedSize(void);
  static uint32_t GetUdpHeaderSize(void);
  static uint32_t GetStaticWholeHeaderSize(void);
};

} // namespace ns3

#endif /* CUSTOM_HEADER_H */
