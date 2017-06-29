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

static void sigTimerHandlerPriv(int sig, siginfo_t* si, void* uc)
{
	printfMessage("Timer Handler is %x\n",si->si_value.sival_ptr);
}

#if 0
void installEventTimer(event *pEvent,sTimer_t *timerID){
	struct sigevent sev;
	struct itimerspec its;
	long long freq_nanosecs;
	sigset_t mask;
	struct sigaction sa;
	
	printfMessage("Install Timer\n");
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = pEvent->EventCallbackFunc;
	sigemptyset(&sa.sa_mask);
	sigaction(SIG, &sa, NULL);
	
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIG;
	sev.sigev_value.sival_ptr = timerid;
	timer_create(CLOCKID, &sev, timerid);
	  
	/* Start the timer */
	its.it_value.tv_sec = (pEvent->timeOutValue)/MSECOND;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;
	
	timer_settime(timerID, 0, &its, NULL);		
}

void unInstallEventTimer(event *pEvent){
}
#endif

