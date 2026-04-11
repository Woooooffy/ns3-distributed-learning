#ifndef SMART_SWITCH_H
#define SMART_SWITCH_H
#include "ns3/bridge-module.h"
#include "ns3/node.h"

namespace ns3{
	class SmartSwitch: public BridgeNetDevice{
		public:
	  	static TypeId GetTypeId (void);

		protected:
/*  		virtual void ReceiveFromDevice(Ptr<NetDevice> device, 
				Ptr<const Packet> packet, uint16_t protocol, 
				const Address &src, const Address &dst, PacketType packetType) override;*/

		private:
			NetDevice::ReceiveCallback m_rxCallback;               //!< receive callback
			NetDevice::PromiscReceiveCallback m_promiscRxCallback; //!< promiscuous receive callback

			Mac48Address m_address; //!< MAC address of the NetDevice
			Time m_expirationTime;  //!< time it takes for learned MAC state to expire
			bool m_enableExpiration; // JW`

			/**
			 * @ingroup bridge
			 * Structure holding the status of an address
			 */
			struct LearnedState
			{
					Ptr<NetDevice> associatedPort; //!< port associated with the address
					Time expirationTime;           //!< time it takes for learned MAC state to expire
			};

			std::map<Mac48Address, LearnedState> m_learnState; //!< Container for known address statuses
			Ptr<Node> m_node;                                  //!< node owning this NetDevice
			std::vector<Ptr<NetDevice>> m_ports;               //!< bridged ports
			uint32_t m_ifIndex;                                //!< Interface index
			uint16_t m_mtu;                                    //!< MTU of the bridged NetDevice
			bool m_enableLearning; //!< true if the bridge will learn the node status

	}; // SmartSwitch


}// namespace ns3

#endif
