#include "switch.h"
namespace ns3{
	NS_OBJECT_ENSURE_REGISTERED (SmartSwitch);
	TypeId SmartSwitch::GetTypeId (void){
		static TypeId tid = TypeId ("ns3::SmartSwitch")
			.SetParent<BridgeNetDevice>()
			.SetGroupName("Bridge")
			.AddConstructor<SmartSwitch>();
		return tid;
	}
}
