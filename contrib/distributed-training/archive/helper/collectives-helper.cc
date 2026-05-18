#include "collectives-helper.h"
#include "ns3/collectives.h"
namespace ns3 {
	CollectivesApplicationHelper::CollectivesApplicationHelper(): ApplicationHelper("ns3::CollectivesApplication"){}
	template<typename T> ApplicationContainer CollectivesApplicationHelper::Install(NodeContainer c){
    ApplicationContainer apps;
    for (auto i = c.Begin(); i != c.End(); ++i){
        apps.Add(DoInstall<T>(*i));
    }
    return apps;	
	}
	template<typename T> ApplicationContainer CollectivesApplicationHelper::Install(Ptr<Node> node){
		return ApplicationContainer(DoInstall<T>(node));
	}
	template<typename T> Ptr<Application> CollectivesApplicationHelper::DoInstall(Ptr<Node> node){
		Ptr<CollectivesApplication> app = DynamicCast<CollectivesApplication, Application>(ApplicationHelper::DoInstall(node));
		app->SetAlgo(DynamicCast<T, Node>(node)->GetAlgo());
    return app;
	}
}
