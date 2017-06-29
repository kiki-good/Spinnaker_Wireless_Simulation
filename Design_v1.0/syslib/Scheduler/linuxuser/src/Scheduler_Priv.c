#include "Scheduler.h"
#include "Scheduler_Priv.h"
#include "timer.h"

void RunDiscreteEventSimulationPriv(HeadObject* pHeadObject,simulationParameters* pSimulationPara)
{	
	object *pObject;
	event_obj *pEvent;
	spnSimClock currentClock;
	
	DumpObjElements(pHeadObject);
		
	do{				
		/* 1. Check if current list is empty */
		for(pObject=LST_Q_FIRST(&pHeadObject->connectorHead);pObject;pObject=LST_Q_NEXT(pObject,connector))
		{			
			/* 2. Check callback associated with event */
			pEvent = pObject->stObjectSetting.pEvent;
			
			if(pEvent->timeOutClockTick > pSimulationPara->stSimulationSetting.simulationTotalTick)
			{
/*
				printfMessageLevel(DBGWAR,"Do not schedule this event %d,cause:Run longer than total simulation time\n",pEvent->id);
*/
				printf("Do not schedule this event %d,cause:Run longer than total simulation time\n",pEvent->id);
				
				/* remove it from Queue otherwise it will fall into this condition endlessly */
				
				objDeque(pHeadObject,pObject);

				DumpObjElements(pHeadObject);

				/* free both event and object */
				freeWrapper(pObject->stObjectSetting.pEvent);
					
				freeWrapper(pObject);	
				
				break;
			}

/*			printfMessageLevel(DBGWAR,"RunDiscreteEventSimulationLoop:GetSystemClock() %d,pEvent->timeOutClockTick %d,Total simulationTick %d\n",
							 GetSimTimebaseTick(pSimulationPara->stSimulationSetting.tbHandle),pEvent->timeOutClockTick,
							 pSimulationPara->stSimulationSetting.simulationTotalTick);
*/			
			printf("GetSystemClock()%d\n",GetSimTimebaseTick(pSimulationPara->stSimulationSetting.tbHandle));
			printf("pEvent->timeOutClockTick %d\n",pEvent->timeOutClockTick);			
			printf("pSimulationPara->stSimulationSetting.simulationTotalTick %d\n",pSimulationPara->stSimulationSetting.simulationTotalTick);
			
			/* fire the callback if the current time is longer than the timeout time of each event */
			if((currentClock =  GetSimTimebaseTick(pSimulationPara->stSimulationSetting.tbHandle)) >= pEvent->timeOutClockTick)
			{			
				if(pObject->pObjActionCallbackFunc)
				{										
					pObject->pObjActionCallbackFunc((object *)pObject);

					/*3. if it has already been fired then remove this event from the queue */					
					objDeque(pHeadObject,pObject);
					
					DumpObjElements(pHeadObject);

					/* free both event and object */
					freeWrapper(pObject->stObjectSetting.pEvent);
					
					freeWrapper(pObject);					

					break;
				}
			}
		}
		
		incSimTimebaseTick(pSimulationPara->stSimulationSetting.tbHandle);

		if(GetSimTimebaseTick(pSimulationPara->stSimulationSetting.tbHandle) >= pSimulationPara->stSimulationSetting.simulationTotalTick)
		{
			break;
		}		
	}while(1);
}

