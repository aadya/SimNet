#include "Engine.h"
#include "ActivityReactor.h"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include<map>
#include<set>
#include<vector>
#include<algorithm>
#include<iterator>
#include<math.h>
#include <limits.h>
using namespace std;
using std::cout;
using std::cerr;
using std::endl;

using namespace Shipping;

std::map<std::string, Location::Ptr > EngineManager::locationMap_;
std::map<std::string, Segment::Ptr> EngineManager::segmentMap_;
Connectivity::Ptr EngineManager::connInstance_;
Statistics::Ptr EngineManager::statsInstance_;
Fleet::Ptr EngineManager::fleetInstance_;
EngineManager::Ptr EngineManager::instance_;
std::map<Segment::ModeType, std::string> EngineManager::modeTypeTable_;
double SegmentReactor::expeditePercentage_ = 0.0;
int SegmentReactor::expeditedSegments_ = 0;
std::vector<Fwk::Ptr<SegmentReactor> > Statistics::srInstances_;
Log::Ptr EngineManager::log_;

//----------| Location Implementation |------------//
Location::Location(std::string _name, Location::LocationType _locationType)
{

	name_ = _name;
	locationType_ = _locationType;
}

void Location::shipmentIs(Shipment::Ptr _shipment) {
	//notifiee_->onShipmentIs(_shipment);
	//Check if all packages of this shipment have been received - otherwise do nothing
	cout << "\nshipment received by location " << name_;
	//if(_shipment->packagesDeliveredCount().value() == _shipment->numberOfPackages().value()){
	cout << " \nAll packages delivered ";
	//update the statistics for the delivering segment
	if (_shipment->currSegment() != "") {
		EngineManager::segmentMap(_shipment->currSegment())->currentShipmentsCountDec();
		_shipment->currSegmentIs("");
	}

	string selectedSegment = findRoutingSegment(_shipment->destination(), _shipment->visitedNodes());
	if (selectedSegment != "") {
		//Check if the segment is under-capacity
		Segment::Ptr selectedSegmentPtr = EngineManager::segmentMap(selectedSegment);
		if (selectedSegmentPtr->currentShipmentsCount().value() < selectedSegmentPtr->capacity().value()) {
			selectedSegmentPtr->shipmentIs(_shipment, Shipment::no_);
		}
		else {
			//Keep the shipment waiting until a segment becomes free, update segment stats
			//hook in a reactor to the selected segment's currentShipmentsSize variable and wait on it
			cout << "creating new shipment segment reactor";
			cout << " \n Segment " << selectedSegment << " does not have enough capacity to carry this shipment - adding to pending";
			pendingShipments_[selectedSegment].push_back(_shipment);
			if (!segReactor)
				segReactor = SegmentReactor::SegmentReactorIs(selectedSegmentPtr);

		}

	}
	//No routing segment could be found => path to destination does not exist
	else {
		cerr << " No path found from source = " << name_ << " to destination = " << _shipment->destination();
	}
	//}
	// All the packages have not been delivered - do nothing
}
void Location::pendingShipmentSchedule(string _segment) {
	if (!pendingShipments_[_segment].empty()) {
		Shipment::Ptr shipment = pendingShipments_[_segment].front();
		pendingShipments_[_segment].pop_front();
		if (shipment != NULL) {
			//Check if the segment is under-capacity
			Segment::Ptr selectedSegmentPtr = EngineManager::segmentMap(_segment);
			if (selectedSegmentPtr->currentShipmentsCount().value() < selectedSegmentPtr->capacity().value()) {
				cout << "\nRescheduling a shipment";
				selectedSegmentPtr->shipmentIs(shipment, Shipment::yes_);
			}
			else {
				//put it back into the pending shipments list
				cout << " \npushing into pending list again!";
				pendingShipments_[_segment].push_back(shipment);

			}

		}
	}
}
/*
//~~added
void Location::receiveShipment(Shipment::Ptr _shipment){
 // decide which segment to forward the shipment on
	Segment::Ptr selectedSegment = findRoutingSegment();
	if(selectedSegment!=NULL){
		//Check if the segment is under-capacity
		if(selectedSegment->currentShipmentsSize() < selectedSegment->capacity()){
			selectedSegment->shipmentIs(_shipment);
		}
		else
				//keep the shipment waiting until a segment becomes free
	}
	else
		//This location cannot fwd shipments .. send it back to the previous location to retry

}

*/


CustomerLocation::CustomerLocation(std::string _name) :
	Location(_name, Location::customer_),
	transferRate_(0),
	shipmentSize_(0),
	destination_(""),
	shipmentsReceived_(0),
	averageLatency_(0.0),
	shipmentsReceivedTotalCost_(0.0),
	totalShipmentsTime_(0.0),
	numberPerHour_(0) {

	//create a new reactor for this customer location
	//CustomerLocationReactor::Ptr customerReactor = CustomerLocationReactor::CustomerLocationReactorIs();
	//customerReactor->notifierIs
}

void CustomerLocation::shipmentIs(Shipment::Ptr _shipment) {

	// cout<<" calling notifiee";
   // notifiee_->onShipmentIs(_shipment);
	cout << " \nCust loc " << name_ << " received shipment ";

	this->receiveShipment(_shipment);

}

void CustomerLocation::receiveShipment(Shipment::Ptr _shipment) {
	//If this customer location is the source, decide on which segment to schedule it on

	//If this customer location is the intended destination, update stats and return
	if ((_shipment->destination()).compare(this->name()) == 0) {
		cout << "\n Reached Destination!";
		EngineManager::segmentMap(_shipment->currSegment())->currentShipmentsCountDec();
		_shipment->currSegmentIs("");

		shipmentsReceived_ = Count(shipmentsReceived_.value() + 1);
		Time endTime = Time(activityManagerInstance()->now().value());
		totalShipmentsTime_ = Time(totalShipmentsTime_.value() + (endTime.value() - _shipment->startTime().value()));
		averageLatency_ = Latency(totalShipmentsTime_.value() / shipmentsReceived_.value());
		shipmentsReceivedTotalCost_ = Cost(shipmentsReceivedTotalCost_.value() + _shipment->shipmentCost().value());
		cout << "-------Statistics--------------";
		cout << "\nCustomer location = " << name_;
		cout << "\n Shipments received = " << shipmentsReceived_.value();
		cout << "\n Latency = " << averageLatency_.value();
		cout << "\n Total cost = " << shipmentsReceivedTotalCost_.value();
	}
	//Otherwise, return it to the previous source
	//Get the segment along which it came
	else {
		//	_shipment->currentSegmentIs(EngineManager::segmentMap(_shipment->prevSegment()->returnSegmentName()));
		cerr << " Shipment not destined for this customer location ! - " << name_;
	}
}

void CustomerLocation::shipmentNew() {
	EngineManager::runDijkstraAlgo();
	EngineManager::findLeastLoadPaths();
	int numShipments = numberPerHour_.value();
	//Adjust for the number of packages set by the transfer rate at the end of the day
	if ((int)(activityManagerInstance()->now().value()) % 24 == 0 && activityManagerInstance()->now().value() != 0) {
		numShipments = transferRate_.value() - (23 * numberPerHour_.value());
		cout << "last hour.. changing num shipments to " << numShipments;
	}

	for (int i = 0; i < numShipments; i++) {
		string shipmentName = "shipment_"; //TODO append the number of the shipment/location to the name
		cout << "\n Created New Shipment: source = " << name() << ", Destination = " << destination_;
		Shipment::Ptr shipment = Shipment::ShipmentNew(shipmentName, this->name_, this->destination_, this->shipmentSize_);
		string selectedSegment = findRoutingSegment(shipment->destination(), shipment->visitedNodes());
		cout << "\n Sending out shipment on segment " << selectedSegment;
		Segment::Ptr selectedSegmentPtr = EngineManager::segmentMap(selectedSegment);
		if (selectedSegmentPtr->currentShipmentsCount().value() < selectedSegmentPtr->capacity().value()) {
			EngineManager::segmentMap(selectedSegment)->shipmentIs(shipment, Shipment::no_);
		}
		else {
			//Keep the shipment waiting until a segment becomes free, update segment stats
			//hook in a reactor to the selected segment's currentShipmentsSize variable and wait on it
			cout << "creating new shipment segment reactor";
			cout << " \n Segment " << selectedSegment << " does not have enough capacity to carry this shipment - adding to pending";
			pendingShipments_[selectedSegment].push_back(shipment);
			if (!segReactor)
				segReactor = SegmentReactor::SegmentReactorIs(selectedSegmentPtr);

		}


	}
}

void CustomerLocation::transferRateIs(Rate _transferRate) {
	transferRate_ = _transferRate;
	//call the notifiee for this location
	//notifiee_->onTransferRateIs(_transferRate, this);
	//aliter: call a method to check the attributes and create activity if required
	if (_transferRate == 0) {
		activityDel();
	}
	else {
		checkCustomerLocationReady();
	}


}

void CustomerLocation::destinationIs(string _destination) {
	if (_destination == "") {
		destination_ = _destination;
		activityDel();
	}
	else if (EngineManager::locationMap(_destination)->locationType() == Location::customer_) {
		destination_ = _destination;
		checkCustomerLocationReady();
	}
	else {
		cerr << " Destination not of type customer location!!";
		//log error
		//EngineManager::log()->entryNew(Log<CustomerLocation>::Error, this,"CustomerLocation::transferRateIs"										,"Destination specified is not an existing customer location" );
	}
}

void CustomerLocation::shipmentSizeIs(Count _shipmentSize) {
	shipmentSize_ = _shipmentSize;
	if (shipmentSize_ == 0) {
		activityDel();
	}
	else {
		checkCustomerLocationReady();
	}
}

void CustomerLocation::checkCustomerLocationReady() {
	//Create a new injection activity if one does not already exist
	Activity::Manager::Ptr manager = activityManagerInstance();
	if (manager->activity(name_) == NULL) {
		if ((transferRate_ != 0) && (shipmentSize_ != 0) && (destination_ != "")) {
			//Calculate the rate at which the shipments are to be sent
			//x shipments/per day => have to send shipments every 24/x hours,
				//also calculate how many shipments to send off each time
			//double numberPerHour =0 ;
			int frequencyInHours = 0;

			if (transferRate_ > 24) {
				frequencyInHours = 1;
				numberPerHour_ = floor(transferRate_.value() / 24.0);
				//spillOver = transferRate%24; //send all of these in the last hour?
			}
			else {
				numberPerHour_ = 1;
				frequencyInHours = floor(24.0 / transferRate_.value());
			}
			Activity::Ptr sourceActivity = manager->activityNew(name_);
			cout << "\n Creating reactor with freq = " << frequencyInHours << " and number per hour = " << numberPerHour_.value();
			sourceActivity->lastNotifieeIs(new InjectShipmentReactor(manager, sourceActivity.ptr(), frequencyInHours, this));
			sourceActivity->statusIs(Activity::nextTimeScheduled);
		}
	}
}

string Location::findRoutingSegment(string destination, set<string> visitedNodes) {
	string nextSegment = "";
	try {
		Connectivity::RoutingAlgo routingAlgorithm = EngineManager::connInstance()->routingAlgo();
		std::map<string, string>::iterator iter;
		switch (routingAlgorithm) {

		case Connectivity::LeastLoadPath_:
			nextSegment = findLeastLoadSegment(destination, visitedNodes);
			break;
		case Connectivity::LeastCostPath_:

			iter = leastCostRoutingTable_.find(destination);
			if (iter != leastCostRoutingTable_.end()) {
				nextSegment = iter->second;
			}
			break;
		default: throw new Fwk::RangeException("Routing algorithm value out of range");

		}
	}
	catch (Fwk::RangeException& e) {
		EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<Location>(this), "Location::findRoutingSegment", "Invalid routing algorithm specified");
		throw e;
	}
	cout << "Returning segment -" << nextSegment;
	return nextSegment;
}

//Populate the leastLoad routing table for this source to all reachable destinations
void EngineManager::findLeastLoadPaths() {
	// for all other destination nodes
	// call connect (src,dest)
	// fill up one row of the routing table with (dest, list of next hops)
	//after all nodes are done, write out the routing table to the destination
	LocationMap::iterator sourceLocMapIter;
	for (sourceLocMapIter = locationMap_.begin(); sourceLocMapIter != locationMap_.end();sourceLocMapIter++) {

		Location::Ptr sourceLocPtr = sourceLocMapIter->second;
		LocationMap::iterator locMapIter;
		std::map<string, set<string> > routingTable;
		string source = sourceLocPtr->name();
		cout << " populating table for source location = |" << source << "|";
		for (locMapIter = locationMap_.begin(); locMapIter != locationMap_.end(); locMapIter++) {
			string dest = locMapIter->first;
			Location::Ptr destLocPtr = locMapIter->second;
			std::vector<Location::Ptr> path;
			//cout<<"\n ************Dest = "<<dest;
			set<string> allRoutingSegments = getConnectingNodes(sourceLocPtr, destLocPtr, path, " ", " ");

			routingTable[dest] = allRoutingSegments;
		}

		(EngineManager::locationMap(source))->leastLoadRoutingTable_ = routingTable;
	}
}

set<string> EngineManager::getConnectingNodes(Location::Ptr startLocPtr, Location::Ptr destLocPtr,
	std::vector<Location::Ptr> path, std::string pathString,
	std::string segToAdd) {
	set<string> outputPaths;

	if (segToAdd.compare(" ") != 0 && pathString.compare(" ") == 0) {
		pathString = segToAdd;
	}
	path.push_back(startLocPtr);

	if (startLocPtr->name().compare(destLocPtr->name()) == 0) {
		//cout<<" \nFound path to dest .. appending segment : "<<pathString;
		set<std::string> singleString;
		singleString.insert(pathString);
		return singleString;
	}
	else if (startLocPtr->locationType() == Location::customer_ && path.size() > 1) {
		set<std::string> singleString;
		return singleString;
	}

	// Loop through all the segments from this start location and see if they lead to a connection
	for (int i = 1; i <= startLocPtr->segmentsSize(); i++) {

		Segment::Ptr seg = EngineManager::segmentMap(startLocPtr->segment(i));
		if (seg) {

			Location::Ptr nextLocPtr = EngineManager::locationMap((EngineManager::segmentMap(seg->returnSegmentName()))->source());
			if (nextLocPtr) {
				//Check if the nextLoc already exists in path
				std::vector<Location::Ptr>::iterator pathIter = find(path.begin(), path.end(), nextLocPtr);
				if (pathIter == path.end()) {
					std::string segString = seg->name();
					set<std::string> returnedPaths = getConnectingNodes(nextLocPtr, destLocPtr, path, pathString, segString);
					//insert the returned value/s into the output set if they arent already there
					set<std::string>::iterator outputSetIter;
					for (outputSetIter = returnedPaths.begin(); outputSetIter != returnedPaths.end(); outputSetIter++) {

						outputPaths.insert(*outputSetIter);
					}
				}
			}
			else {
				//fprintf(stderr, "Error finding location pointer using return segment");
				string errMsg = "Error finding location pointer using return segment : ";
				errMsg.append(seg->name());
				//throw EntityNotFoundException(errMsg);
				//EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<EngineManager>(this), "EngineManager::getConnectingNodes", errMsg.c_str());

			}
		}
		else {
			//fprintf(stderr, "Error finding segment for start location");
			string errMsg = "Error finding segment for start location : ";
			errMsg.append(startLocPtr->name());
			//throw EntityNotFoundException(errMsg);
			//EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<EngineManager>(this), "EngineManager::getConnectingNodes", errMsg.c_str());
		}
	}
	return outputPaths;
}

//Find the least cost routing table for source
void EngineManager::runDijkstraAlgo() {
	//for each location loc in the graph, init dist[loc] = infinity (distance from source)
	// and next[loc] = undefined (the next location on the shortest path from the source to loc
	LocationMap::iterator sourceLocMapIter;
	for (sourceLocMapIter = locationMap_.begin(); sourceLocMapIter != locationMap_.end();sourceLocMapIter++) {
		Location::Ptr sourceLocPtr = sourceLocMapIter->second;
		LocationMap::iterator locMapIter;
		std::map<string, string> prevNode;
		std::map<string, string> prevSeg;
		std::map<string, double> costTable;
		for (locMapIter = locationMap_.begin(); locMapIter != locationMap_.end(); locMapIter++) {
			string dest = locMapIter->first;
			costTable[dest] = std::numeric_limits<double>::infinity();
			prevNode[dest] = "";
		}
		//cout<<"\nInitialized";
		costTable[sourceLocPtr->name()] = 0;

		//Initialize the set of unfinished nodes
		set<string> unMarkedNodes;
		for (locMapIter = locationMap_.begin(); locMapIter != locationMap_.end(); locMapIter++) {
			unMarkedNodes.insert(locMapIter->first);
		}

		while (!unMarkedNodes.empty()) {
			//Find the node with the smallest dist of all the unmarked nodes
			set<string>::iterator setIter;
			double lowestCost = std::numeric_limits<double>::infinity();
			string nearestLocation;
			for (setIter = unMarkedNodes.begin(); setIter != unMarkedNodes.end(); setIter++) {
				string anotherLocation = *setIter;
				if (costTable[anotherLocation] < lowestCost) {
					lowestCost = costTable[anotherLocation];
					nearestLocation = anotherLocation;
				}
			}

			//cout<<"\n Selected location with least cost ="<<nearestLocation;
			//Break condition - no other nodes can be reached
			if (lowestCost == std::numeric_limits<double>::infinity()) {
				//cout<<"\n no other locations..exiting";
				break;

			}

			unMarkedNodes.erase(nearestLocation);

			//Find all the locations reachable from this location and update their costs
			Location::Ptr nearestLocPtr = locationMap(nearestLocation);
			if ((nearestLocation.compare(sourceLocPtr->name()) == 0) || (nearestLocPtr->locationType() != Location::customer_)) {
				for (int i = 1; i <= nearestLocPtr->segmentsSize(); i++) {

					Segment::Ptr seg = EngineManager::segmentMap(nearestLocPtr->segment(i));
					if (seg) {
						Location::Ptr nextTarget = EngineManager::locationMap((EngineManager::segmentMap(seg->returnSegmentName()))->source());
						if (nextTarget) {
							//if(nearestLocation.compare(sourceLocPtr->name()) == 0 || nextTarget->locationType()!= Location::customer_){
							//if(nearestLocation.compare(sourceLocPtr->name()) == 0)
								//cout<<" source = "<<sourceLocPtr->name()<<" = "<<nearestLocation<<" is true!"<<"\n now considering target = "<<nextTarget->name();

							//cout<<"\n considering.. "<<nextTarget->name();
							//compare the costs and update if necessary
							double segCost = Connectivity::segmentCost(seg->mode(), seg->difficulty(), seg->length(), Segment::no_).value();
							double altCost = costTable[nearestLocation] + segCost;
							if (altCost < costTable[nextTarget->name()]) {
								//cout<<"\nalt = "<<altCost<<" orig = "<< costTable[nextTarget->name()];
								//cout<<"\n found cost lower .. updating ..";

								//if(nextTarget->locationType() == Location::customer_)
									//costTable[nextTarget->name()] = std::numeric_limits<double>::infinity();
								//else
								costTable[nextTarget->name()] = altCost;
								prevNode[nextTarget->name()] = nearestLocation;
								prevSeg[nextTarget->name()] = seg->name();

							}
						}
						else {
							//EngineManager::log()->entryNew(Log::Warning, Fwk::Ptr<EngineManager>(this), "EngineManager::runDijkstraAlgo", "Could not find return segment");
						}
						//}
					}
					else {
						string errMsg = "Invalid segment reference found for location : ";
						errMsg.append(nearestLocPtr->name());
						//EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<EngineManager>(this), "EngineManager::runDijkstraAlgo", "Invalid segment reference found for location : ");
					}

				}
			}

		}
		//cout<<"\n source ="<<sourceLocPtr->name();
		//backtrack through the prev nodes list to find the next hop for each possible target
		std::map<string, string> routingTable;
		for (locMapIter = locationMap_.begin(); locMapIter != locationMap_.end(); locMapIter++) {
			string target = locMapIter->first;
			string tempLoc;
			string tempSeg;
			//If no prev node was found, there is not path from the source to this node
			if (prevNode[target] == "")
				tempLoc = "";
			else {
				tempLoc = target;
				tempSeg = prevSeg[target];
				while ((prevNode[tempLoc]).compare(sourceLocPtr->name()) != 0) {

					tempLoc = prevNode[tempLoc];
					tempSeg = prevSeg[tempLoc];
				}
				//cout<<"\n Target = "<< target<<" nextHop = "<<tempSeg;
			}
			routingTable[target] = tempSeg;
			//cout<<"\nnext hop to target: "<< target<<" = "<<tempLoc<<" seg to use = "<<tempSeg;
		}

		(EngineManager::locationMap(sourceLocPtr->name()))->leastCostRoutingTable_ = routingTable;
	}
}

string Location::findLeastLoadSegment(string destination, set<string> visitedNodes) {
	string nextSegment = "";
	int leastLoad;
	leastLoad = INT_MAX;
	Segment::Ptr seg;


	set<string> possibleSegs = leastLoadRoutingTable_[destination];
	set<string>::iterator segIter;
	//Iterate over all the possible next hop segments for this source-destination
	// and select the one with the most free capacity (most difference between capacity and actual load

	for (segIter = possibleSegs.begin();segIter != possibleSegs.end(); segIter++) {
		//cout<<" found segment = |"<< *segIter<<"|";
		if (*segIter != " ") {
			Segment::Ptr seg = EngineManager::segmentMap(*segIter);
			if (seg != NULL) {
				string nextLocation = EngineManager::segmentMap(seg->returnSegmentName())->source();
				//Test if the location reachable through this segment is suitable i.e
				// it has not already been traversed by the shipment
				if (visitedNodes.find(nextLocation) == visitedNodes.end()) {
					int segCapacity = seg->capacity().value();
					int segLoad = seg->currentShipmentsCount().value();
					int freeCapacity = segCapacity - segLoad;
					//cout<<" \nconsidering segment "<<seg->name()<<" segcap = "<<segCapacity<<"seg load = "<<segLoad;
					if (freeCapacity < leastLoad) {
						//cout<<"updating..";
						nextSegment = seg->name();
						leastLoad = seg->currentShipmentsCount().value();

					}
				}
			}
		}
	}
	//cout<<"\n Least load found = "<<leastLoad;
	//cout<<"\n next seg found = "<<nextSegment;
	return nextSegment;
}

PortLocation::PortLocation(std::string _name) :
	Location(_name, Location::port_) {

}

BoatLocation::BoatLocation(std::string _name) :
	Location(_name, Location::boatTerminal_) {

}

PlaneLocation::PlaneLocation(std::string _name) :
	Location(_name, Location::planeTerminal_) {

}
TruckLocation::TruckLocation(std::string _name) :
	Location(_name, Location::truckTerminal_) {

}

void Location::addSegment(std::string _segmentName) {

	segment_.push_back(_segmentName);
}

void Location::removeSegment(std::string _segmentName) {
	std::vector<std::string>::iterator iter = find(segment_.begin(), segment_.end(), _segmentName);
	if (iter != segment_.end()) {
		segment_.erase(iter);
	}
}
/*~~~~~EngineManager Implementation~~~~~*/

Location::Ptr EngineManager::LocationNew(std::string _name, Location::LocationType _locationType)
{
	//Creates the new location and stores it in the location map

	Location::Ptr l = new Location(_name, _locationType);
	if (notifiee_)
		notifiee_->onLocationNew(l);
	locationMap_.insert(pair<std::string, Location::Ptr>(_name, l));
	return l;
}

CustomerLocation::Ptr EngineManager::CustomerLocationNew(std::string _name)
{
	//Creates the new location and stores it in the location map
	CustomerLocation::Ptr l = new CustomerLocation(_name);
	l->lastNotifieeIs(new CustomerLocationReactor(l.ptr()));

	if (notifiee_)
		notifiee_->onLocationNew(l);
	locationMap_[_name] = l;
	return l;
}

void EngineManager::updateRoutingTables(CustomerLocation::Ptr customerLoc) {
	//Update routes from all other customer locations to this location

	//Iterate through all the locations in locationMap
	//call connectivity::connect to find all possible paths with
	// (i)this loc as src
	// (ii) this loc as destination
	//Create the routing table for this destination, update all the other routing tables
	EngineManager::LocationMap::iterator locMapIter;
	for (locMapIter = locationMap_.begin(); locMapIter != locationMap_.end(); locMapIter++) {
		Location::Ptr oldCustomerLoc = locMapIter->second;
		//Update the routing table for the old location

		//leastCostRoutingTable_.insert(pair<string, string>())

	}

}


PortLocation::Ptr EngineManager::PortLocationNew(std::string _name) {
	PortLocation::Ptr l = new PortLocation(_name);
	locationMap_[_name] = l;
	if (notifiee_)
		notifiee_->onLocationNew(l);
	return l;
}
TruckLocation::Ptr EngineManager::TruckLocationNew(std::string _name) {
	TruckLocation::Ptr l = new TruckLocation(_name);
	locationMap_[_name] = l;
	if (notifiee_)
		notifiee_->onLocationNew(l);
	return l;
}

BoatLocation::Ptr EngineManager::BoatLocationNew(std::string _name) {
	BoatLocation::Ptr l = new BoatLocation(_name);
	locationMap_[_name] = l;
	if (notifiee_)
		notifiee_->onLocationNew(l);
	return l;
}

PlaneLocation::Ptr EngineManager::PlaneLocationNew(std::string _name) {
	PlaneLocation::Ptr l = new PlaneLocation(_name);
	locationMap_[_name] = l;
	if (notifiee_)
		notifiee_->onLocationNew(l);
	return l;
}


EngineManager::Ptr EngineManager::instance() {
	if (instance_ == NULL)
		instance_ = new EngineManager();
	return instance_;
}


Segment::Ptr EngineManager::SegmentNew(std::string _name, Segment::ModeType _mode) {
	Segment::Ptr s = new Segment(_name, _mode);
	//EngineManager::segmentMap_[_name]=s;
	segmentMap_.insert(pair<std::string, Segment::Ptr>(_name, s));

	//Code to call notifiees
	if (notifiee_)
		notifiee_->onSegmentNew(s);


	//hook in segment reactors to track expedited %, if statistics has not been created yet
	if (EngineManager::statsInstance() != NULL) {
		SegmentReactor::Ptr segReactorPtr = SegmentReactor::SegmentReactorIs(Segment::no_, s);
		//segReactorPtr->notifierIs(s);

		//s->lastNotifieeIs(segReactorPtr.ptr());
		Statistics::srInstances_.push_back(segReactorPtr);
	}
	return s;
}

BoatSegment::Ptr EngineManager::BoatSegmentNew(std::string _name) {
	BoatSegment::Ptr  s = SegmentNew(_name, Segment::boat_);
	return s;
}

TruckSegment::Ptr EngineManager::TruckSegmentNew(std::string _name) {
	TruckSegment::Ptr  s = SegmentNew(_name, Segment::truck_);
	return s;
}

PlaneSegment::Ptr EngineManager::PlaneSegmentNew(std::string _name) {
	PlaneSegment::Ptr  s = SegmentNew(_name, Segment::plane_);
	return s;
}

Connectivity::Ptr EngineManager::connectivityNew(std::string _name) {
	if (!connInstance_)
		connInstance_ = new Connectivity(_name);
	return connInstance_;
}

Fleet::Ptr EngineManager::fleetNew(std::string _name) {
	if (!fleetInstance_)
		fleetInstance_ = new Fleet(_name);
	//Hook in a reactor to activity manager to be notified of time
	ActivityImpl::ActivityManagerReactor* amrPtr;
	amrPtr = ActivityImpl::ActivityManagerReactor::ActivityManagerReactorIs(activityManagerInstance().ptr());

	return fleetInstance_;
}

Statistics::Ptr EngineManager::statisticsNew(std::string _name) {
	if (!statsInstance_) {
		statsInstance_ = new Statistics(_name);
		(statsInstance_->erInstance_)->notifierIs(this);

	}
	return statsInstance_;

}


Location::Ptr EngineManager::locationMap(std::string _locName) {

	std::map<std::string, Location::Ptr>::iterator locMapIter = locationMap_.find(_locName);
	if (locMapIter != locationMap_.end()) {
		//cout<<"\n returning loc pointer";
		return locMapIter->second;// second()->ptr() ??
		//return s; // will return a NULL if there is no seg by that name
	}
	return NULL;
}

/*~~~~~Fleet Implementation~~~~~*/

Fleet::Fleet(std::string _name) :costScale_(0.0), capacityScale_(0.0), offPeakTimeStart_(0.0), offPeakTimeEnd_(0.0) {
	name_ = _name;
	truck_ = new Property();
	boat_ = new Property();
	plane_ = new Property();

}

void Fleet::timeNowIs(Time _time) {
	//new time =
	cout << " \nIn fleet " << _time.value();
	//If the time was prev below the cutoff and is now above, reset costs of segments
	cout << " \n in 24 hrs, time = " << fmod(_time.value(), 24.0);
	if (fmod(_time.value(), 24.0) >= offPeakTimeStart_.value()) {
		cout << " \nTime beyond offPeak.. resetting attributes";
		//boat_->costIs(boat_)
	}
}


void Fleet::capacityIs(std::string _mode, Capacity _val) {

	if (_mode.compare("Boat") == 0) {
		boat_->capacityIs(_val);
	}
	else if (_mode.compare("Truck") == 0) {
		truck_->capacityIs(_val);
	}
	else if (_mode.compare("Plane") == 0) {
		plane_->capacityIs(_val);
	}

}

void Fleet::costIs(std::string _mode, Cost _val) {
	if (_mode.compare("Boat") == 0) {
		boat_->costIs(_val);
	}
	else if (_mode.compare("Truck") == 0) {
		truck_->costIs(_val);
	}
	else if (_mode.compare("Plane") == 0) {
		plane_->costIs(_val);
	}

}

void Fleet::speedIs(std::string _mode, Speed _val) {
	if (_mode.compare("Boat") == 0) {
		boat_->speedIs(_val);
	}
	else if (_mode.compare("Truck") == 0) {
		truck_->speedIs(_val);
	}
	else if (_mode.compare("Plane") == 0) {
		plane_->speedIs(_val);
	}

}

Capacity Fleet::capacity(std::string _mode) {
	if (_mode.compare("Boat") == 0) {
		return boat_->capacity();
	}
	else if (_mode.compare("Truck") == 0) {
		return truck_->capacity();
	}
	else if (_mode.compare("Plane") == 0) {
		return plane_->capacity();
	}
	return Capacity(0);
}


Cost Fleet::cost(std::string _mode) {
	if (_mode.compare("Boat") == 0) {
		return boat_->cost();
	}
	else if (_mode.compare("Truck") == 0) {
		return truck_->cost();
	}
	else if (_mode.compare("Plane") == 0) {
		return plane_->cost();
	}
	return Cost(1.0);
}


Speed Fleet::speed(std::string _mode) {
	try {
		if (_mode.compare("Boat") == 0) {
			return boat_->speed();
		}
		else if (_mode.compare("Truck") == 0) {
			return truck_->speed();
		}
		else if (_mode.compare("Plane") == 0) {
			return plane_->speed();
		}
		else {
			throw Fwk::EntityNotFoundException("Entity Type requested not found : "/*+entityType*/);
			EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<Fleet>(this), "Fleet::speed", "Entity Type requested not found : ");
		}
	}
	catch (Fwk::EntityNotFoundException& e) {
		EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<Fleet>(this), "Fleet::speed", "Entity Type requested not found : ");
	}
	return Speed(1.0);
}


/*~~~~~Statistics Implementation~~~~~*/
int Statistics::entityCount(std::string entityType) {

	if (entityType.compare("Customer") == 0) {
		return erInstance_->customerLocationCount();
	}
	else if (entityType.compare("Port") == 0) {
		return erInstance_->portCount();
	}
	else if (entityType.compare("Truck terminal") == 0) {
		return erInstance_->truckTerminalCount();
	}
	else if (entityType.compare("Boat terminal") == 0) {
		return erInstance_->boatTerminalCount();
	}
	else if (entityType.compare("Plane terminal") == 0) {
		return erInstance_->planeTerminalCount();
	}
	else if (entityType.compare("Truck segment") == 0) {
		return erInstance_->truckSegmentCount();
	}
	else if (entityType.compare("Boat segment") == 0) {
		return erInstance_->boatSegmentCount();
	}
	else if (entityType.compare("Plane segment") == 0) {

		return erInstance_->planeSegmentCount();
	}
	else {
		//throw UnRecognizedNameException("Entity Type requested not found : "+entityType);
		EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<Statistics>(this), "Statistics::entityCount", "Entity Type requested not found : ");
	}

	return 0;

}

Percentage Statistics::expeditePercentage() {
	return SegmentReactor::expeditePercentage();
}

Statistics::Statistics(std::string _name) : expeditePercentage_(0.0) {
	name_ = _name;
	erInstance_ = EngineReactor::EngineReactorIs(EngineManager::instance().ptr());

	EngineManager::SegmentMap::iterator segmentIter = EngineManager::segmentMapIter();
	for (int i = 0; i < EngineManager::segmentMapSize(); i++) {
		Segment::Ptr segPtr = segmentIter->second;
		SegmentReactor::Ptr segReactorPtr = SegmentReactor::SegmentReactorIs(segPtr->expediteSupport(), segPtr);

		//segReactorPtr->notifierIs(segPtr);
		srInstances_.push_back(segReactorPtr);
		++segmentIter;
	}


}

//~~~~~~~~~~Connectivity Implementation~~~~~~~~~~~~//

std::string Connectivity::paths(std::string _pathAttributes) {

	// parse the string and determine if it is to explore or connect
	vector<string> result;
	string functionType;
	string paths = "";
	char* tok;
	char * tokChars = new char[_pathAttributes.size() + 1];
	strcpy(tokChars, _pathAttributes.c_str());
	tok = strtok(tokChars, " ");
	functionType = tok;

	Length dist = 0;
	Cost cost = 0;
	Time time = 0;
	Segment::ExpediteSupport expedited = Segment::no_;

	if (functionType.compare("explore") == 0) {
		string startLoc = strtok(NULL, " ");

		Location::Ptr startlocPtr = EngineManager::locationMap(startLoc);
		if (!startlocPtr) {
			fprintf(stderr, "start location not found for explore request");
			return "";
		}
		else {
			// Parse constraints for the exploration
			strtok(NULL, " "); // ignore the ":"
			while (tok != NULL) {

				tok = strtok(NULL, " ");
				if (tok != NULL) {
					std::string constraintName = tok;

					if (constraintName.compare("cost") == 0) {
						tok = strtok(NULL, " ");
						std::string costValStr = tok;
						double costVal = convertToDouble(costValStr);
						cost = Cost(costVal);
					}
					else if (constraintName.compare("time") == 0) {
						tok = strtok(NULL, " ");
						std::string timeValStr = tok;
						double timeVal = convertToDouble(timeValStr);
						time = Time(timeVal);
					}
					else if (constraintName.compare("distance") == 0) {
						tok = strtok(NULL, " ");
						std::string distanceValStr = tok;
						double distanceVal = convertToDouble(distanceValStr);
						dist = Length(distanceVal);
					}
					else if (constraintName.compare("expedited") == 0) {
						expedited = Segment::yes_;
					}
				}
			}
		}

		std::vector<Location::Ptr> pathSoFar;
		// check if the location exists and get a ptr to it
		std::vector<std::string> paths;
		result = explore(startlocPtr, dist, Length(0),
			cost, Cost(0),
			time, Time(0),
			expedited,
			pathSoFar, "", " ");

	}
	else if (functionType.compare("connect") == 0) {



		string startLoc = strtok(NULL, " ");
		strtok(NULL, " ");
		string destLoc = strtok(NULL, " ");
		Location::Ptr startlocPtr = EngineManager::locationMap(startLoc);
		Location::Ptr destlocPtr = EngineManager::locationMap(destLoc);
		std::vector<Location::Ptr> path;

		if (startlocPtr && destlocPtr) {


			vector<string> expResults;
			vector<string> nonExpResults;

			//Call connect twice, with expedited = true/false

			nonExpResults = connect(startlocPtr, destlocPtr, path, " ", " ", Segment::no_, 0, 0, 0, 0, Segment::yes_);
			expResults = connect(startlocPtr, destlocPtr, path, " ", " ", Segment::yes_, 0, 0, 0, 0, Segment::yes_);
			result.insert(result.end(), expResults.begin(), expResults.end());
			result.insert(result.end(), nonExpResults.begin(), nonExpResults.end());

		}
	}
	paths = convertToString(result);
	return paths;
}

inline double Connectivity::convertToDouble(const std::string& s)
{
	std::istringstream i(s);
	double x;
	i >> x;
	return x;
}

//Recursive method that populates the vector of all possible paths from a starting location within constraints
std::vector<std::string> Connectivity::explore(Location::Ptr startLocPtr,
	Length dist, Length currDist,
	Cost cost, Cost currCost,
	Time time, Time currTime,
	Segment::ExpediteSupport expedited,
	std::vector<Location::Ptr> pathSoFar,
	std::string pathString, std::string segToAdd) {

	std::vector<std::string> outputPaths;
	if (segToAdd.compare(" ") != 0) {
		pathSoFar.push_back(startLocPtr);

		pathString.append(segToAdd);
		pathString.append(convertToString(startLocPtr));

		outputPaths.push_back(pathString);
	}

	for (int i = 1; i <= startLocPtr->segmentsSize(); i++) {
		Segment::Ptr seg = EngineManager::segmentMap(startLocPtr->segment(i));

		if (seg) {
			Location::Ptr nextLocPtr = EngineManager::locationMap((EngineManager::segmentMap(seg->returnSegmentName()))->source());
			if (nextLocPtr) {
				//Check if this node has already been traversed

				std::vector<Location::Ptr>::iterator pathSoFarIter = find(pathSoFar.begin(), pathSoFar.end(), nextLocPtr);
				if (pathSoFarIter == pathSoFar.end()) {

					//Determine if adding this segment satisfies constraints
					Length segDist = seg->length();
					Cost segCost = segmentCost(seg->mode(), seg->difficulty(), seg->length(), expedited);
					Time segTime = segmentTime(seg->mode(), seg->length(), expedited);

					// determine if adding this segment will satisfy constraints

					if ((dist.value() == 0 || dist >= currDist + segDist)
						&& (cost.value() == 0 || cost.value() >= currCost.value() + segCost.value())
						&& (time.value() == 0 || time.value() >= currTime.value() + segTime.value())
						&& !(expedited == Segment::yes_ && seg->expediteSupport() == Segment::no_)) {

						//This segment satisfies the constraints and can be added to the path.
						if (pathString.compare("") == 0) {
							pathSoFar.push_back(startLocPtr);
							pathString.append(startLocPtr->name());
						}
						// add the current location into pathSofar and add this path into paths
						segToAdd = convertToString(seg);
						std::vector<std::string> newPaths = explore(nextLocPtr, dist, Length(currDist.value() + segDist.value()), cost, Cost(currCost.value() + segCost.value()), time, Time(currTime.value() + segTime.value()), expedited, pathSoFar, pathString, segToAdd);
						outputPaths.insert(outputPaths.end(), newPaths.begin(), newPaths.end());
					}
				}
			}
		}
	}
	return outputPaths;
}

std::vector<std::string> Connectivity::connect(Location::Ptr startLocPtr, Location::Ptr destLocPtr,
	std::vector<Location::Ptr> path, std::string pathString,
	std::string segToAdd, Segment::ExpediteSupport expedited,
	Cost pathCost, Cost segCost,
	Time pathTime, Time segTime,
	Segment::ExpediteSupport allSegmentsExpedited) {
	std::vector<std::string> outputPaths;

	if (segToAdd.compare(" ") != 0) {
		pathCost = Cost(pathCost.value() + segCost.value());
		pathTime = Time(pathTime.value() + segTime.value());
		pathString.append(segToAdd);
	}
	path.push_back(startLocPtr);
	pathString.append(convertToString(startLocPtr));

	if (startLocPtr->name().compare(destLocPtr->name()) == 0) {
		std::vector<std::string> singleString;
		pathString = convertToString(pathCost, pathTime, expedited) + pathString;
		singleString.push_back(pathString);
		return singleString;

	}

	// Loop through all the segments from this start location and see if they lead to a connection
	for (int i = 1; i <= startLocPtr->segmentsSize(); i++) {

		Segment::Ptr seg = EngineManager::segmentMap(startLocPtr->segment(i));
		if (seg) {
			Location::Ptr nextLocPtr = EngineManager::locationMap((EngineManager::segmentMap(seg->returnSegmentName()))->source());
			if (nextLocPtr) {
				//Check if the nextLoc already exists in path

				std::vector<Location::Ptr>::iterator pathIter = find(path.begin(), path.end(), nextLocPtr);
				if (pathIter == path.end()) {
					//Check if the path is supposed to be expedited but this segment is not
					if ((seg->expediteSupport() == Segment::yes_) or (expedited == Segment::no_)) {

						std::string segString = convertToString(seg);
						Cost currSegCost = segmentCost(seg->mode(), seg->difficulty(), seg->length(), expedited);
						Time currSegTime = segmentTime(seg->mode(), seg->length(), expedited);
						Segment::ExpediteSupport expediteSupportSoFar = allSegmentsExpedited;
						if (seg->expediteSupport() == Segment::no_ || allSegmentsExpedited == Segment::no_) {
							expediteSupportSoFar = Segment::no_;
						}

						std::vector<std::string> returnedPaths = connect(nextLocPtr, destLocPtr, path, pathString, segString, expedited, pathCost, currSegCost, pathTime, currSegTime, expediteSupportSoFar);

						//check if returned array is empty?
						outputPaths.insert(outputPaths.end(), returnedPaths.begin(), returnedPaths.end());
					}
				}
			}
			else
				fprintf(stderr, "Error finding location pointer using return segment");
		}
		else
			fprintf(stderr, "Error finding segment for start location");
	}
	return outputPaths;
}

//~~added
/*
std::vector<std::string> EngineManager::findConnectingRoute(Location::Ptr startLocPtr, Location::Ptr destLocPtr,
											std::vector<Location::Ptr> path, std::string pathString,
											std::string segToAdd, Cost pathCost, Cost segCost, Cost lowestCost){
	std::vector<std::string> outputPaths;

	if(segToAdd.compare(" ") !=0 ){
			pathCost = Cost(pathCost.value() + segCost.value());
			pathTime = Time(pathTime.value() + segTime.value());
			pathString.append(segToAdd);
	}
	path.push_back(startLocPtr);
	pathString.append(convertToString(startLocPtr));

	if(startLocPtr->name().compare(destLocPtr->name()) == 0){
		std::vector<std::string> singleString;
		//pathString = convertToString(pathCost, pathTime, expedited) + pathString;
		singleString.push_back(pathString);
		return singleString;

	}

	// Loop through all the segments from this start location and see if they lead to a connection
	for(int i = 1; i <= startLocPtr->segmentsSize(); i++){

		Segment::Ptr seg = EngineManager::segmentMap(startLocPtr->segment(i));
		if(seg){
		Location::Ptr nextLocPtr = EngineManager::locationMap((EngineManager::segmentMap(seg->returnSegmentName()))->source());
		if(nextLocPtr){
		//Check if the nextLoc already exists in path

			std::vector<Location::Ptr>::iterator pathIter = find(path.begin(), path.end(), nextLocPtr);
			if(pathIter == path.end()){
				//Check if the path is supposed to be expedited but this segment is not


					std::string segString = convertToString(seg);
					Cost currSegCost = segmentCost(seg->mode(), seg->difficulty(), seg->length(), expedited);

					std::vector<std::string> returnedPaths = connect(nextLocPtr, destLocPtr, path, pathString, segString, pathCost, currSegCost);

					//check if returned array is empty?
					outputPaths.insert(outputPaths.end(), returnedPaths.begin(), returnedPaths.end());
			}
		}
		else
			fprintf(stderr, "Error finding location pointer using return segment");
		}
		else
			fprintf(stderr, "Error finding segment for start location");
	}
	return outputPaths;
}
*/

std::string Connectivity::convertToString(std::vector<std::string> strVect) {
	std::vector<std::string>::iterator iter;
	std::string resultPaths;
	unsigned int i = 0;
	if (strVect.size() == 0 || strVect.empty()) {
		return "";
	}

	for (i = 0; i < strVect.size() - 1; i++) {
		resultPaths.append(strVect.at(i));

		resultPaths.append("\n");
	}
	resultPaths.append(strVect.at(i));
	return resultPaths;
}


std::string Connectivity::convertToString(Cost cost, Time time, Segment::ExpediteSupport expedited) {
	std::string str;
	str.append(convertToString(cost.value()));
	str.append(" ");
	str.append(convertToString(time.value()));
	str.append(" ");
	if (expedited == Segment::yes_)
		str.append("yes");
	else
		str.append("no");
	str.append(";");
	return str;

}

Cost Connectivity::segmentCost(Segment::ModeType _mode, Difficulty _diff, Length _len, Segment::ExpediteSupport _exp) {
	double costPerMile = EngineManager::fleetInstance()->cost(EngineManager::modeTypeTable_[_mode]).value();
	double difficulty = _diff.value();
	double length = _len.value();
	double cost = costPerMile * difficulty*length;
	if (_exp == Segment::yes_)
		cost = 1.5*cost;
	return Cost(cost);
}


std::string Connectivity::convertToString(double val) {

	std::stringstream dblToStr;
	dblToStr << setprecision(2) << fixed;
	dblToStr << val;
	std::string dblStr = dblToStr.str();
	return dblStr;
}

std::string Connectivity::convertToString(Location::Ptr locPtr) {
	return locPtr->name();
}

std::string Connectivity::convertToString(Segment::Ptr segPtr) {
	std::string strSeg;
	strSeg.append("(");
	strSeg.append(segPtr->name());
	strSeg.append(":");
	std::ostringstream lengthToStr;
	lengthToStr << segPtr->length().value();
	std::string lengthStr = lengthToStr.str();
	strSeg.append(lengthStr);
	strSeg.append(":");
	strSeg.append(segPtr->returnSegmentName());
	strSeg.append(")");
	strSeg.append(" ");
	return strSeg;
}


//~~~~~~~Segment Implementation~~~~~~~~~~~~~~//

void EngineManager::onZeroReferences() {}

EngineManager::EngineManager() {
	//notifieeListSize_ =0;
	modeTypeTable_[Segment::boat_] = "Boat";
	modeTypeTable_[Segment::plane_] = "Plane";
	modeTypeTable_[Segment::truck_] = "Truck";
	log_ = Log::log();
}

void Segment::expediteSupportIs(Segment::ExpediteSupport _expediteSupport)
{
	Segment::ExpediteSupport prevExpediteSupport = this->expediteSupport_;
	expediteSupport_ = _expediteSupport;
	if (notifieeListSize() > 0) {
		NotifieeListIterator n = notifieeIterBegin();

		for (unsigned int i = 0; i < notifieeListSize(); i++) {
			try {
				(*n)->onExpediteSupportIs(_expediteSupport, prevExpediteSupport);
				++n;
			}
			catch (...) {}

		}
	}
}


SegmentReactor::Ptr SegmentReactor::SegmentReactorIs(Segment::ExpediteSupport exp, Segment::Ptr s) {

	SegmentReactor * sr = new SegmentReactor(s.ptr());
	SegmentReactor::Ptr srPtr = sr;
	//Initialize the expedite counts for all the segments that have already been created
	int totalSegments = EngineManager::segmentMapSize();
	if (totalSegments != 0) {
		if (exp == Segment::yes_) {
			expeditedSegments_++;
			expeditePercentage_ = (expeditedSegments_) / totalSegments * 100;
		}
	}
	return sr;
}

SegmentReactor::Ptr SegmentReactor::SegmentReactorIs(Segment::Ptr s) {

	SegmentReactor * sr = new SegmentReactor(s.ptr());
	SegmentReactor::Ptr srPtr = sr;
	return sr;
}

ShipmentSegmentReactor::Ptr ShipmentSegmentReactor::ShipmentSegmentReactorIs(Segment::Ptr s) {

	ShipmentSegmentReactor * sr = new ShipmentSegmentReactor(s.ptr());
	ShipmentSegmentReactor::Ptr srPtr = sr;

	return sr;
}
void SegmentReactor::onExpediteSupportIs(Segment::ExpediteSupport _newValue, Segment::ExpediteSupport _previousValue) {
	//check if expedite support has been added or removed and update the %

	double totalSegments = EngineManager::segmentMapSize();
	cout << "\n In on expedite support is!!!";
	if (_newValue == Segment::yes_ && _previousValue == Segment::no_) {
		expeditedSegments_++;
	}
	else if (_newValue == Segment::no_ && _previousValue == Segment::yes_) {
		expeditedSegments_--;
	}
	expeditePercentage_ = (expeditedSegments_) / totalSegments * 100;
}


EngineReactor::Ptr EngineReactor::EngineReactorIs(EngineManager* emPtr) {

	EngineReactor * er = new EngineReactor(emPtr);
	EngineReactor::Ptr erPtr = er;
	//Initialize the counts from engine manager
	EngineManager::LocationMap::iterator locIter = EngineManager::locationMapIter();
	for (int i = 0; i < EngineManager::locationMapSize(); i++) {
		Location::LocationType t = locIter->second->locationType();
		if (t == Location::customer_) {
			erPtr->customerLocationCount_++;
		}
		else if (t == Location::boatTerminal_) {
			erPtr->boatTerminalCount_++;
		}
		else if (t == Location::planeTerminal_) {
			erPtr->planeTerminalCount_++;
		}
		else if (t == Location::truckTerminal_) {
			erPtr->truckTerminalCount_++;
		}
		else if (t == Location::port_) {
			erPtr->portCount_++;
		}
		++locIter;
	}

	EngineManager::SegmentMap::iterator segmentIter = EngineManager::segmentMapIter();
	for (int i = 0; i < EngineManager::segmentMapSize(); i++) {
		Segment::ModeType t = segmentIter->second->mode();
		if (t == Segment::plane_) {

			erPtr->planeSegmentCount_++;
		}
		else if (t == Segment::truck_) {
			erPtr->truckSegmentCount_++;
		}
		else if (t == Segment::boat_) {
			erPtr->boatSegmentCount_++;
		}
		++segmentIter;
	}

	return er;
}

void EngineReactor::onSegmentNew(Segment::Ptr segPtr) {
	if (segPtr->mode() == Segment::plane_) {
		planeSegmentCount_++;
	}
	else if (segPtr->mode() == Segment::truck_) {
		truckSegmentCount_++;
	}
	else if (segPtr->mode() == Segment::boat_) {
		boatSegmentCount_++;
	}


}

void EngineReactor::onLocationNew(Location::Ptr locPtr) {

	if (locPtr->locationType() == Location::customer_) {
		customerLocationCount_++;
	}
	else if (locPtr->locationType() == Location::boatTerminal_) {
		boatTerminalCount_++;
	}
	else if (locPtr->locationType() == Location::planeTerminal_) {
		planeTerminalCount_++;
	}
	else if (locPtr->locationType() == Location::truckTerminal_) {
		truckTerminalCount_++;
	}
	else if (locPtr->locationType() == Location::port_) {
		portCount_++;
	}

}


void Segment::returnSegmentNameIs(std::string _returnSegmentName) {
	EngineManager::Ptr em = EngineManager::instance();
	Segment::Ptr rs;
	if (returnSegmentName_.size() > 0)  rs = em->segmentMap(returnSegmentName_);
	Segment::Ptr rsNew = em->segmentMap(_returnSegmentName);
	if (rs) {
		returnSegmentName_ = _returnSegmentName;
		if (rs->returnSegmentName().compare(name_) != 0)
			rs->returnSegmentNameIs(name_);

	}
	else if (rsNew && _returnSegmentName.compare("")) {
		if (rsNew->mode() == mode_) {
			if (rsNew->source().compare(source_) != 0)
			{
				returnSegmentName_ = _returnSegmentName;
				if (rsNew->returnSegmentName().compare(name_) != 0)
					rsNew->returnSegmentNameIs(name_);

			}
			else {
				//cerr<<"Not Allowed : segment pairs cannot have the same source "<<endl;
				EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<Segment>(this), "Segment::returnSegmentNameIs", "Not Allowed : segment pairs cannot have the same source ");
			}
		}
		else {
			//cerr<<"Not Allowed : segment pairs should have the same mode"<<endl;
			EngineManager::log()->entryNew(Log::Error, Fwk::Ptr<Segment>(this), "Segment::returnSegmentNameIs", "Not Allowed : segment pairs cannot have the same source ");
		}
	}
}

Segment::Segment(std::string _name, Segment::ModeType _mode) : difficulty_(1.0),
length_(0.0), capacity_(10),
currentShipmentsCount_(0.0),
shipmentsReceived_(0.0),
shipmentsRefused_(0.0) {

	name_ = _name;
	mode_ = _mode;
	returnSegmentName_ = "";
	expediteSupport_ = no_;
	source_ = "";
	countToDel = 0;

}

//~~added
void Segment::shipmentIs(Shipment::Ptr _shipment, Shipment::previouslyRefused prevRefused) {
	if (currentShipmentsCount_.value() < capacity_.value()) {
		shipments_.push_back(_shipment);
		//Update statistics
		shipmentsReceived_ = Count(shipmentsReceived_.value() + 1);
		if (prevRefused == Shipment::yes_) {
			shipmentsRefused_ = Count(shipmentsRefused_.value() + 1);
		}
		_shipment->nextLocationIs(EngineManager::segmentMap(this->returnSegmentName())->source());

		//Find out how many trips are necessary to transport this shipment along this segment
		Capacity segmentModeCapacity = EngineManager::fleetInstance()->capacity(EngineManager::modeTypeTable_[mode_]);
		int tripCount = ceil(double(_shipment->numberOfPackages().value()) / double(segmentModeCapacity.value()));
		//cout<<"\ntrip count =" << tripCount;
		// Calculate the time taken for each trip across this segment
		double speed = EngineManager::fleetInstance()->speed(EngineManager::modeTypeTable_[mode_]).value();
		Length len = this->length();
		double time = len.value() / speed;
		Time delay = Time(time*tripCount);
		cout << "\n Delay for segment " << name_ << " = " << delay.value();
		//cout<<"delay = "<<delay.value();
		//schedule activities to wait for the appropriate time, and then indicate that the shipment has been delivered
		_shipment->currSegmentIs(this->name());

		//update the cost of the shipment - add the cost of fwding along this segment
		Cost segCost = segmentCost();
		_shipment->shipmentCostInc(segCost);
		//cout<<" In segment "<< name_<<" .. creating fwd activity";
		double packagesDelivered = 0;
		//for(int i =0;  i<tripCount; i++){
		Activity::Manager::Ptr manager = activityManagerInstance();
		string fwdActName = "FwdActivity_";
		fwdActName.append(name_);
		std::stringstream ss;
		ss << countToDel;
		//fwdActName.append(ss.str());
		countToDel++;
		Activity::Ptr fwdActivity = manager->activity(fwdActName);

		if (fwdActivity == 0) {
			fwdActivity = manager->activityNew(fwdActName);
			//cout<<"\nsetting the fwd activity to execute at time ="<<delay.value()+manager->now().value();
			fwdShipmentReactor = new ForwardShipmentReactor(manager, fwdActivity.ptr(), delay.value());
			fwdActivity->lastNotifieeIs(fwdShipmentReactor);
			fwdActivity->nextTimeIs(Time(manager->now().value() + delay.value()));
			fwdActivity->statusIs(Activity::nextTimeScheduled);
		}

		//else{
		else if (fwdShipmentReactor->shipmentPendingSize() == 0) {
			//schedule the activity
			cout << " \n shipment que is empty .. setting next time = " << manager->now().value() + delay.value();
			fwdActivity->nextTimeIs(Time(manager->now().value() + delay.value()));
			fwdActivity->statusIs(Activity::nextTimeScheduled);
		}
		fwdShipmentReactor->shipmentPending(_shipment, Time(manager->now().value() + delay.value()));
		//}
		//if(i+1==tripCount)
			//_shipment->packagesDeliveredCountIs(_shipment->numberOfPackages());
		//else
			//_shipment->packagesDeliveredCountInc(Count(segmentModeCapacity.value()));
		//***************TO CHANGE - TEMP SET DELAY TO 1
		//fwdActivity->lastNotifieeIs(new ForwardShipmentReactor(manager, fwdActivity.ptr(), delay.value(), _shipment));


//	}

		currentShipmentsCount_ = Count(currentShipmentsCount_.value() + 1);
	}
	//If the segment is full, refuse shipment

}
//~~added
Cost Segment::segmentCost() {
	double costPerMile = EngineManager::fleetInstance()->cost(EngineManager::modeTypeTable_[mode_]).value();
	double difficulty = difficulty_.value();
	double length = length_.value();
	double cost = costPerMile * difficulty*length;
	return Cost(cost);
}

void Segment::sourceIs(std::string _source) {
	Location::Ptr l = EngineManager::locationMap(_source);

	if (l) {
		if ((l->locationType() == Location::boatTerminal_ && mode_ == Segment::boat_) ||
			(l->locationType() == Location::planeTerminal_ && mode_ == Segment::plane_) ||
			(l->locationType() == Location::truckTerminal_ && mode_ == Segment::truck_) ||
			l->locationType() == Location::customer_ || l->locationType() == Location::port_)
		{
			source_ = _source;
			Location::Ptr l1 = EngineManager::locationMap(this->source());
			if (l1)
				l1->removeSegment(this->name());

			l->addSegment(this->name());
		}
		else cerr << "Not Allowed : Location " << _source << " not compatible with Segment " << name_ << endl;
	}
	else
		cerr << "Location " << _source << " does not exist";
}
void Segment::currentShipmentsCountDec() {
	currentShipmentsCount_ = Count(currentShipmentsCount_.value() - 1);
	cout << " number of notifiees = " << notifieeListSize();

	// call only shipment notifiee instead!!
	if (notifieeSingle_) {
		notifieeSingle_->onCurrentShipmentsCountDec(source_, name_);
	}
	/*if(notifieeListSize()>0){
						NotifieeListIterator n=notifieeIterBegin();

						for(unsigned int i =0; i<notifieeListSize(); i++){
							try{
								cout<<"calling on decr shipments..";
							  (*n)->onCurrentShipmentsCountDec(source_, name_);
							  ++n;
							}catch(...) { }

					}
				}*/

}
BoatSegment::BoatSegment(std::string _name) :Segment(_name, Segment::boat_) {

}
PlaneSegment::PlaneSegment(std::string _name) : Segment(_name, Segment::plane_) {

}
TruckSegment::TruckSegment(std::string _name) : Segment(_name, Segment::truck_) {

}

//TO return time

Shipping::Time Connectivity::segmentTime(Segment::ModeType _mode, Length _len, Segment::ExpediteSupport _exp) {

	double speed = EngineManager::fleetInstance()->speed(EngineManager::modeTypeTable_[_mode]).value();
	if (_exp == Segment::yes_)
		speed = speed + (speed*0.3);

	double time = _len.value() / speed;

	return Time(time);
}

//~~added
void CustomerLocationReactor::onShipmentIs(Shipment::Ptr _shipment) {
	//calculate how often packages have to be sent.
	//create a new activity, custloc_act_reactor and set last notifiee is, timeIs etc.
	cout << " In On shipment is";

}

void CustomerLocation::activityDel() {

	Activity::Manager::Ptr manager = activityManagerInstance();
	cout << "Deleting activity .. " << name_;
	//manager->activityDel(name_);
	manager->activity(name_)->statusIs(Activity::deleted);
}

void CustomerLocationReactor::onTransferRateIs(Rate _transferRate) {
	cout << " In on transfer rate ";
	//If the transfer rate has been un-set, stop the simulation
	if (_transferRate == 0) {
		//locPtr->activityDel();
	}
}

Shipment::Shipment(string _name, string _source, string _destination, Count _numberOfPackages) : shipmentCost_(0.0), startTime_(0.0), numberOfPackages_(0), packagesDeliveredCount_(0) {
	name_ = _name;
	source_ = _source;
	destination_ = _destination;
	numberOfPackages_ = _numberOfPackages;
	currSegment_ = "";
	visitedNodes_.insert(source_);
	startTime_ = Time(activityManagerInstance()->now().value());
	//prevSegment_ = "";
	//startTime = now();

}
