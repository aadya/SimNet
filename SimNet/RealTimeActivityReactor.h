#ifndef __REAL_TIME_ACTIVITY_REACTOR_H__
#define __REAL_TIME_ACTIVITY_REACTOR_H__

#include "ActivityImpl.h"
#include "Engine.h"

namespace ActivityImpl {
	/*
	class ManagerImpl;

	class RealTimeActivityReactor: public Activity::Notifiee {
	public:
		RealTimeActivityReactor(Fwk::Ptr<Activity::Manager> manager,Activity* activity)
			:Notifiee(activity), activity_(activity), manager_(manager){}
		void onStatus();
	protected:
	  Activity::Ptr activity_;
	  Fwk::Ptr<Activity::Manager> manager_;
	  double rate_;
	};

	void RealTimeActivityReactor::onStatus(){
		ActivityImpl::ManagerImpl::Ptr managerImpl = Fwk::ptr_cast<ActivityImpl::ManagerImpl>(manager_);
		switch (activity_->status()) {
			case Activity::executing:
			// sleep for 100 the time difference between to two activities
				 usleep(100000);
				 cout<<" RT exec !"<<endl;
				 //Activity::Manager::Ptr manager = activityManagerInstance();
				 //scaling factor- 1 sec of real time is 1 hr of virtual time
				 managerImpl->nowIs(activity_->nextTime().value()+1);
				 //manager->vManager()->nowIs(activity_->nextTime().value()+1)
			break;

			case Activity::free:
				cout<<" RT free !"<<endl;
			//When done, automatically set myself for next execution till timepassedIs
			activity_->nextTimeIs(Time(activity_->nextTime().value() + 1));
			activity_->statusIs(Activity::nextTimeScheduled);
			break;

			case Activity::nextTimeScheduled:
				cout<<" RT next time !"<<endl;
			//executes itself till virtual time set by now is of virtual manager
				activity_->statusIs(Activity::executing);
			break;

			default:
			break;
			}

	}
	*/
}
#endif
