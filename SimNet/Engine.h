#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <vector>
#include "Ptr.h"
#include "PtrInterface.h"
//#include "fwk/NamedInterface.h"
#include<exception>
//#include "Instance.h"
#include "ListRaw.h"
#include "Exception.h"
//#include "Exception.h"

#include "Nominal.h"
#include "Notifiee.h"

#include "Log.h"
#include <map>
#include<list>
#include<set>
#include <stdlib.h>
#include <stdio.h>
#include <ostream>
#include <iostream>
#include <stdio.h>

using std::cout;
using std::cerr;
using std::endl;
class ForwardShipmentReactor;
class ActivityManagerReactor;

namespace Shipping {


	class EngineManager;
	class EngineReactor;
	class SegmentReactor;
	class Shipment;
	class CustomerLocation;
	class Segment;
	class ShipmentSegmentReactor;
	//Note: The Engine.h file includes some private members as well for better understanding of our planned implementation, although this is not part of the public interface between the Rep/Engine layers
	//Sowmya

	/*class Size : public Ordinal<Size, unsigned int > {
		public:
		Size(unsigned int num) : Ordinal<Size, unsigned int>(num) {
			if(num <0){
				this->value_ = 1;
				fprintf(stderr, "\nSize value out of bounds");
			}
		}
	};*/

	class Rate : public Ordinal<Rate, unsigned int > {
	public:
		Rate(unsigned int num) : Ordinal<Rate, unsigned int>(num) {
		}
	};


	class Latency : public Ordinal<Latency, double> {
	public:
		Latency(double num) : Ordinal<Latency, double>(num) {


		}
	};

	class Capacity : public Ordinal<Capacity, unsigned int > {
	public:
		Capacity(unsigned int num) : Ordinal<Capacity, unsigned int>(num) {
			if (num < 0) {
				this->value_ = 1;
				fprintf(stderr, "\nCapacity value out of bounds");
			}
		}
	};

	class Speed : public Ordinal<Speed, double> {
	public:
		Speed(double num) : Ordinal<Speed, double>(num) {
			if (num < 0) {
				this->value_ = 1.0;
				fprintf(stderr, "\nSpeed value out of bounds");
			}
		}
	};

	class Length : public Ordinal<Length, double> {
	public:
		Length(double num) : Ordinal<Length, double>(num) {
			if (num < 0) {
				this->value_ = 0.0;
				fprintf(stderr, "\nLength value out of bounds");
			}
		}

	};

	class Count : public Ordinal<Length, unsigned int> {
	public:
		Count(unsigned int num) : Ordinal<Length, unsigned int>(num) {}

	};

	class Cost : public Ordinal<Cost, double> {
	public:
		Cost(double num) : Ordinal<Cost, double>(num) {
			if (num < 0) {
				this->value_ = 1.0;
				fprintf(stderr, "\nCost value out of bounds");
			}
		}
	};

	class Time : public Ordinal<Time, double> {
	public:
		Time(double num) : Ordinal<Time, double>(num) {
			if (num < 0) {
				this->value_ = 1.0;
				fprintf(stderr, "\nTime value out of bounds");
			}
		}
	};

	class Percentage : public Ordinal<Percentage, double> {
	public:
		Percentage(double num) : Ordinal<Percentage, double>(num) {
			//Check that val is between 0 to 100
			if (num < 0 || num>100) {
				this->value_ = 0.0;
				fprintf(stderr, "\nPercentage value out of bounds");
			}
		}
	};

	class Difficulty : public Ordinal<Difficulty, double> {
	public:
		Difficulty(double num) : Ordinal<Difficulty, double>(num) {
			//check that difficulty is within the bounds 1.0-5.0
			if (num<1.0 || num >5.0) {
				this->value_ = 1.0;
				fprintf(stderr, "\nDifficulty value out of bounds");
			}
		}
	};

	class Shipment : public Fwk::PtrInterface<Shipment> {
		// Placeholder for class to simulate shipments
		// Each shipment object denotes a particular shipping order
		// and encapsulates the source and destination locations, path and properties like cost, expedited/non-expedited etc.
	public:
		typedef Fwk::Ptr<Shipment const> PtrConst;
		typedef Fwk::Ptr<Shipment> Ptr;
		enum previouslyRefused {
			no_ = 0,
			yes_ = 1
		};
		// void sourceIs(CustomerLocation::Ptr _source){ source_ = _source; }
		 //void destinationIs(CustomerLocation::Ptr _destination){ destination_ = _destination; }
		 //void numberOfPackagesIs(Count _numberOfPackages) { numberOfPackages_ = _numberOfPackages; }
		string source() { return source_; }
		string destination() { return destination_; }
		Count numberOfPackages() { return numberOfPackages_; }
		Time startTime() { return startTime_; }
		Cost shipmentCost() { return shipmentCost_; }
		string nextLocation() { return nextLocation_; }
		void nextLocationIs(string _nextLocation) {
			nextLocation_ = _nextLocation;
			// add this location to the list of visited locations
			visitedNodes_.insert(_nextLocation);
		}
		string currSegment() { return currSegment_; }
		void currSegmentIs(string _currSegment) { currSegment_ = _currSegment; }
		static Shipment::Ptr ShipmentNew(string _name, string _source, string _destination, Count _numberOfPackages) {
			Shipment::Ptr s = new Shipment(_name, _source, _destination, _numberOfPackages);
			return s;
		}
		Count packagesDeliveredCount() { return packagesDeliveredCount_; }
		void packagesDeliveredCountIs(Count _packagesDeliveredCount) { packagesDeliveredCount_ = _packagesDeliveredCount; }
		void packagesDeliveredCountInc(Count _packagesDeliveredInc) {
			packagesDeliveredCount_ = Count(packagesDeliveredCount_.value() + _packagesDeliveredInc.value());
		}
		void shipmentCostInc(Cost _cost) {
			shipmentCost_ = Cost(shipmentCost_.value() + _cost.value());
		}
		string name() { return name_; }
		set<string> visitedNodes() { return visitedNodes_; }
	private:
		string name_;
		string source_;
		string destination_;
		string currSegment_;
		string nextLocation_;
		Count numberOfPackages_;
		Time startTime_; // To be initialized when the shipment is created
		Cost shipmentCost_;
		Count packagesDeliveredCount_;
		set<string> visitedNodes_;
		Shipment(string _name, string _source, string _destination, Count _numberOfPackages);
	};

	class Location : public Fwk::PtrInterface<Location> {
	public:
		enum LocationType {
			customer_ = 0,
			port_ = 1,
			truckTerminal_ = 2,
			boatTerminal_ = 3,
			planeTerminal_ = 4
		};

		typedef Fwk::Ptr<Location const> PtrConst;
		typedef Fwk::Ptr<Location> Ptr;

		//static Location::Ptr LocationNew(std::string _name, Location::LocationType _locationType); //will be overriden in the derived classes
		std::string name() const { return name_; };
		std::string segment(unsigned int position) const {
			if (segment_.size() >= position) {
				return segment_.at(position - 1);
			}
			else return "";
		};
		Location::LocationType locationType() const { return locationType_; }
		int segmentsSize() const { return segment_.size(); }

		class Notifiee : public Fwk::BaseNotifiee<Location> {
		public:
			typedef Fwk::Ptr<Notifiee> Ptr;

			Notifiee(Location* location) : Fwk::BaseNotifiee<Location>(location) {}

			virtual void onShipmentIs(Shipment::Ptr _shipment) {}

		};
		Fwk::Ptr<Notifiee> notifiee() { return notifiee_; }
		void lastNotifieeIs(Notifiee* n) {

			Location* me = const_cast<Location*>(this);
			me->notifiee_ = n;
		}
		virtual void shipmentIs(Shipment::Ptr _shipment);
		void pendingShipmentSchedule(string _segment);
		friend class EngineManager;
		friend class Segment;
		string findLeastLoadSegment(string destination, set<string> visitedNodes);
	protected:

		std::vector <std::string> segment_;
		std::string name_;
		Location::LocationType locationType_;
		Notifiee* notifiee_;
		map<string, list<Shipment::Ptr> > pendingShipments_;
		//ShipmentSegmentReactor* segReactor;
		//~~added
	   // typedef std::map<CustomerLocation::Ptr, Segment::Ptr> RoutingTable;
		// RoutingTable leastCostRoutingTable_;
		// RoutingTable leastTimeRoutingTable_;
		string findRoutingSegment(string destination, set<string> visitedNodes);
		Fwk::Ptr<SegmentReactor> segReactor;
		// virtual void receiveShipment();
		//~~added
		Location(std::string _name, Location::LocationType _locationType);
		void addSegment(std::string _segmentName);
		void removeSegment(std::string _segmentName);
		std::map<string, string> leastCostRoutingTable_; // This stores a map of required destination and the next forwarding location to achieve least cost
		std::map<string, set<string> > leastLoadRoutingTable_;
		void pendingShipmentDel(string _segment);
		// void onZeroReferences();
	};

	//  there will be a derived class corresponding to each entity type :
	// Customer", "Port", "Truck terminal", "Boat terminal", "Plane terminal"
	class CustomerLocation : public Location {
	public:
		friend class EngineManager;
		typedef Fwk::Ptr<CustomerLocation const> PtrConst;
		typedef Fwk::Ptr<CustomerLocation> Ptr;
		Rate transferRate() { return transferRate_; }
		void transferRateIs(Rate _transferRate);
		Count shipmentSize() { return shipmentSize_; }
		void shipmentSizeIs(Count _shipmentSize);
		string destination() { return destination_; }
		void destinationIs(string _destination);
		Count shipmentsReceived() { return shipmentsReceived_; }
		Latency averageLatency() { return averageLatency_; };
		Cost shipmentsReceivedTotalCost() { return shipmentsReceivedTotalCost_; }
		void checkCustomerLocationReady();
		void activityDel();
		class Notifiee : public Fwk::BaseNotifiee<CustomerLocation> {
		public:
			typedef Fwk::Ptr<Notifiee> Ptr;

			Notifiee(CustomerLocation* location) : Fwk::BaseNotifiee<CustomerLocation>(location) {}

			virtual void onShipmentIs(Shipment::Ptr _shipment) {}
			virtual void onTransferRateIs() { }
			virtual void onDestinationIs() { }
			virtual void onShipmentSizeIs() { }
		};
		Fwk::Ptr<Notifiee> notifiee() { return notifiee_; }

		void lastNotifieeIs(Notifiee* n) {
			//cout<<" creatign cust loc notifiee";
			CustomerLocation* me = const_cast<CustomerLocation*>(this);
			me->notifiee_ = n;

		}
		void shipmentIs(Shipment::Ptr _shipment);
		void shipmentNew();
		//string findBFSLeastLoad(string destination);
	protected:
		CustomerLocation(std::string _name);
		Rate transferRate_;
		Count shipmentSize_;
		string destination_;
		//Readonly attributes
		Count shipmentsReceived_;
		Latency averageLatency_;
		Time totalShipmentsTime_;
		Cost shipmentsReceivedTotalCost_;
		Notifiee * notifiee_;
		Count numberPerHour_;
		//Method to receive and handle shipments
		//This also triggers the notifiee method to update statistics
		//virtual void receiveShipment();
		void receiveShipment(Shipment::Ptr _shipment);

		//string findRoutingSegment(string destination);

	};

	class PortLocation : public Location {
	public:
		friend class EngineManager;
		typedef Fwk::Ptr<PortLocation const> PtrConst;
		typedef Fwk::Ptr<PortLocation> Ptr;
	protected:
		PortLocation(std::string _name);

	};

	class TruckLocation : public Location {
	public:
		friend class EngineManager;
		typedef Fwk::Ptr<TruckLocation const> PtrConst;
		typedef Fwk::Ptr<TruckLocation> Ptr;
	protected:
		TruckLocation(std::string _name);

	};

	class BoatLocation : public Location {
	public:
		friend class EngineManager;
		typedef Fwk::Ptr<BoatLocation const> PtrConst;
		typedef Fwk::Ptr<BoatLocation> Ptr;
	protected:
		BoatLocation(std::string _name);

	};

	class PlaneLocation : public Location {
	public:
		friend class EngineManager;
		typedef Fwk::Ptr<PlaneLocation const> PtrConst;
		typedef Fwk::Ptr<PlaneLocation> Ptr;
	protected:
		PlaneLocation(std::string _name);

	};

	class Segment : public Fwk::PtrInterface<Segment>

	{

		friend class EngineManager;
		friend class Location;
	public:
		typedef Fwk::Ptr<Segment const> PtrConst;
		typedef Fwk::Ptr<Segment> Ptr;
		enum ExpediteSupport
		{
			yes_, no_
		};
		enum ModeType { truck_, boat_, plane_ };
		ModeType mode()const { return mode_; }
		std::string source()const { return source_; }
		void sourceIs(std::string _source);
		Length length()const { return length_; }
		void lengthIs(Length _length) { length_ = _length; }
		std::string name()const { return name_; }

		Difficulty difficulty() const { return difficulty_; }
		void difficultyIs(Difficulty _difficulty) { difficulty_ = _difficulty; }
		ExpediteSupport expediteSupport() const { return expediteSupport_; }
		void expediteSupportIs(Segment::ExpediteSupport _expediteSupport);
		std::string returnSegmentName() const { return returnSegmentName_; }
		void returnSegmentNameIs(std::string);

		class Notifiee : public Fwk::BaseNotifiee<Segment> {
		public:
			typedef Fwk::Ptr<Notifiee> Ptr;

			Notifiee(Segment* segment) : Fwk::BaseNotifiee<Segment>(segment) {}
			virtual void onExpediteSupportIs(Segment::ExpediteSupport _newValue,
				Segment::ExpediteSupport _previousValue) { }
			virtual void onCurrentShipmentsCountDec(string _source, string _segment) { }
		};
		// Fwk::Ptr<Notifiee> notifiee() { return notifiee_; }
		void lastNotifieeIs(Notifiee* n) {
			Segment* me = const_cast<Segment*>(this);
			// cout<<"\n adding new notifiee : segment = "<<name_;;
			me->notifieeList_.push_back(n);
			notifieeSingle_ = n;
		}

		typedef std::list<Notifiee *> NotifieeList;
		typedef NotifieeList::iterator NotifieeListIterator;
		NotifieeListIterator notifieeIterBegin() { return notifieeList_.begin(); }
		int notifieeListSize() { return notifieeList_.size(); }

		//~~added
		Count shipmentsReceived() { return shipmentsReceived_; }
		Count shipmentsRefused() { return shipmentsRefused_; }
		Capacity capacity() { return capacity_; }
		void capacityIs(Capacity _capacity) { capacity_ = _capacity; }
		void shipmentIs(Shipment::Ptr _shipment, Shipment::previouslyRefused);
		Count currentShipmentsCount() { return currentShipmentsCount_; }
		void currentShipmentsCountDec();
		//~~added

	protected:
		Segment(std::string _name, Segment::ModeType _mode);
		std::string source_;
		std::string name_;
		std::string returnSegmentName_;
		ExpediteSupport expediteSupport_;
		Difficulty difficulty_;
		Length length_;
		//defaults ?
		ModeType mode_;
		//~~added
		Count shipmentsReceived_;
		Count shipmentsRefused_;
		Capacity capacity_; // number of shipments
		std::vector<Shipment::Ptr> shipments_;
		Count currentShipmentsCount_;
		//~~added
		int countToDel;
		Notifiee* notifieeSingle_;
		std::list<Notifiee *> notifieeList_;
		ForwardShipmentReactor * fwdShipmentReactor;
		Cost segmentCost();


	};

	class BoatSegment :public Segment {
	protected:
		BoatSegment(std::string _name);

	};
	class PlaneSegment :public Segment {
	protected:
		PlaneSegment(std::string _name);
	};
	class TruckSegment :public Segment {
	protected:
		TruckSegment(std::string _name);
	};


	class Connectivity : public Fwk::PtrInterface<Connectivity>
	{

	public:
		enum ConnType
		{
			explore_ = 0,
			connect_ = 1
		};

		//~~added
		enum RoutingAlgo
		{
			LeastLoadPath_ = 0,
			LeastCostPath_ = 1,

		};
		//~~added

		typedef Fwk::Ptr<Connectivity const> PtrConst;
		typedef Fwk::Ptr<Connectivity> Ptr;
		//public accessors, no mutators - writes can be discarded in the Rep layer
		std::string paths(std::string args);
		std::string name() const { return name_; }
		//~~added
		void routingIs(RoutingAlgo _routingAlgo) { routingAlgo_ = _routingAlgo; }
		RoutingAlgo routingAlgo() { return routingAlgo_; }
		//~~

		Connectivity::ConnType connectionType() { return connectionType_; }

	protected:
		friend class EngineManager;
		std::string name_;
		std::vector<std::string> paths_;
		//std::vector<std::string> connectionPaths_;
		ConnType connectionType_;
		//~~added
		RoutingAlgo routingAlgo_;
		//~~

		Connectivity(std::string _name) :name_(_name), routingAlgo_(LeastLoadPath_) { }
		Connectivity(Connectivity const&); // prevent copy construction
		Connectivity& operator= (Connectivity const&); // prevent copying

		std::string convertToString(std::vector<std::string> strVect);
		std::vector<std::string> explore(Location::Ptr startLocPtr,
			Length dist, Length currDist,
			Cost cost, Cost currCost,
			Time time, Time currTime,
			Segment::ExpediteSupport expedited,
			std::vector<Location::Ptr> pathSoFar,
			std::string pathString, std::string segToAdd);
		std::vector<std::string> connect(Location::Ptr startLocPtr, Location::Ptr destLocPtr,
			std::vector<Location::Ptr> path, std::string pathString,
			std::string segToAdd, Segment::ExpediteSupport expedited,
			Cost pathCost, Cost segCost,
			Time pathTime, Time segTime,
			Segment::ExpediteSupport allSegmentsExpedited);
		std::string convertToString(Location::Ptr locPtr);
		std::string convertToString(Segment::Ptr segPtr);
		std::string convertToString(Cost cost, Time time, Segment::ExpediteSupport expedited);
		std::string convertToString(double val);
		static Time segmentTime(Segment::ModeType _mode, Length _len, Segment::ExpediteSupport _exp);
		static Cost segmentCost(Segment::ModeType _mode, Difficulty _diff, Length _len, Segment::ExpediteSupport exp);
		inline double convertToDouble(const std::string& s);
	};



	class Statistics : public Fwk::PtrInterface<Statistics> {
	protected:
		Statistics(std::string _name);
		Statistics(Statistics const&); // prevent copy construction
		Statistics& operator= (Statistics const&); // prevent copying

		std::string name_;
		Fwk::Ptr<EngineReactor> erInstance_;

		static std::map<std::string, Count> locationCount_;
		static std::map<std::string, Count> segmentCount_;
		Percentage expeditePercentage_;
		void onZeroReferences();
		//data members for counts
	public:
		static std::vector<Fwk::Ptr<SegmentReactor> > srInstances_;
		typedef Fwk::Ptr<Statistics const> PtrConst;
		typedef Fwk::Ptr<Statistics> Ptr;
		std::string name() { return name_; }
		int entityCount(std::string entityType);
		Percentage expeditePercentage();

		friend class EngineManager;

	};

	class Fleet : public Fwk::PtrInterface<Fleet>
	{
		friend class EngineManager;
	public:
		class Property : public Fwk::PtrInterface<Property> {

		protected:
			Speed speed_;
			Cost cost_;
			Capacity capacity_;
		public:
			typedef Fwk::Ptr<Property const> PtrConst;
			typedef Fwk::Ptr<Property> Ptr;
			void speedIs(Speed val) { speed_ = val; }
			void costIs(Cost val) { cost_ = val; }
			void capacityIs(Capacity val) { capacity_ = val; }
			Speed speed() { return speed_; }
			Capacity capacity() { return capacity_; }
			Cost cost() { return cost_; }
			Property() :speed_(1.0), cost_(1.0), capacity_(1) {}
		};

		typedef Fwk::Ptr<Fleet const> PtrConst;
		typedef Fwk::Ptr<Fleet> Ptr;
		std::string name() const { return name_; }
		//std::string mode(){return mode_;}
		Property::Ptr truck()const { return truck_; }
		Property::Ptr boat()const { return boat_; }
		Property::Ptr plane()const { return plane_; }
		void truckIs(Property::Ptr _truck) { truck_ = _truck; }
		void boatIs(Property::Ptr _boat) { boat_ = _boat; }
		void planeIs(Property::Ptr _plane) { plane_ = _plane; }

		void capacityIs(std::string _mode, Capacity _val);
		void costIs(std::string _mode, Cost _val);
		void speedIs(std::string _mode, Speed _val);
		Capacity capacity(std::string _mode);
		Cost cost(std::string _mode);
		Speed speed(std::string _mode);
		Time offPeakTimeStart() { return offPeakTimeStart_; }
		Time offPeakTimeEnd() { return offPeakTimeEnd_; }
		void timeNowIs(Time _time);
		void offPeakTimeStartIs(Time _offPeakTimeStart) { offPeakTimeStart_ = _offPeakTimeStart; }
		void offPeakTimeEndIs(Time _offPeakTimeEnd) { offPeakTimeEnd_ = _offPeakTimeEnd; }
		void offPeakCostScaleIs(Percentage _costScale) { costScale_ = _costScale; }
		void offPeakCapacityScaleIs(Percentage _capacityScale) { capacityScale_ = _capacityScale; }
	protected:
		Fleet(std::string name_);
		Fleet(Fleet const&); // prevent copy construction
		Fleet& operator= (Fleet const&); // prevent copying

	private:
		std::string name_;
		Property::Ptr  truck_, boat_, plane_;
		Percentage costScale_, capacityScale_;
		Time offPeakTimeStart_; // Time in 24 hour scale
		Time offPeakTimeEnd_;
	};

	class EngineManager : public Fwk::PtrInterface<EngineManager>
	{
	public:
		typedef Fwk::Ptr<EngineManager const> PtrConst;
		typedef Fwk::Ptr<EngineManager> Ptr;

		static Ptr EngineManagerNew() { return new EngineManager(); }

		Segment::Ptr SegmentNew(std::string _name, Segment::ModeType _mode);
		BoatSegment::Ptr BoatSegmentNew(std::string _name);
		TruckSegment::Ptr TruckSegmentNew(std::string _name);
		PlaneSegment::Ptr PlaneSegmentNew(std::string _name);

		Location::Ptr LocationNew(std::string _name, Location::LocationType _locationType);
		CustomerLocation::Ptr CustomerLocationNew(std::string _name);
		PortLocation::Ptr PortLocationNew(std::string _name);
		BoatLocation::Ptr BoatLocationNew(std::string _name);
		TruckLocation::Ptr TruckLocationNew(std::string _name);
		PlaneLocation::Ptr PlaneLocationNew(std::string _name);

		static Connectivity::Ptr connectivityNew(std::string name_);
		Statistics::Ptr statisticsNew(std::string _name);
		Fleet::Ptr fleetNew(std::string _name);

		static Fleet::Ptr fleetInstance() { return fleetInstance_; }
		static Connectivity::Ptr connInstance() { return connInstance_; }
		static Statistics::Ptr statsInstance() { return statsInstance_; }
		static EngineManager::Ptr instance();

		static std::map<Segment::ModeType, std::string> modeTypeTable_;
		static Segment::Ptr segmentMap(std::string _segmentName) {
			if (segmentMap_.count(_segmentName) > 0) {
				Segment::Ptr s = segmentMap_.find(_segmentName)->second;// second()->ptr() ??
				return s;
			}
		}
		static Location::Ptr locationMap(std::string _locName);
		static int locationMapSize() { return locationMap_.size(); }
		static std::map<std::string, Location::Ptr >::iterator locationMapIter() { return locationMap_.begin(); }
		static int segmentMapSize() { return segmentMap_.size(); }
		static std::map<std::string, Segment::Ptr>::iterator segmentMapIter() { return segmentMap_.begin(); }
		//static Log<PtrInterface> log() { return log_; }

		class Notifiee : public Fwk::BaseNotifiee<EngineManager> {
		public:
			typedef Fwk::Ptr<Notifiee> Ptr;

			Notifiee(EngineManager* engineManager) : Fwk::BaseNotifiee<EngineManager>(engineManager) {}

			virtual void onLocationNew(Location::Ptr) {}
			virtual void onSegmentNew(Segment::Ptr) {}
			virtual void onConnectivityNew(Connectivity::Ptr) {}



		};
		Fwk::Ptr<Notifiee> notifiee() { return notifiee_; }
		void lastNotifieeIs(Notifiee* n) {

			EngineManager* me = const_cast<EngineManager*>(this);
			me->notifiee_ = n;
		}



		typedef std::map<std::string, Location::Ptr > LocationMap;
		typedef std::map<std::string, Segment::Ptr> SegmentMap;
		static Log::Ptr log() { return log_; }
		static void runDijkstraAlgo();
		static void findLeastLoadPaths();
	protected:
		EngineManager();
		static std::map<std::string, Location::Ptr > locationMap_;
		Notifiee* notifiee_;

		static Connectivity::Ptr connInstance_;
		static Statistics::Ptr statsInstance_;
		static Fleet::Ptr fleetInstance_;
		static EngineManager::Ptr instance_;
		static Log::Ptr log_;
		// ActivityImpl::ActivityManagerReactor* amrPtr;
		 //static Log<Fwk::PtrInterface > log_;
		void onZeroReferences();
		static std::map<std::string, Segment::Ptr> segmentMap_;
		void updateRoutingTables(CustomerLocation::Ptr customerLoc);
		static set<string> getConnectingNodes(Location::Ptr startLocPtr, Location::Ptr destLocPtr,
			std::vector<Location::Ptr> path, std::string pathString,
			std::string segToAdd);

	};

	class ShipmentSegmentReactor : public Segment::Notifiee {
	public:
		typedef Fwk::Ptr<ShipmentSegmentReactor const> PtrConst;
		typedef Fwk::Ptr<ShipmentSegmentReactor> Ptr;

		static ShipmentSegmentReactor::Ptr ShipmentSegmentReactorIs(Segment::Ptr s);
		void onCurrentShipmentsCountDec(string _source, string _segment) {
			//notify the source of this segment that the segment is free
			cout << " In on curr shipments count dec";
			//EngineManager::locationMap(_source)->shipmentIs();
		}
	protected:
		ShipmentSegmentReactor(Segment* s) : Segment::Notifiee(s) { }
	};

	class SegmentReactor : public Segment::Notifiee {
	public:
		typedef Fwk::Ptr<SegmentReactor const> PtrConst;
		typedef Fwk::Ptr<SegmentReactor> Ptr;

		static SegmentReactor::Ptr SegmentReactorIs(Segment::ExpediteSupport exp, Segment::Ptr s);
		static SegmentReactor::Ptr SegmentReactorIs(Segment::Ptr s);
		void onExpediteSupportIs(Segment::ExpediteSupport _newValue, Segment::ExpediteSupport _previousValue);
		void onCurrentShipmentsCountDec(string _source, string _segment) {
			cout << " \nIn on curr shipments count dec in class segmentreactor\n";
			cout << "Resending to location = " << _source;
			//if(!pendingShipmentSchedule[_segment].empty()){
			EngineManager::locationMap(_source)->pendingShipmentSchedule(_segment);
			//}
		}
		//void SegmentReactor::onSourceIs(std::string _prevLoc, std::string _newLoc, std::string _segmentName);
		Percentage static expeditePercentage() { return Percentage(expeditePercentage_); }

	protected:
		static double expeditePercentage_;
		static int expeditedSegments_;
		SegmentReactor(Segment* s) : Segment::Notifiee(s) { }
	};


	class EngineReactor : public EngineManager::Notifiee {
	public:
		typedef Fwk::Ptr<EngineReactor const> PtrConst;
		typedef Fwk::Ptr<EngineReactor> Ptr;

		void onLocationNew(Location::Ptr locPtr);
		void onSegmentNew(Segment::Ptr segPtr);
		static EngineReactor::Ptr EngineReactorIs(EngineManager* emPtr);

		int customerLocationCount() { return customerLocationCount_; }
		int portCount() { return portCount_; }
		int truckTerminalCount() const { return truckTerminalCount_; }
		int boatTerminalCount() const { return boatTerminalCount_; }
		int planeTerminalCount()const { return planeTerminalCount_; }
		int boatSegmentCount() const { return boatSegmentCount_; }
		int truckSegmentCount() const { return truckSegmentCount_; }
		int planeSegmentCount()const { return planeSegmentCount_; }

	protected:
		int boatSegmentCount_, truckSegmentCount_, planeSegmentCount_;
		int customerLocationCount_, portCount_, truckTerminalCount_, boatTerminalCount_, planeTerminalCount_;
		EngineReactor(EngineManager* emPtr) : EngineManager::Notifiee(emPtr), boatSegmentCount_(0),
			truckSegmentCount_(0),
			planeSegmentCount_(0),
			customerLocationCount_(0),
			portCount_(0),
			truckTerminalCount_(0),
			boatTerminalCount_(0),
			planeTerminalCount_(0) { }
	};

	//~~added


	class CustomerLocationReactor : public CustomerLocation::Notifiee {
		// Method to start shipments sim once all req attributes are set
	public:
		typedef Fwk::Ptr<CustomerLocationReactor const> PtrConst;
		typedef Fwk::Ptr<CustomerLocationReactor> Ptr;
		virtual void onShipmentIs(Shipment::Ptr _shipment);
		void onTransferRateIs(Rate _transferRate);
		static CustomerLocationReactor::Ptr CustomerLocationReactorIs();

		//private:
		string locationName_;
		CustomerLocationReactor(CustomerLocation * customerLocation) : CustomerLocation::Notifiee(customerLocation) { }
	};


} // end namespace
#endif