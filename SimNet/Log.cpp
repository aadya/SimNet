#include "Log.h"
#include <stdio.h>
#include <time.h>
Log::Ptr Log::log_;
/*template <class T>
Log<T>::Log(){
	// Open a new log file

	 logFile_.open ("Log.txt");
	 logFile_<<"Starting Log..";

}*/
/*
template <class T>
void Log<T>::entryNew( Log<T>::Priority p,
		Fwk::Ptr<T> objectPtr,
		const std::string functionName,
		const std::string condition){

	time_t timeNow;
	struct tm * localTime;

	  time ( &timeNow );
	  localTime = localtime ( &timeNow );
	  std::string timeString = asctime (localTime);
	  printf ( "The current date/time is: %s",timeString);

	logFile_<<"\n"<<timeString <<" "<<functionName<<" "<<condition;
}*/

/*template <class T>
Log<T>::~Log(){
	logFile_.close();
}*/