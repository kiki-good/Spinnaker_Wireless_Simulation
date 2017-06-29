#ifndef TIMER_H
#define TIMER_H

#include "SpnMiscTypes.h"

#define CLOCK_TO_SEC 1000000

typedef enum{
	NANOSECOND = 1, /* 10e-9s */	
	USSECOND = 1000, /* 10e-6s */	
	MILLSECOND = 1000000, /* 10e-3s */
	SECOND = 1000000000
}TIMEUNIT;

typedef struct timebaseSetting{
	spnSimClock simCLockTick;
	TIMEUNIT eSimPrecision;
	TIMEUNIT eTimeUnit;
}timebaseSetting;

typedef struct stTimeBase{
	sUint32_t id;
	spnSimClock internalClockTick;
	spnSimClock totalSimulationPeriod;
	timebaseSetting stTimebaseSetting;
}stTimeBase;

typedef stTimeBase* timeBaseHandle;

extern void sleepWrapper(sUint32_t seconds);
extern spnClock GetSystemClock();
extern void delayWrapper(sUint32_t seconds);
extern void getTimeWrapper(sUint32_t *uSecond);
extern void highResolutionSleepWrapper(sUint32_t seconds,sUint32_t naoSeconds);
extern timeBaseHandle createSimTimebase(sUint32_t id,timebaseSetting *pTimebaseSetting);
extern void destroySimTimebase(timeBaseHandle tbHandle);
extern void getSimTimebaseSetting(timeBaseHandle tbHandle,timebaseSetting *pTimebaseSetting);
extern void setSimTimbaseSetting(timeBaseHandle tbHandle,timebaseSetting *pTimebaseSetting);
extern void incSimTimebaseTick(timeBaseHandle tbHandle);
extern spnSimClock GetSimTimebaseTick(timeBaseHandle tbHandle);
#endif
