#ifndef COLL_HELPER_H
#define COLL_HELPER_H
#include "ns3/applications-module.h"
namespace ns3 {

	class CollectivesApplicationHelper : public ApplicationHelper {

		public:
			CollectivesApplicationHelper();
			template<typename T> ApplicationContainer Install(NodeContainer nodes);
			template<typename T> ApplicationContainer Install(Ptr<Node> node);
		protected:
			template<typename T> Ptr<Application> DoInstall(Ptr<Node> node);

	};
}
#endif
