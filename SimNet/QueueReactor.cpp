#include <iostream>

#include "QueueReactor.h"

using std::cout;
using std::endl;

void QueueReactor::onEnQ(int value) {
	cout << "Notification of enqueuing value: " << value << endl;
	//nothing to do. the activity is just waiting
}

void QueueReactor::onDeQ(int value) {
	cout << "Notification of dequeuing value: " << value << endl;
	//implement activity execution based on its type
	 //producer activity - Shipment loaded
	//consumer activity- Shipment unloaded
	// inject activity- assign source destination to a shipment-
	   // what is the customer loction reactor doing then ? or this just starts activity injection
	// forward activity- update source n detination of a shipment
}
