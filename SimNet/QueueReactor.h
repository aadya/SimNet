#ifndef __QUEUE_REACTOR_H__
#define __QUEUE_REACTOR_H__

#include "Queue.h"

class QueueReactor : public Queue::Notifiee {
public:
	QueueReactor(Queue* queue)
		: Notifiee(queue) {}

	virtual void onEnQ(int value);
	virtual void onDeQ(int value);

};


#endif
