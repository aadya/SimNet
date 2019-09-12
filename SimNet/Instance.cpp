#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include "Instance.h"

namespace Shipping {

	using namespace std;
	using Fwk::Ptr;
	//
	// Rep layer classes
	//

	class ManagerImpl : public Instance::Manager {
	public:
		ManagerImpl();

		// Manager method
		Ptr<Instance> instanceNew(const string& name, const string& type);

		// Manager method
		Ptr<Instance> instance(const string& name);

		// Manager method
		void instanceDel(const string& name);

	private:
		map<string, Ptr<Instance> > instance_;
		std::map<std::string, std::vector<Location::Ptr> > locationMapRep_;
		std::map<std::string, std::vector<Segment::Ptr> > segmentMapRep_;
	};
	class StatisticsRep : public Instance {
	public:

		StatisticsRep(const string& name, ManagerImpl* manager) :
			Instance(name), manager_(manager)
		{
			// Nothing else to do.
		}

		// Instance method
		string attribute(const string& name);

		// Instance method
		void attributeIs(const string& name, const string& v);

	private:
		Ptr<ManagerImpl> manager_;
	};
	/*~~~~~FleetRep Declaration~~~~~*/
	class FleetRep : public Instance {
	public:

		FleetRep(const string& name, ManagerImpl* manager) :
			Instance(name), manager_(manager)
		{
			// Nothing else to do.
		}

		// Instance method
		string attribute(const string& name);

		// Instance method
		void attributeIs(const string& name, const string& v);

	private:
		Ptr<ManagerImpl> manager_;


	};


	/*~~~~~LocationRep Declaration~~~~~*/
	class LocationRep : public Instance {
	public:

		LocationRep(const string& name, ManagerImpl* manager) :
			Instance(name), manager_(manager)
		{
			// Nothing else to do.
		}

		// Instance method
		string attribute(const string& name);

		// Instance method
		void attributeIs(const string& name, const string& v);

	private:
		Ptr<ManagerImpl> manager_;

		int segmentNumber(const string& name);

	};

	class TruckTerminalRep : public LocationRep {
	public:

		TruckTerminalRep(const string& name, ManagerImpl *manager) :
			LocationRep(name, manager)
		{
			// Nothing else to do.
		}

	};
	class CustomerRep : public LocationRep {
	public:

		CustomerRep(const string& name, ManagerImpl *manager) :
			LocationRep(name, manager)
		{
			// Nothing else to do.
		}
		// Instance method
		string attribute(const string& name);

		// Instance method
		void attributeIs(const string& name, const string& v);

	};

	class PortRep : public LocationRep {
	public:

		PortRep(const string& name, ManagerImpl *manager) :
			LocationRep(name, manager)
		{
			// Nothing else to do.
		}

	};
	class BoatTerminalRep : public LocationRep {
	public:

		BoatTerminalRep(const string& name, ManagerImpl *manager) :
			LocationRep(name, manager)
		{
			// Nothing else to do.
		}

	};

	class PlaneTerminalRep : public LocationRep {
	public:

		PlaneTerminalRep(const string& name, ManagerImpl *manager) :
			LocationRep(name, manager)
		{
			// Nothing else to do.
		}

	};
	/*~~~~~SegmentRep Declaration~~~~~*/
	class SegmentRep : public Instance {
	public:

		SegmentRep(const string& name, ManagerImpl* manager) :
			Instance(name), manager_(manager)
		{
			// Nothing else to do.
		}

		// Instance method
		string attribute(const string& name);

		// Instance method
		void attributeIs(const string& name, const string& v);

	private:
		Ptr<ManagerImpl> manager_;
	};

	class TruckSegmentRep : public SegmentRep {
	public:

		TruckSegmentRep(const string& name, ManagerImpl *manager) :
			SegmentRep(name, manager)
		{
			// Nothing else to do.
		}

	};

	class BoatSegmentRep : public SegmentRep {
	public:

		BoatSegmentRep(const string& name, ManagerImpl *manager) :
			SegmentRep(name, manager)
		{
			// Nothing else to do.
		}

	};

	class PlaneSegmentRep : public SegmentRep {
	public:

		PlaneSegmentRep(const string& name, ManagerImpl *manager) :
			SegmentRep(name, manager)
		{
			// Nothing else to do.
		}

	};

	/*~~~~~ConnectivityRep Declaration~~~~~*/
	class ConnectivityRep : public Instance {
	public:

		ConnectivityRep(const string& name, ManagerImpl* manager) :
			Instance(name), manager_(manager)
		{
			// Nothing else to do.
		}

		// Instance method
		string attribute(const string& name);

		// Instance method
		void attributeIs(const string& name, const string& v);

	private:
		Ptr<ManagerImpl> manager_;

	};

	/*~~~~~ManagerImp Implementation~~~~~*/
	ManagerImpl::ManagerImpl() {
	}

	Ptr<Instance> ManagerImpl::instanceNew(const string& name, const string& type) {
		EngineManager::Ptr em = EngineManager::instance();
		if (type == "Customer") {
			Ptr<CustomerRep> t = new CustomerRep(name, this);
			CustomerLocation::Ptr  t2 = em->CustomerLocationNew(name);

			//locationMapRep_.insert(makepair(name,t2));
			//locationMapRep_[name] = t2;
			instance_[name] = t;
			return t;

		}
		else if (type == "Port") {
			Ptr<CustomerRep> t = new CustomerRep(name, this);
			PortLocation::Ptr  t2 = em->PortLocationNew(name);
			//locationMapRep_.insert(makepair(name,t2));
			instance_[name] = t;
			return t;
		}
		else if (type == "Boat terminal") {
			Ptr<CustomerRep> t = new CustomerRep(name, this);
			BoatLocation::Ptr  t2 = em->BoatLocationNew(name);
			//locationMapRep_.insert(makepair(name,t2));
			instance_[name] = t;
			return t;
		}
		else if (type == "Plane terminal") {
			Ptr<CustomerRep> t = new CustomerRep(name, this);
			PlaneLocation::Ptr  t2 = em->PlaneLocationNew(name);
			//locationMapRep_.insert(makepair(name,t2));
			instance_[name] = t;
			return t;
		}
		else if (type == "Truck terminal") {
			Ptr<TruckTerminalRep> t = new TruckTerminalRep(name, this);
			TruckLocation::Ptr  t2 = em->TruckLocationNew(name);
			//locationMapRep_.insert(makepair(name,t2));
			instance_[name] = t;
			return t;
		}
		else if (type == "Truck segment") {
			Ptr<TruckSegmentRep> t = new TruckSegmentRep(name, this);
			//Segment::Ptr  t2= em->SegmentNew(name,Segment::truck_);
			//cout<<"\n created segment : "<<t2->name();
			TruckSegment::Ptr  t2 = em->TruckSegmentNew(name);
			//segmentMapRep_.insert(makepair(name,t2));
			instance_[name] = t;
			return t;
		}
		else if (type == "Boat segment") {
			Ptr<BoatSegmentRep> t = new BoatSegmentRep(name, this);
			BoatSegment::Ptr  t2 = em->BoatSegmentNew(name);
			//segmentMapRep_.insert(makepair(name,t2));
			instance_[name] = t;
			return t;
		}
		else if (type == "Plane segment") {
			Ptr<PlaneSegmentRep> t = new PlaneSegmentRep(name, this);
			PlaneSegment::Ptr  t2 = em->PlaneSegmentNew(name);
			//segmentMapRep_.insert(makepair(name,t2));
			instance_[name] = t;
			return t;
		}
		else if (type == "Fleet") {
			Ptr<FleetRep> t = new FleetRep(name, this);
			Fleet::Ptr t1 = em->fleetNew(name);
			instance_[name] = t;
			return t;
		}
		else if (type == "Stats") {
			Ptr<StatisticsRep> t = new StatisticsRep(name, this);
			Statistics::Ptr s = em->statisticsNew(name);
			instance_[name] = t;
			return t;
		}
		else if (type == "Conn") {
			Ptr<ConnectivityRep> t = new ConnectivityRep(name, this);
			Connectivity::Ptr t1 = em->connectivityNew(name);
			instance_[name] = t;
			return t;
		}
		else {
			cerr << "Not allowed : creation of " << name << " not allowed";
			return NULL;
		}
	}

	Ptr<Instance> ManagerImpl::instance(const string& name) {
		map<string, Ptr<Instance> >::const_iterator t = instance_.find(name);

		return t == instance_.end() ? NULL : (*t).second;
	}

	void ManagerImpl::instanceDel(const string& name) {
	}

	/*~~~~~LocationRep Implementation~~~~~*/

	string LocationRep::attribute(const string& name) {
		EngineManager::Ptr em = EngineManager::instance();
		int i = segmentNumber(name);
		if (i != 0) {
			Location::Ptr l = em->locationMap(this->name());
			if (l) {
				//cout<<endl<<" 3 "<<i<<endl;
				//unsigned int i1=i;
				return l->segment(i);
			}
			//cout << "Tried to read interface " << i;
		}
		else cerr << "Not found : Segment " << name << endl; //client cannot call a methods if the instance dose not exist anyways
		return "";
	}


	void LocationRep::attributeIs(const string& name, const string& v) {
		//nothing to do
	}



	static const string segmentStr = "segment";
	static const int segmentStrlen = segmentStr.length();

	int LocationRep::segmentNumber(const string& name) {
		if (name.substr(0, segmentStrlen) == segmentStr) {
			const char* t = name.c_str() + segmentStrlen;
			return atoi(t);
		}
		return 0;
	}
	/*~~~~~SegmentRep Implementation~~~~~*/
	string SegmentRep::attribute(const string& name) {
		EngineManager::Ptr em = EngineManager::instance();
		Segment::Ptr s = em->segmentMap(this->name());
		std::string l;
		if (s) {
			std::stringstream strs;
			strs << setprecision(2) << fixed;
			if (name == "source") {
				l = s->source();
				if (l == " ") {
					cerr << "Not found : " << this->name() << " - " << name << endl;
					return "";
				}
				return l;
			}
			else if (name == "length") {
				strs << s->length().value();
				l = strs.str();
				return l;
				//char* c;
				//sprintf(c,"%f",s->length().value());
				//l.c_str()=const_cast<char*> (c);
			}
			else if (name == "return segment") {
				l = s->returnSegmentName();
				if (l == "") {
					cerr << "Not found :  return segment of " << name << endl;
					return "";
				}
				return l;

			}
			else if (name == "difficulty") {
				strs << s->difficulty().value();
				l = strs.str();
				return l;
			}
			else if (name == "expedite support") {
				if (s->expediteSupport() == Segment::yes_) return "yes";
				else if (s->expediteSupport() == Segment::no_) return "no";
				else cerr << "Not found : expedite support for segment " << name << endl;

			}
		}
		else cerr << "Not found : segment " << name << endl;

		return "";
	}

	void SegmentRep::attributeIs(const string& name, const string& v) {
		EngineManager::Ptr em = EngineManager::instance();
		Segment::Ptr s = em->segmentMap(this->name());
		if (s) {
			if (name == "source") {
				std::string l = s->source();
				if (l == " ") {
					cerr << "Not found : " << name << " - " << v << endl;
					return;
				}
				s->sourceIs(v);
				return;
			}
			else if (name == "length") {
				Length len(atof(v.c_str()));
				s->lengthIs(len);
				return;
			}
			else if (name == "return segment") {
				s->returnSegmentNameIs(v);
				return;
			}
			else if (name == "difficulty") {
				Difficulty d(atof(v.c_str()));
				s->difficultyIs(d);
			}
			else if (name == "expedite support") {
				if (v == "yes") s->expediteSupportIs(Segment::yes_);
				else if (v == "no") s->expediteSupportIs(Segment::no_);
				else cerr << "Not allowed : " << name << " - " << v << endl;

			}
		}
		else cerr << "Not found : segment " << name << endl;

	}

	/*~~~~~FleetRep Implementation~~~~~*/
	// Instance method
	string FleetRep::attribute(const string& name) {
		int  s = name.size();
		int  pos = name.find(",");
		if (pos != -1 && pos != 0) {
			std::string s_1 = name.substr(0, pos);
			std::string s_2 = name.substr(pos + 2, s);
			if (!s_1.compare("Boat") || !s_1.compare("Truck") || !s_1.compare("Plane"))
			{
				EngineManager::Ptr em = EngineManager::instance();
				Fleet::Ptr f = em->fleetInstance();
				std::stringstream strs;
				strs << setprecision(2) << fixed;
				if (s_2.compare("cost") == 0) {
					strs << f->cost(s_1).value();
					return strs.str();
				}
				else if (s_2.compare("capacity") == 0) {
					strs << f->capacity(s_1).value();
					return strs.str();
				}
				else if (s_2.compare("speed") == 0) {
					strs << f->speed(s_1).value();
					return strs.str();
				}
				else cerr << "Not allowed : Call " << s_2.c_str() << " on " << endl;
			}
			else {
				cerr << "Not allowed : Fleet type " << s_1.c_str() << endl;
			}

		}
		else {
			cerr << "Not Allowed : Wrong Attribute call on " << this->name() << endl;
			return "";
		}
		return "";
	}

	// Instance method
	void FleetRep::attributeIs(const string& name, const string& v) {
		EngineManager::Ptr em = EngineManager::instance();
		Fleet::Ptr f = em->fleetInstance();
		int  s = name.size();
		int  pos = name.find(",");
		if (pos != -1 && pos != 0) {
			std::string s_1 = name.substr(0, pos);
			std::string s_2 = name.substr(pos + 2, s);
			if (!s_1.compare("Boat") || !s_1.compare("Truck") || !s_1.compare("Plane"))
			{


				if (s_2.compare("cost") == 0) {
					Cost c(atof(v.c_str()));
					f->costIs(s_1, c);
					return;
				}
				else if (s_2.compare("capacity") == 0) {
					Capacity c(atoi(v.c_str()));
					f->capacityIs(s_1, c);
					return;
				}
				else if (s_2.compare("speed") == 0) {
					Speed s(atof(v.c_str()));
					f->speedIs(s_1, s);
					return;
				}
				else cerr << "Not allowed : Attribute calls " << s_2.c_str() << " on " << s_1.c_str() << endl;
			}
			else {
				cerr << "Not allowed : Fleet type " << s_1.c_str() << endl;
			}
		}
		else {
			//cerr<<"Not Allowed : Wrong Attribute call on "<<this->name()<<endl;
			if (!name.compare("OffPeak Start Time")) {
				Time t(atof(v.c_str()));
				f->offPeakTimeStartIs(t);
			}
			if (!name.compare("OffPeak End Time")) {
				Time t(atof(v.c_str()));
				f->offPeakTimeEndIs(t);
			}
			if (!name.compare("OffPeak Capcity Scale")) {
				Percentage p(atof(v.c_str()));
				f->offPeakCapacityScaleIs(p);
			}
			if (!name.compare("OffPeak Cost Scale")) {
				Percentage p(atof(v.c_str()));
				f->offPeakCostScaleIs(p);
			}
		}
	}

	/*~~~~~StatisticsRep Implementation~~~~~*/
	// Instance method
	string StatisticsRep::attribute(const string& name) {
		EngineManager::Ptr em = EngineManager::instance();
		Statistics::Ptr s = em->statsInstance();
		std::stringstream strs;
		strs << setprecision(2) << fixed;
		if (!name.compare("expedite percentage")) {
			strs << s->expeditePercentage().value();
			return strs.str();
		}
		else {
			strs << s->entityCount(name);
			return strs.str();
		}

	}

	// Instance method
	void StatisticsRep::attributeIs(const string& name, const string& v) {
		//nothing to do - cerr ??
	}
	/*~~~~~ConnectivityRep Implementation~~~~~*/
	// Instance method
	string ConnectivityRep::attribute(const string& name) {
		EngineManager::Ptr em = EngineManager::instance();
		Connectivity::Ptr c = em->connInstance();
		string s = c->paths(name);
		//cout<<s;
		return s;
		//return c->paths(name);
	}

	// Instance method
	void ConnectivityRep::attributeIs(const string& name, const string& v) {
		EngineManager::Ptr em = EngineManager::instance();
		Connectivity::Ptr cn = em->connInstance();
		if (!v.compare("Least Load")) {
			cn->routingIs(Connectivity::LeastCostPath_);

		}
		if (!v.compare("Least Cost")) {
			cn->routingIs(Connectivity::LeastCostPath_);

		}


	}

	/*~~~~~CustomerRep Implementation~~~~~*/

	string CustomerRep::attribute(const string& name) {
		EngineManager::Ptr em = EngineManager::instance();
		CustomerLocation::Ptr c = dynamic_cast<CustomerLocation*>(em->locationMap(this->name()).ptr());

		std::stringstream strs;
		strs << setprecision(2) << fixed;
		if (!name.compare("Transfer Rate")) {
			strs << c->transferRate().value();
			return strs.str();
		}
		else if (!name.compare("Shipment Size")) {
			strs << c->shipmentSize().value();
			return strs.str();
		}
		else if (!name.compare("Destination")) {
			strs << c->destination();
			return strs.str();
		}
		else if (!name.compare("Shipments Received")) {
			strs << c->shipmentsReceived().value();
			return strs.str();
		}
		else if (!name.compare("Average Latency")) {
			strs << c->averageLatency().value();
			return strs.str();
		}
		else if (!name.compare("Shipments Received Cost")) {
			strs << c->shipmentsReceivedTotalCost().value();
			return strs.str();
		}
		// exception

	}
	void CustomerRep::attributeIs(const string& name, const string& v) {
		EngineManager::Ptr em = EngineManager::instance();
		CustomerLocation::Ptr c = dynamic_cast<CustomerLocation*>(em->locationMap(this->name()).ptr());

		std::stringstream strs;
		strs << setprecision(2) << fixed;
		if (!name.compare("Transfer Rate")) {
			Rate r(atof(v.c_str()));
			c->transferRateIs(r);
		}
		else if (!name.compare("Shipment Size")) {
			Count s(atof(v.c_str()));
			c->shipmentSizeIs(s);

		}
		else if (!name.compare("Destination")) {

			c->destinationIs(v);
		}

	}


}

/*
 * This is the entry point for your library.
 * The client program will call this function to get a handle
 * on the Instance::Manager object, and from there will use
 * that object to interact with the middle layer (which will
 * in turn interact with the engine layer).
 */
Ptr<Instance::Manager> shippingInstanceManager() {
	return new Shipping::ManagerImpl();
}
