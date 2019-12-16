#ifndef _TimeManager_H
#define _TimeManager_H

#include <chrono>
#include <thread>

class TimeManager {
public:

	static __declspec(dllexport) TimeManager& Instance() {
		static TimeManager instance;

		return instance;
	}

	double CalculateFrameRate(bool writeToConsole);

	double GetRunningTime();
	double GetTime();

	void Sleep(int milliseconds);

	double DeltaTime = 0;
	double StartTime = 0;

	double CurrentTime = 0;

private:

	TimeManager() {
		StartTime = GetTime();
	}
	;
	TimeManager(TimeManager const&);
	TimeManager& operator=(TimeManager const&);
};

#endif
