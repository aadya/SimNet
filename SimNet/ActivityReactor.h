#ifndef __ACTIVITY_REACTOR_H__
#define __ACTIVITY_REACTOR_H__

#include "ActivityImpl.h"

#include <list>

using namespace Shipping;


class ProducerActivityReactor : public Activity::Notifiee {
public:
	ProducerActivityReactor(Fwk::Ptr<Activity::Manager> manager, Activity*
		activity, double rate, CustomerLocation::Ptr loc)
		: Notifiee(activity), rate_(rate), activity_(activity), manager_(manager), loc_(loc) {}

	void onStatus();

protected:
	double rate_;
	Activity::Ptr activity_;
	Fwk::Ptr<Activity::Manager> manager_;
	CustomerLocation::Ptr loc_;

};

class InjectShipmentReactor : public Activity::Notifiee {
public:
	InjectShipmentReactor(Fwk::Ptr<Activity::Manager> manager, Activity*
		activity, double rate, CustomerLocation::Ptr loc)
		: Notifiee(activity), rate_(rate), activity_(activity), manager_(manager), loc_(loc) {}

	void onStatus();

protected:
	double rate_;
	Activity::Ptr activity_;
	Fwk::Ptr<Activity::Manager> manager_;
	CustomerLocation::Ptr loc_;

};

class ForwardShipmentReactor : public Activity::Notifiee {
public:
	ForwardShipmentReactor(Fwk::Ptr<Activity::Manager> manager, Activity*
		activity, double rate)
		: Notifiee(activity), rate_(rate), activity_(activity), manager_(manager) {}

	void shipmentPending(Shipment::Ptr _shipment, Time activityNextTime) {
		cout << " pushing back shipment to fwd at " << activityNextTime.value();
		shipments_.push_back(make_pair(_shipment, activityNextTime.value()));

	}
	int shipmentPendingSize() {
		return shipments_.size();
	}

	void onStatus();

protected:
	double rate_;
	Activity::Ptr activity_;
	Fwk::Ptr<Activity::Manager> manager_;
	//Shipment::Ptr shipment_;
	list<pair<Shipment::Ptr, double> > shipments_;
	pair<Shipment::Ptr, double> popShipment() {
		if (shipments_.empty()) return make_pair<Shipment::Ptr, double>(NULL, 0.0);
		pair<Shipment::Ptr, double> s = shipments_.front();
		shipments_.pop_front();
		return s;
	}
};


class ConsumerActivityReactor : public Activity::Notifiee {
public:
	ConsumerActivityReactor(Fwk::Ptr<Activity::Manager> manager, Activity*
		activity, double rate, Shipment shipment)
		: Notifiee(activity), rate_(rate), activity_(activity), manager_(manager), shipment_(shipment) {}

	void onStatus();

protected:
	double rate_;
	Activity::Ptr activity_;
	Fwk::Ptr<Activity::Manager> manager_;
	Shipment shipment_;

};

#endif
