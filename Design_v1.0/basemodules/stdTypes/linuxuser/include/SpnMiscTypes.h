#ifndef SPNMISCTYPES_H
#define SPNMISCTYPES_H
#include <time.h>
#include <signal.h>
#include "SpnTypes.h"
typedef clock_t spnClock;
typedef siginfo_t spnSigInfo;
typedef sUint32_t spnSimClock;
typedef enum{DBGMSG,DBGWAR,DBGERR} DEBUG_LEVEL;	
#endif
