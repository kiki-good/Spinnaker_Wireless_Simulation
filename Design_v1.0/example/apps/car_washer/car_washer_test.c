#include "Scheduler.h"
#include "Event.h"
#include "Object.h"
#include "timer.h"

#define INITAL_TIMEOUT 3
#define INITAL_SIMTIME 200

/* Test purpose:used to restrict/constrain random event time out value */
#define DIVIDE_FACTOR 3

/* Golable variable for the shared Queue */
HeadObject stHeadObject;

/* Golable variable for configuration */
spnSimClock simulationTotalTick;
timeBaseHandle tbHandle;

static void objCallbackFunc(struct object* pObject);

#ifdef SUPPORT_NO_OS
void c_main()
#else
void main()
#endif
{
	event_obj *event1, *event2;	
	sUint32_t event1_id,event2_id;
	object *object1,*object2;
	/* user maintains the list header since it is more flexible */
	objectSetting stObjectSetting;
	scheduleSetting stScheduleSetting;
	performanceOuput stPerformanceOuput;
	simulationSetting stSimSetting;
	simulationParameters stSimulationParameters;
	/* timebase setting */
	timebaseSetting stTmebaseSetting;
	sUint32_t timeBaseId;
	spnSimClock clock_config;

	/* simulation time */
	timeBaseId = 0;
	/* set simulation time unit */
	stTmebaseSetting.eTimeUnit = MILLSECOND;
	/* set simulation time value */
	stTmebaseSetting.simCLockTick = INITAL_SIMTIME;	
	stTmebaseSetting.eSimPrecision = MILLSECOND;
	tbHandle = createSimTimebase(timeBaseId,&stTmebaseSetting);	
	simulationTotalTick = tbHandle->totalSimulationPeriod;

	/* set simulation input parameters */
	stSimSetting.simulationTotalTick = tbHandle->totalSimulationPeriod;
	stSimSetting.tbHandle = tbHandle;
	stSimulationParameters.stSimulationSetting = stSimSetting;

	/* inital timeout setting for the event */	
	clock_config = INITAL_TIMEOUT;
	
	/* set setting for scheduler */
	stScheduleSetting.timeOutclockTick = clock_config;	
	stScheduleSetting.stSimulationSetting = stSimSetting;
	stScheduleSetting.tbHandle = tbHandle;
	
	/* A.init stage */	
	event1_id = 0;
    event1 = createEvent(event1_id);	
	
	printfMessageLevel(DBGMSG,"event1:Address %x\n",event1);	
	printfMessageLevel(DBGMSG,"event1:Id %d\n",event1->id);	

    /* A.1. use same event id to identify object */
	memsetWrapper(&stObjectSetting,0,sizeof(objectSetting));
	stObjectSetting.id = event1_id;
	stObjectSetting.pEvent = event1;
	object1 = createObj(&stObjectSetting);	
	object1->pObjActionCallbackFunc = objCallbackFunc; 
	object1->pSchedulerFunc = objSchedulerFunc;
	
	printfMessageLevel(DBGMSG,"object1:Address %x\n",object1);	
	printfMessageLevel(DBGMSG,"object1:Id %d\n",object1->stObjectSetting.id);	

	/* A.2. init link list header(must called before objectAttachEvent)*/
	objInitListHead(&stHeadObject);

	DumpObjElements(&stHeadObject);

	/* Configure random number seed before running the scheduler */
	sUint32_t randomLoop = 0;
	sUint32_t randomLoopMax = 30;
	for(randomLoop = 0;randomLoop < randomLoopMax;randomLoop++)
	{
		printfMessageLevel(DBGMSG,"random number test: [%d]\n",GetRandNumberRange(INITAL_SIMTIME));
	}

	/* initial trigger */
	object1->pSchedulerFunc(&stHeadObject,object1,&stScheduleSetting);

	memsetWrapper(&stPerformanceOuput,0,sizeof(performanceOuput));

	RunDiscreteEventSimulation(&stHeadObject,&stSimulationParameters);

	destroySimTimebase(tbHandle);

	printfMessageLevel(DBGWAR,"Test is finished\n");
	
	return;
}

/* callback function for each object */
static void objCallbackFunc(struct object* pObject)
{	
	static sUint32_t event_id_counter = 1;
	objectSetting stObjectSetting;	
	simulationSetting stSimSetting;	
	simulationParameters stSimulationParameters;	
	scheduleSetting stScheduleSetting;
	sUint32_t randomClockTick;
	event_obj *event_n;
	object *object_n;
	spnSimClock clock_config;

#ifdef SUPPORT_NO_OS
	printfMessageLevel(DBGWAR,"objCallbackFunc:current event %d is triggered\n",pObject->stObjectSetting.id);
#else
	printf("objCallbackFunc:current event %d is triggered\n",pObject->stObjectSetting.id);
#endif

	/* Auto (randomly) start to schedule the next event */
	/* 1.Create another object and event */	
    event_n = createEvent(event_id_counter);	
	memsetWrapper(&stObjectSetting,0,sizeof(objectSetting));
	stObjectSetting.id = event_id_counter;
	stObjectSetting.pEvent = event_n;
	object_n = createObj(&stObjectSetting);	
	object_n->pObjActionCallbackFunc = objCallbackFunc; 
	object_n->pSchedulerFunc = objSchedulerFunc;	

#ifdef SUPPORT_NO_OS
	printfMessageLevel(DBGWAR,"objCallbackFunc:nexut event_id_counter is %d\n",event_id_counter);
#else
	printf("objCallbackFunc:nexut event_id_counter is %d\n",event_id_counter);
#endif

	event_id_counter++;

	/* 2.Run the Scheduler */

	/* Set random parameters */	
#ifdef SUPPORT_NO_OS
	printfMessageLevel(DBGWAR,"simulationTotalTick %d\n",simulationTotalTick);
#else
	printf("simulationTotalTick %d\n",simulationTotalTick);
#endif
	randomClockTick = GetRandNumberRange(simulationTotalTick)/DIVIDE_FACTOR;

#ifdef SUPPORT_NO_OS
	printfMessageLevel(DBGWAR,"objCallbackFunc:randowConfig %d\n",randomClockTick);
#else
	printf("objCallbackFunc:randowConfig %d\n",randomClockTick);
#endif
	/* set setting for scheduler */
	stSimSetting.simulationTotalTick = simulationTotalTick;
	stSimSetting.tbHandle = tbHandle;
	stScheduleSetting.timeOutclockTick = randomClockTick;	
	stScheduleSetting.stSimulationSetting = stSimSetting;
	stScheduleSetting.tbHandle = tbHandle;

	object_n->pSchedulerFunc(&stHeadObject,object_n,&stScheduleSetting);
}
