#include "ActivityImpl.h"
#include <time.h>
class RealTimeManager;

namespace ActivityImpl {

	void RealTimeActivityReactor::onStatus() {
		ManagerImpl::Ptr managerImpl = Fwk::ptr_cast<ManagerImpl>(manager_);
		managerImpl->now();

		Activity::RealTimeManager::Ptr realactivityManager = realTimeActivityManagerInstance();
		//time t= RealTimeManagerImpl::Now();
		//realactivityManager->
		// I need to know real time here
		//RealTimeManager::Ptr realactivityManager= Fwk::ptr_cast<ManagerImpl>(manager_);
		switch (activity_->status()) {
		case Activity::executing:
			// sleep for 1000000 ms for 1 hr of virtual time
				 //usleep(1000000); //tempCommentAA
				 //cout<<" RT exec !"<<endl;
				 //Activity::Manager::Ptr manager = activityManagerInstance();
				 //scaling factor- 1 sec of real time is 1 hr of virtual time
				 //managerImpl->nowIs();
				 //manager->vManager()->nowIs(activity_->nextTime().value()+1)
			break;

		case Activity::free:
			cout << " RT free !" << endl;
			//When done, automatically set myself for next execution till timepassedIs
			activity_->nextTimeIs(Time(activity_->nextTime().value() + 1));
			activity_->statusIs(Activity::nextTimeScheduled);
			break;

		case Activity::nextTimeScheduled:
			cout << " RT next time !" << endl;
			//executes itself till virtual time set by now is of virtual manager
			activity_->statusIs(Activity::executing);
			break;

		default:
			break;
		}

	}
}
