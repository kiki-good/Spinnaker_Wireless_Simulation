#include "Event.h"

event_obj* createEvent(sUint32_t id)
{
	event_obj* pEvent;

	pEvent = (event_obj*)mallocWrapper(sizeof(event_obj));
	if(pEvent == NULL)
		return NULL;
	
	memsetWrapper(pEvent,0,sizeof(event_obj));

	pEvent->id = id;

	return pEvent;
}

sUint8_t deleteEvent(event_obj* pEvent)
{
    sUint8_t rc = 1;
	
	if(freeWrapper(pEvent) != 0)
	{
		return rc;
	}
	else
	{
		return 0;
	}	
}

