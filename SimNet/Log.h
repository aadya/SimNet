#ifndef LOG_H_
#define LOG_H_
#include <iostream>
#include <fstream>
#include <time.h>
#include "PtrInterface.h"
#include "Ptr.h"
#include <string>
using std::cout;

class Log : public Fwk::PtrInterface<Log>
{
protected:
	//fstream logFile_;
	//Log::Ptr log_;
public:

	typedef Fwk::Ptr<Log const> PtrConst;
	typedef Fwk::Ptr<Log > Ptr;
	enum Priority { Null, Critical, Error, Warning, Status, Debug };

	template <class T>
	void entryNew(Priority p,
		Fwk::Ptr<T> objectPtr,
		const char *functionName,
		const char *condition)
	{

		time_t timeNow;
		struct tm * localTime;

		time(&timeNow);
		localTime = localtime(&timeNow);
		std::string timeString = asctime(localTime);
		// cout<<"In log entry new.. ";
		logFile_ << "\n" << timeString << ": " << functionName << " " << condition;
	}

	static Fwk::Ptr<Log > log() {
		if (!log_) {
			Log* l = new Log();
			log_ = l;
			return l;
		}
		else
			return log_;
	}

protected:
	std::fstream logFile_;
	static Log::Ptr log_;
	Log() {
		logFile_.open("Log.txt");
		logFile_ << "Starting Log..";
	}
	~Log() {
		logFile_.close();
	}
};
#endif /* LOG_H_ */
