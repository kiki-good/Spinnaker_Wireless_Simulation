#ifndef EVENT_H
#define EVENT_H

#include "DoubleLinkList.h"
#include "Queue.h"

typedef struct event_obj{
    /* we might support more events in the furture so sets type to uint32 */
	sUint32_t id;
	/*(sUint8_t)(*EventCallbackFunc)(sUint8_t id,void* pArg);	*/	
	/* expire in milisecond */
	sUint32_t timeOutValue;
    spnSimClock timeOutClockTick;
}event_obj;

typedef struct timer{
}timer;

extern event_obj* createEvent(sUint32_t id);
extern sUint8_t deleteEvent(event_obj* pEvent);

#endif
