#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "DoubleLinkList.h"
#include "Queue.h"
#include "Object.h"
#include "timer.h"

typedef struct simulationSetting{
	spnSimClock simulationTotalTick;	
	timeBaseHandle tbHandle;
}simulationSetting;

typedef enum schedulePolicy{
	FIFO
} schedulePolicy;

typedef struct scheduleSetting{
	schedulePolicy ePolicy;
	/* Define the average scheduling granularity(used in FIFO mode) */
	sUint32_t timeSlice;
	/* Time out value */
	spnSimClock timeOutclockTick; 	
	simulationSetting stSimulationSetting;
	timeBaseHandle tbHandle;
}scheduleSetting;

typedef struct performanceOuput{
	spnSimClock totalWaitCLockTick;
}performanceOuput;

typedef struct simulationParameters{
	simulationSetting stSimulationSetting;
	performanceOuput stPerformanceOuput;
}simulationParameters;

extern void objSchedulerFunc(HeadObject *pHeadObject,struct object* pObject,void* pScheduleSetting);
extern void RunDiscreteEventSimulation(HeadObject* pHeadObject,simulationParameters *pSimulationPara);


#endif
