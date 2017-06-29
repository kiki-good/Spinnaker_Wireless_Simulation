#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "SpnTypes.h"
#include "SpnMiscTypes.h"
#include "SpnDebug.h"

/* inclue Spinnaker library */
#include "spinnaker.h"
#include "spin1_api.h"
#include "spin1_api_params.h"
#include "sark.h"

void printfMessage(sChar* fmt,...)
{
	io_printf(IO_BUF,fmt);
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
	/* TO DO */
	/*assert(value);*/
}

void* mallocWrapper(sUint32_t size)
{
	void* rc;
	rc = spin1_malloc(size);
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
	
	spin1_free(ptr);

	return 0;
}

void memsetWrapper(void* ptr,sUint32_t value,sUint32_t nBytes)
{
	/* TO DO */
    /*memset(ptr,value,nBytes);*/
}

void* memcpyWrapper(void* dest,void* src,sUint32_t size)
{
	/* In Spinnaker implementation, it returns NULL */
	spin1_memcpy(dest,src,size);
	return NULL;
}

void SetRandNumberSeed(sUint32_t seedNumber)
{
	spin1_srand(seedNumber);
}

sUint32_t GetRandNumber()
{
	return spin1_rand();
}

sUint32_t GetRandNumberRange(sUint32_t n) 
{	

#if 1
	sUint32_t r;	
	sUint32_t end = RAND_MAX / n;
	
 	if ((n - 1) == RAND_MAX) 
	{
    	return spin1_rand();
  	} 
	else 
	{
	    assertAction (end > 0L);
	    end *= n;
	    while ((r = spin1_rand()) >= end);

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

