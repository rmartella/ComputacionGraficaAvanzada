#ifndef _TimeManager_H
#define _TimeManager_H

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

#include <chrono>
#include <thread>

class DLL_PUBLIC TimeManager {
public:

	static TimeManager& Instance() {
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
