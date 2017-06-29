#ifndef SCHEDULER_PRIV_H
#define SCHEDULER_PRIV_H

#include "Object.h"
#include "Event.h"

#define MSECOND 1000

/* Configure Scheduling Policy */
#define CLOCKID CLOCK_REALTIME

extern void RunDiscreteEventSimulationPriv(HeadObject* pHeadObject,simulationParameters* pSimulationPara);
#endif
