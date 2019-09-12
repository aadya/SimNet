#include <iostream>
#include <time.h>

#include "ActivityImpl.h"
#include "Activity.h"

Fwk::Ptr<Activity::Manager> activityManagerInstance() {
	return ActivityImpl::ManagerImpl::activityManagerInstance();
}

Fwk::Ptr<Activity::RealTimeManager> realTimeActivityManagerInstance() {
	return ActivityImpl::RealTimeManagerImpl::realTimeActivityManagerInstance();
}

namespace ActivityImpl {

	//Definition of static member
	Fwk::Ptr<Activity::Manager> ManagerImpl::activityInstance_ = NULL;

	//Gets the singleton instance of ManagerImpl
	Fwk::Ptr<Activity::Manager> ManagerImpl::activityManagerInstance() {

		if (activityInstance_ == NULL) {
			activityInstance_ = new ManagerImpl();

		}
		return activityInstance_;

	}

	//  Definition of static member
	Fwk::Ptr<Activity::RealTimeManager> RealTimeManagerImpl::realTimeActivityManagerInstance_ = NULL;

	//Gets the singleton instance of RealTimeManagerImpl
	Fwk::Ptr<Activity::RealTimeManager> RealTimeManagerImpl::realTimeActivityManagerInstance() {

		if (realTimeActivityManagerInstance_ == NULL) {
			realTimeActivityManagerInstance_ = new RealTimeManagerImpl();
		}

		return realTimeActivityManagerInstance_;
	}

	Activity::Ptr ManagerImpl::activityNew(const string& name) {
		Activity::Ptr activity = activities_[name];

		if (activity != NULL) {
			cerr << "Activity already exists!" << endl;
			return NULL;
		}

		activity = new ActivityImpl(name, this);
		activities_[name] = activity;

		return activity;
	}

	Activity::Ptr RealTimeManagerImpl::activityNew(const string& name) {


		activity_ = new ActivityImpl(name, this);
		return activity_;
	}

	Activity::Ptr ManagerImpl::activity(const string& name) const {
		map<string, Activity::Ptr>::const_iterator it = activities_.find(name);

		if (it != activities_.end()) {
			return (*it).second;
		}

		// dont throw an exception (accessor)
		return NULL;
	}

	void ManagerImpl::activityDel(const string& name) {
		activities_.erase(name);
	}

	void ManagerImpl::lastActivityIs(Activity::Ptr activity) {
		scheduledActivities_.push(activity);

	}

	void ManagerImpl::nowIs(Time t) {
		//Activity::RealTimeManager::Ptr rtm =realTimeActivityManagerInstance();
		if (notifiee_ != NULL) {
			notifiee_->onNowIs();
		}
		//find the most recent activites to run and run them in order
		while (!scheduledActivities_.empty()) {

			//figure out the next activity to run
			Activity::Ptr nextToRun = scheduledActivities_.top();

			//if the next time is greater than the specified time, break
			//the loop
			if (nextToRun->nextTime() > t) {
				break;
			}

			//calculate amount of time to sleep
			Time diff = Time(nextToRun->nextTime().value() - now_.value());
			//rtImpl()
			//sleep 100ms (100,000 microseconds) for every unit of time
			//usleep(( ((int)diff.value()) * 100000));

			now_ = nextToRun->nextTime();

			//print out size of queue
			cout << "size of queue: " << queue_->size() << endl;

			//run the minimum time activity and remove it from the queue
			scheduledActivities_.pop();

			nextToRun->statusIs(Activity::executing);
			nextToRun->statusIs(Activity::free);

		}

		//syncrhonize the time
		now_ = t;
	}

	void ManagerImpl::lastNotifieeIs(Notifiee* n) {
		ManagerImpl* me = const_cast<ManagerImpl*>(this);
		notifiee_ = n;
	}
	void RealTimeManagerImpl::NowIs(Time t) {
		//cout<<"Real Good night !"<<endl;
		//Activity::Ptr clockMover = activity();

		//sleep for the required time
		if (activity()->nextTime() > t) {
			activity()->statusIs(Activity::waiting);
			return;
		}
		activity()->statusIs(Activity::executing);
		activity()->statusIs(Activity::free);
		Now_ = activity()->nextTime().value() + 1;

	}
	void RealTimeManagerImpl::activityIs(Activity::Ptr activity) {
		activity_ = activity;

	}
	/*
	RealTimeManagerImpl::RealTimeManagerImpl():now_(0) {
				activity_= this->activityNew("clockMover");
				Activity::Manager::Ptr manager = activityManagerInstance();
				RealTimeActivityReactor* rt= new RealTimeActivityReactor(manager,activity().ptr());
				//activity_->lastNotifieeIs(new RealTimeActivityReactor(manager,activity_));
			}
*/
} //end namespace ActivityImpl
