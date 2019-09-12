#include "ActivityReactor.h"

static int num = 0;

void ProducerActivityReactor::onStatus() {
	Queue::Ptr q = NULL;
	ActivityImpl::ManagerImpl::Ptr managerImpl = Fwk::ptr_cast<ActivityImpl::ManagerImpl>(manager_);
	switch (activity_->status()) {

	case Activity::executing:
		//I am executing now
		q = managerImpl->queue();
		//might be better to hook up the reactor in ActivityImpl ??
		q->lastNotifieeIs(new QueueReactor(q.ptr()));
		std::cout << activity_->name() << " enqueueing number " << num << endl;
		q->enQ(num);
		num++;
		break;

	case Activity::free:
		//when done, automatically enqueue myself for next execution
		activity_->nextTimeIs(Time(activity_->nextTime().value() + rate_));
		activity_->statusIs(Activity::nextTimeScheduled);
		break;

	case Activity::nextTimeScheduled:
		//add myself to be scheduled
		manager_->lastActivityIs(activity_);
		break;

	default:
		break;
	}

}

void ConsumerActivityReactor::onStatus() {
	Queue::Ptr q = NULL;
	int n = 0;
	ActivityImpl::ManagerImpl::Ptr managerImpl = Fwk::ptr_cast<ActivityImpl::ManagerImpl>(manager_);

	switch (activity_->status()) {
	case Activity::executing:
		//I am executing now
		q = managerImpl->queue();
		n = q->deQ();
		cout << activity_->name() << " dequeing number " << n << endl;
		break;

	case Activity::free:
		//When done, automatically enqueue myself for next execution
		activity_->nextTimeIs(Time(activity_->nextTime().value() + rate_));
		activity_->statusIs(Activity::nextTimeScheduled);
		break;

	case Activity::nextTimeScheduled:
		//add myself to be scheduled
		manager_->lastActivityIs(activity_);
		break;

	default:
		break;
	}

}

void InjectShipmentReactor::onStatus() {
	Queue::Ptr q = NULL;
	int n = 0;
	ActivityImpl::ManagerImpl::Ptr managerImpl = Fwk::ptr_cast<ActivityImpl::ManagerImpl>(manager_);

	switch (activity_->status()) {
	case Activity::executing:
		//I am executing now
		cout << "\n*******Time = " << managerImpl->now().value() << "\n";
		q = managerImpl->queue();
		n = q->deQ();
		cout << activity_->name() << " dequeing number " << n << endl;
		loc_->shipmentNew();
		break;

	case Activity::free:
		//When done, automatically enqueue myself for next execution
			//cout<<"setting next time is to "
		activity_->nextTimeIs(Time(activity_->nextTime().value() + rate_));
		activity_->statusIs(Activity::nextTimeScheduled);
		break;

	case Activity::nextTimeScheduled:
		//add myself to be scheduled
		manager_->lastActivityIs(activity_);
		break;

	default:cout << " \n reactor doing nothing..";
		break;
	}

}

void ForwardShipmentReactor::onStatus() {
	Queue::Ptr q = NULL;
	int n = 0;
	ActivityImpl::ManagerImpl::Ptr managerImpl = Fwk::ptr_cast<ActivityImpl::ManagerImpl>(manager_);
	Shipment::Ptr shipment;
	pair<Shipment::Ptr, double> shipmentPair;
	// Time nextTime = Time(0.0);
	switch (activity_->status()) {
	case Activity::executing:
		//I am executing now
		cout << "\n*******Time = " << managerImpl->now().value() << "\n";
		//	q = managerImpl->queue();
		//	n = q->deQ();
			//cout << activity_->name() << " dequeing number " << n << endl;
		//	/cout<<" Executing fwd activity .. next loc = "<< shipment_->nextLocation();

		shipmentPair = popShipment();
		shipment = shipmentPair.first;
		if (shipment != NULL)
			EngineManager::locationMap(shipment->nextLocation())->shipmentIs(shipment);
		break;

	case Activity::free:
		//When done, automatically enqueue myself for next execution
		if (!shipments_.empty()) {
			shipmentPair = shipments_.front();
			//nextTime = shipmentPair.second;
			cout << "\n rescheduling the fwd activity to execute at " << shipmentPair.second;
			activity_->nextTimeIs(Time(shipmentPair.second));
			activity_->statusIs(Activity::nextTimeScheduled);
		}
		break;

	case Activity::nextTimeScheduled:
		//add myself to be scheduled
		manager_->lastActivityIs(activity_);
		break;

	default: //cout<<" \n reactor doing nothing..";
		break;
	}

}


