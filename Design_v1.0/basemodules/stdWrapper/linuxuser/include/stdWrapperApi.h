#ifndef STDWRAPPERAPI_H
#define STDWRAPPERAPI_H
#include "SpnMiscTypes.h"
#include "SpnTypes.h"

extern void printfMessageLevel(DEBUG_LEVEL level,sChar* fmt, ...);
extern void printfMessage(sChar* fmt, ...);
extern void assertAction(sUint8_t value);
extern void* mallocWrapper(sUint32_t size);
extern sUint8_t freeWrapper(void* ptr);
extern void memsetWrapper(void* ptr,sUint32_t value,sUint32_t nBytes);
extern void* memcpyWrapper(void* dest,void* src,sUint32_t size);
extern void SetRandNumberSeed(sUint32_t seedNumber);
extern sUint32_t GetRandNumber();
extern sUint32_t GetRandNumberRange(sUint32_t n);

#endif
