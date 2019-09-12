#ifndef __ACTIVITY_IMPL_H__
#define __ACTIVITY_IMPL_H__

#include <map>
#include <string>
#include <queue>

#include "Activity.h"
#include "Queue.h"
#include "QueueReactor.h"
#include "RealTimeActivityReactor.h"
#include "Engine.h"

using namespace std;


Fwk::Ptr<Activity::Manager> activityManagerInstance();
Fwk::Ptr<Activity::RealTimeManager> realTimeActivityManagerInstance();
//class RealTimeActivityReactor;

namespace ActivityImpl {

	//Comparison class for activities   
	class ActivityComp : public binary_function<Activity::Ptr, Activity::Ptr, bool> {
	public:
		ActivityComp() {}

		bool operator()(Activity::Ptr a, Activity::Ptr b) const {
			return (a->nextTime() > b->nextTime());
		}
	};

	class ActivityImpl : public Activity {
	protected:
		ActivityImpl(const string& name, Fwk::Ptr<class ManagerImpl> manager)
			: Activity(name), status_(free), nextTime_(0.0), notifiee_(NULL), manager_(manager) {}

		ActivityImpl(const string& name, Fwk::Ptr<class RealTimeManagerImpl> manager)
			: Activity(name), status_(free), nextTime_(0.0), notifiee_(NULL), rtmanager_(manager) {}

		Fwk::Ptr<class ManagerImpl> manager() const { return manager_; }



		virtual Status status() const { return status_; }
		virtual void statusIs(Status s) {
			status_ = s;
			if (notifiee_ != NULL) {
				notifiee_->onStatus();
			}
		}

		virtual Time nextTime() const { return nextTime_; }
		virtual void nextTimeIs(Time t) {
			nextTime_ = t;
			if (notifiee_ != NULL) {
				notifiee_->onNextTime();
			}
		}

		virtual Notifiee::Ptr notifiee() const { return notifiee_; }

		virtual void lastNotifieeIs(Notifiee* n) {
			ActivityImpl* me = const_cast<ActivityImpl*>(this);
			me->notifiee_ = n;

		}


	private:
		friend class ManagerImpl;
		friend class RealTimeManagerImpl;
		Status status_;
		Time nextTime_;
		Notifiee* notifiee_;
		Fwk::Ptr<class ManagerImpl> manager_;
		Fwk::Ptr<class RealTimeManagerImpl> rtmanager_;
	};

	class RealTimeActivityReactor : public Activity::Notifiee {
	public:
		RealTimeActivityReactor(Fwk::Ptr<Activity::Manager> manager, Activity* activity)
			:Notifiee(activity), activity_(activity), manager_(manager) {}
		void onStatus();
	protected:
		Activity::Ptr activity_;
		Fwk::Ptr<Activity::Manager> manager_;
	};



	class RealTimeManagerImpl :public Activity::RealTimeManager {
	public:
		typedef Fwk::Ptr<RealTimeManagerImpl> Ptr;
		static Fwk::Ptr<Activity::RealTimeManager> realTimeActivityManagerInstance();
		Activity::Ptr activityNew(const string& name);
		Activity::Ptr activity() { return activity_; }
		void  activityIs(Activity::Ptr);
		void NowIs(Time time);
		Time Now() { return Now_; }
		//friend class ManagerImpl;
		class Notifiee : public Fwk::BaseNotifiee<RealTimeManagerImpl> {
		public:
			typedef Fwk::Ptr<Notifiee> Ptr;

			Notifiee() : Fwk::BaseNotifiee<RealTimeManagerImpl>() {}
			virtual void onNowIs() {}
		};
		void lastNotifieeIs(Notifiee* n);
	private:
		Time Now_;
		Activity::Ptr activity_;
		//ManagerImpl::Ptr vManager_;

	protected:
		RealTimeManagerImpl() :Now_(0) {

			activity_ = this->activityNew("clockMover");
			Activity::Manager::Ptr manager = activityManagerInstance();
			RealTimeActivityReactor* rt = new RealTimeActivityReactor(manager, activity().ptr());
			activity_->lastNotifieeIs(rt);
		}
		//singleton instance
		static Fwk::Ptr<Activity::RealTimeManager> realTimeActivityManagerInstance_;
	};



	class ManagerImpl : public Activity::Manager {

	public:
		typedef Fwk::Ptr<ManagerImpl> Ptr;
		//Notifiee class for Activities
		class Notifiee : public Fwk::BaseNotifiee<ManagerImpl> {
		public:
			typedef Fwk::Ptr<Notifiee> Ptr;

			Notifiee(ManagerImpl* managerImpl) : Fwk::BaseNotifiee<ManagerImpl>(managerImpl) {}
			virtual void onNowIs() {}
		};

		virtual Activity::Ptr activityNew(const string& name);
		virtual Activity::Ptr activity(const string& name) const;
		virtual void activityDel(const string& name);

		virtual Time now() const { return now_; }
		virtual void nowIs(Time time);

		static Fwk::Ptr<Activity::Manager> activityManagerInstance();
		virtual void lastActivityIs(Activity::Ptr activity);
		virtual void lastNotifieeIs(Notifiee* n);

		//specific to this example
		Queue::Ptr queue() const { return queue_; }

	private:
		Notifiee* notifiee_;
		//RealTimeManagerImpl::Ptr rtmImpl_;
	protected:
		ManagerImpl() : now_(0) {
			queue_ = new Queue();
			//rtmImpl_=new RealTimeManagerImpl();
			//notifiee_= new ManagerReactor();
		}

		//Data members
		priority_queue<Activity::Ptr, vector<Activity::Ptr>, ActivityComp> scheduledActivities_;
		map<string, Activity::Ptr> activities_; //pool of all activities
		Time now_;

		//specific to this example
		Queue::Ptr queue_;


		//singleton instance
		static Fwk::Ptr<Activity::Manager> activityInstance_;

	};


	class ActivityManagerReactor : public ManagerImpl::Notifiee {
	public:
		typedef Fwk::Ptr<ActivityManagerReactor const> PtrConst;
		typedef Fwk::Ptr<ActivityManagerReactor> Ptr;
		void onNowIs(Time _time) {
			Shipping::EngineManager::fleetInstance()->timeNowIs(_time);

		}
		static ActivityManagerReactor* ActivityManagerReactorIs(Activity::Manager * am) {
			ManagerImpl* managerImpl = static_cast<ManagerImpl*>(am);
			ActivityManagerReactor * amr = new ActivityManagerReactor(managerImpl);
			//ActivityManagerReactor::Ptr amPtr = am;
			//	return amPtr;
			return amr;
		}

	protected:

		ActivityManagerReactor(ManagerImpl * managerImpl) : ManagerImpl::Notifiee(managerImpl) { }


	};



}

#endif /* __ACTIVITY_IMPL_H__ */

