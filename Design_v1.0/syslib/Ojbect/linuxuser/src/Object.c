#include "Object.h"

object* createObj(objectSetting *pObjectSetting)
{
	object* pObject;
	
	pObject = (object*)mallocWrapper(sizeof(object));
	if(pObject == NULL)
		return NULL;
	
	memsetWrapper(pObject,0,sizeof(object));

	memcpyWrapper(&pObject->stObjectSetting,pObjectSetting,sizeof(objectSetting));

	return pObject;
}

sUint8_t freeObj(object* pObject)
{
	sUint8_t rc = 1;
	  
	if(freeWrapper(pObject) != 0)
	{
		return rc;
	}
	else
	{
		return 0;
	}   
}

void objAttachEvent(object* pObject,event_obj* pEvent)
{
	pObject->stObjectSetting.pEvent = pEvent;
}

void objDetachEvent(object* pObject)
{
	pObject->stObjectSetting.pEvent = NULL;
}

void objInitListHead(HeadObject* pHeadObject)
{	
	LST_Q_INIT(&pHeadObject->connectorHead);
}

void objEnque(HeadObject* pHeadObject,object* pObject)
{	
	LST_Q_INSERT_TAIL(&pHeadObject->connectorHead, pObject, connector);
}

void objDeque(HeadObject* pHeadObject,object* pObject)
{	
	LST_Q_REMOVE(&pHeadObject->connectorHead, pObject, connector);
}

void DumpObjElements(HeadObject* pHeadObject)
{
	object *pObject;

	if(LST_Q_EMPTY(&pHeadObject->connectorHead))
	{
		printfMessageLevel(DBGWAR,"DumpObjElements:Empty Queue\n");
		return;
	}
	
	for(pObject=LST_Q_FIRST(&pHeadObject->connectorHead);pObject;pObject=LST_Q_NEXT(pObject,connector)) {
/*
		printfMessageLevel(DBGWAR,"DumpObjElements:Current Object ID %d\n",pObject->stObjectSetting.id);
*/
		printf("DumpObjElements:Current Object ID %d\n",pObject->stObjectSetting.id);
	}
}
