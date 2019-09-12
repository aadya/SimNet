#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <iostream>
#include <deque>

#include "PtrInterface.h"
#include "Ptr.h"

#include "Nominal.h"
#include "Notifiee.h"

typedef Nominal<class QueueSize_, unsigned int> QueueSize;

/* ===========================================================================
 * A pretty typical Q which defines enQ and deQ, and a queue size.
 * The default Q size is 10
 * ==========================================================================*/
class Queue : public Fwk::PtrInterface<Queue> {
public:
	typedef Fwk::Ptr<Queue> Ptr;

	/* Begin Notifiee stuff for Queue */
	class Notifiee : public Fwk::BaseNotifiee<Queue> {
	public:
		typedef Fwk::Ptr<Notifiee> Ptr;

		Notifiee(Queue* queue) : Fwk::BaseNotifiee<Queue>(queue) {}

		virtual void onEnQ(int) {}
		virtual void onDeQ(int) {}
	};
	Fwk::Ptr<Notifiee> notifiee() { return notifiee_; }
	void lastNotifieeIs(Notifiee* n) {
		Queue* me = const_cast<Queue*>(this);
		me->notifiee_ = n;
	}
	/* End Notifiee stuff for Queue */

	Queue()
		: notifiee_(NULL), queueSize_(10) {}

	QueueSize queueSize() const { return queueSize_; }
	void queueSizeIs(QueueSize queueSize) { queueSize_ = queueSize; }

	int size() { return deq_.size(); }

	int deQ() {
		if (deq_.empty()) {
			return 0;
		}

		int value = deq_.front();
		deq_.pop_front();

		if (notifiee_ != NULL) {
			notifiee_->onDeQ(value);
		}
		return value;
	}

	void enQ(int v) {
		if (deq_.size() < queueSize().value()) {
			deq_.push_back(v);

			if (notifiee_ != NULL) {
				notifiee_->onEnQ(v);
			}
		}
		else {
			std::cout << "q full " << std::endl;
		}
	}

private:
	Notifiee* notifiee_;
	QueueSize queueSize_;
	std::deque<int> deq_;
};

#endif
