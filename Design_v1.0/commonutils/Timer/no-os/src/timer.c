#include <time.h>
#include <stdio.h>
#include "stdWrapperApi.h"
#include "SpnMiscTypes.h"
#include "SpnTypes.h"
#include "timer.h"

/* inclue Spinnaker library */
#include "spinnaker.h"
#include "spin1_api.h"
#include "spin1_api_params.h"
#include "sark.h"

spnClock GetSystemClock()
{
	/* TO DO */
	/* return clock();*/
	return 0;
}

void sleepWrapper(sUint32_t seconds)
{
	/* TO DO */
	/* sleep(seconds); */
}

void highResolutionSleepWrapper(sUint32_t seconds,sUint32_t naoSeconds)
{
	/* TO DO */
	/*
	struct timespec ts;

	ts.tv_sec = seconds;
	ts.tv_nsec = naoSeconds;
	
	nanosleep(&ts, NULL);
	*/
}

void getTimeWrapper(sUint32_t *uSecond)
{
	/* TO DO */
	/*
	struct timeval start;

	gettimeofday(&start, NULL);
	*uSecond = start.tv_sec * 1000000 + start.tv_usec; 
	*/
}

void delayWrapper(sUint32_t seconds)
{
	spin1_delay_us(seconds*CLOCK_TO_SEC);
}

/* ================ APIs below for Simulation Clock =============================== */
timeBaseHandle createSimTimebase(sUint32_t id,timebaseSetting *pTimebaseSetting)
{
	timeBaseHandle tbHandle = NULL;

	if((pTimebaseSetting->eTimeUnit)/(pTimebaseSetting->eSimPrecision))
	{
		tbHandle = (timeBaseHandle)mallocWrapper(sizeof(stTimeBase));
		if(!tbHandle)
		{
			assertAction(0);
		}
		
		memsetWrapper(tbHandle,0,sizeof(stTimeBase));
		
		tbHandle->id = id;
		
		tbHandle->totalSimulationPeriod = ((pTimebaseSetting->simCLockTick)*(pTimebaseSetting->eTimeUnit/pTimebaseSetting->eSimPrecision));

		printfMessageLevel(DBGWAR,"totalSimulationPeriod is %d,divide result is %d,pTimebaseSetting->simCLockTick is %d\n",
							tbHandle->totalSimulationPeriod,
							(pTimebaseSetting->eTimeUnit/pTimebaseSetting->eSimPrecision),
							pTimebaseSetting->simCLockTick);
			
		/* remove after memsetWrapper is implemented in no-os */
		tbHandle->internalClockTick = 0;
					
		memcpyWrapper(&tbHandle->stTimebaseSetting,pTimebaseSetting,sizeof(timebaseSetting));
		
		return tbHandle;
	}
	else
	{
		printfMessageLevel(DBGERR,"Incorrect parameters!\n");
		return tbHandle;
	}	
}

void destroySimTimebase(timeBaseHandle tbHandle)
{
	freeWrapper(tbHandle);
}

void incSimTimebaseTick(timeBaseHandle tbHandle)
{
	tbHandle->internalClockTick++;
}

spnSimClock GetSimTimebaseTick(timeBaseHandle tbHandle)
{
	return tbHandle->internalClockTick;
}

void getTimebaseSetting(timeBaseHandle tbHandle,timebaseSetting *pTimebaseSetting)
{
	memcpyWrapper(pTimebaseSetting,&tbHandle->stTimebaseSetting,sizeof(timebaseSetting));
}

void setTimbaseSetting(timeBaseHandle tbHandle,timebaseSetting *pTimebaseSetting)
{	
	memcpyWrapper(&tbHandle->stTimebaseSetting,pTimebaseSetting,sizeof(timebaseSetting));
}

