#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "SpnTypes.h"
#include "SpnMiscTypes.h"
#include "SpnDebug.h"

void printfMessage(sChar* fmt, ...)
{
	va_list args;
	
	va_start(args,fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
}

void printfMessageLevel(DEBUG_LEVEL level,sChar* fmt, ...)
{
	if(level >= DEBUG_LEVEL_CONFIG)
	{
		printfMessage(fmt);
	}
}

void assertAction(sUint8_t value)
{
	assert(value);
}

void* mallocWrapper(sUint32_t size)
{
	void* rc;
	rc = malloc(size);
	if(!rc)
	{
		printfMessage("Memory allocation failed\n");
	}

	return rc;
}

sUint8_t freeWrapper(void* ptr)
{
	sUint8_t rc = 1;

	if(ptr == NULL)
	{		
		printfMessage("Trying to free NULL memmory\n");
		return rc;
	}
	
	free(ptr);

	return 0;
}

void memsetWrapper(void* ptr,sUint32_t value,sUint32_t nBytes)
{
    memset(ptr,value,nBytes);
}

void* memcpyWrapper(void* dest,void* src,sUint32_t size)
{
	return memcpy(dest,src,size);
}

void SetRandNumberSeed(sUint32_t seedNumber)
{
	srand(seedNumber);
}

sUint32_t GetRandNumber()
{
	return rand();
}

sUint32_t GetRandNumberRange(sUint32_t n) 
{	

#if 0
	sUint32_t r;	
	sUint32_t end = RAND_MAX / n;
	
 	if ((n - 1) == RAND_MAX) 
	{
    	return rand();
  	} 
	else 
	{
	    assert (end > 0L);
	    end *= n;
	    while ((r = rand()) >= end);

	    return r % n;
  	}
#else
	sUint32_t num;
	srand(time(NULL));
	num = rand();
	num = num % n;

	return num;
#endif
}

