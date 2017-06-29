#include "Scheduler.h"
#include "Scheduler_Priv.h"
#include "timer.h"

void objSchedulerFunc(HeadObject *pHeadObject,struct object* pObject,void* pScheduleSetting)
{
    event_obj* pEvent;

	scheduleSetting* pScheduleSettingLocal = (scheduleSetting*)pScheduleSetting;

	/* Check the parameter is legal */
	if(!pHeadObject || !pObject || (!(pEvent = pObject->stObjectSetting.pEvent))||
		(!pScheduleSettingLocal))
	{
		printfMessage("ERROR:Parameters are invalid\n");
		assertAction(0);		
	}

	/* Check the parameter is valid */
	if(pScheduleSettingLocal->timeOutclockTick > pScheduleSettingLocal->stSimulationSetting.simulationTotalTick)
	{
		printfMessageLevel(DBGERR,"ERROR:Unable to Scheddule the Event:Cause 0x0\n");
		printfMessageLevel(DBGERR,"ERROR:timeOutclockTick %d\n",pScheduleSettingLocal->timeOutclockTick);		
		printfMessageLevel(DBGERR,"ERROR:simulationTotalTick %d\n",pScheduleSettingLocal->stSimulationSetting.simulationTotalTick);
		return;
	}
			
	/* set timeout clock tick for the object's event 
	    note: must add current system time for the final timeout clock tick 
	*/
	pObject->stObjectSetting.pEvent->timeOutClockTick = GetSimTimebaseTick(pScheduleSettingLocal->tbHandle) + pScheduleSettingLocal->timeOutclockTick;

	printfMessageLevel(DBGWAR,"objSchedulerFunc:object %d triggered\n",pObject->stObjectSetting.id);
	printfMessageLevel(DBGWAR,"objSchedulerFunc:GetSimTimebaseTick() is %d\n",GetSimTimebaseTick(pScheduleSettingLocal->tbHandle));
	printfMessageLevel(DBGWAR,"objSchedulerFunc:timeOutClockTick %d\n",pObject->stObjectSetting.pEvent->timeOutClockTick);

	objEnque(pHeadObject,pObject);
}

void RunDiscreteEventSimulation(HeadObject* pHeadObject,simulationParameters *pSimulationPara)
{
	RunDiscreteEventSimulationPriv(pHeadObject,pSimulationPara);
}
