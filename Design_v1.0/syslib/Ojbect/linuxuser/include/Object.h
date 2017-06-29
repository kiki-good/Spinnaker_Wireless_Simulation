#ifndef OBJECT_H
#define OBJECT_H
#include "DoubleLinkList.h"
#include "Queue.h"
#include "Event.h"

typedef struct HeadObject{
	LST_Q_HEAD(ObjQueue, object) connectorHead;
}HeadObject;

typedef struct objectSetting{
	event_obj* pEvent;	
	sUint32_t id;
}objectSetting;

typedef struct object{
	LST_Q_ENTRY(object) connector;
	objectSetting stObjectSetting;	
	void(*pObjActionCallbackFunc)(struct object* pObject);
	void(*pSchedulerFunc)(HeadObject *pHeadObject,struct object* pObject,void* pScheduleSetting);
}object;

/* obj create/destory */
extern object* createObj(objectSetting *pObjectSetting);
extern sUint8_t freeObj(object* pObject);

/* obj attach/detach */
extern void objAttachEvent(object* pObject,event_obj* pEvent);
extern void objDetachEvent(object* pObject);

/* obj list operation */
extern void objInitListHead(HeadObject* pHeadObject);
extern void objEnque(HeadObject* pHeadObject,object* pObject);
extern void objDeque(HeadObject* pHeadObject,object* pObject);

/* obj debug */
extern void DumpObjElements(HeadObject* pHeadObject);
#endif
