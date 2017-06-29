#ifndef QUEUE_H
#define QUEUE_H
#include "stdWrapperApi.h"

#define LST_Q_P_SET_HEAD(node, head, field) do { (node)->field.l_head = (const void *)(head); }while(0)
#define LST_Q_P_TEST_HEAD(node, head, field) do { assertAction((node)->field.l_head == (const void *)(head)); }while(0)
#define LST_Q_P_HEAD(head) head

#define LST_Q_HEAD(name, type) struct name { struct type *q_first, *q_last; }

#define LST_Q_ENTRY(type)  struct { struct type *q_next, *q_prev; LST_Q_P_HEAD(const void *l_head;) }

#define LST_Q_INIT(head) ((head)->q_last = (head)->q_first=NULL)

#define LST_Q_INITIALIZER(head) {NULL, NULL}

#define LST_Q_EMPTY(head) ((head)->q_first == NULL)

#define LST_Q_FIRST(head) ((head)->q_first)

#define LST_Q_LAST(head) ((head)->q_last)

#define LST_Q_NEXT(elm, field) ((elm)->field.q_next)

#define LST_Q_PREV(elm, field) ((elm)->field.q_prev)

#define LST_Q_INSERT_HEAD(head, new_elm, field) do { \
    if ( (head)->q_first != NULL ) (head)->q_first->field.q_prev = (new_elm); \
    if ( (head)->q_last == NULL) (head)->q_last = (new_elm); \
    (new_elm)->field.q_next = (head)->q_first; LST_Q_P_SET_HEAD(new_elm,head,field); \
    (head)->q_first = (new_elm); (new_elm)->field.q_prev = NULL;}  while(0)

#define LST_Q_INSERT_TAIL(head, new_elm, field) do { \
    if ((head)->q_last) {                         \
        (head)->q_last->field.q_next = (new_elm); \
    } else {                                      \
        (head)->q_first = (new_elm);              \
    }                                             \
    LST_Q_P_SET_HEAD(new_elm,head,field);        \
    (new_elm)->field.q_next = NULL;               \
    (new_elm)->field.q_prev = (head)->q_last;     \
    (head)->q_last = (new_elm);                   \
   } while(0)	

#define LST_Q_INSERT_AFTER(head, elm, new_elm, field) do { \
    LST_Q_P_TEST_HEAD(elm,head,field); \
    if ((elm)->field.q_next != NULL)  (elm)->field.q_next->field.q_prev = (new_elm); \
    if ((elm)->field.q_next == NULL)  (head)->q_last = (new_elm);\
    LST_Q_P_SET_HEAD(new_elm,head,field); (new_elm)->field.q_next = (elm)->field.q_next;\
    (new_elm)->field.q_prev = (elm); (elm)->field.q_next = (new_elm); } while(0)	

#define LST_Q_INSERT_BEFORE(head, elm, new_elm, field) do { \
    LST_Q_P_TEST_HEAD(elm,head,field); \
    LST_Q_P_SET_HEAD(new_elm,head,field); \
    (new_elm)->field.q_next = (elm); \
    if (((new_elm)->field.q_prev = (elm)->field.q_prev)!=NULL) (elm)->field.q_prev->field.q_next = new_elm; \
    else (head)->q_first = (new_elm); \
    (elm)->field.q_prev = (new_elm); } while(0)	

#define LST_Q_REMOVE(head, elm, field) do { \
    LST_Q_P_TEST_HEAD(elm,head,field); \
    if ((elm)->field.q_next) (elm)->field.q_next->field.q_prev = (elm)->field.q_prev;  else (head)->q_last = (elm)->field.q_prev; \
    if ((elm)->field.q_prev) (elm)->field.q_prev->field.q_next = (elm)->field.q_next; else (head)->q_first = (elm)->field.q_next; \
    } while(0)	

#define LST_Q_REMOVE_HEAD(head, field) do { \
	assertAction((head)->q_first); \
	LST_Q_P_TEST_HEAD((head)->q_first,head,field); \
	(head)->q_first = (head)->q_first->field.q_next; \
	if ((head)->q_first) { (head)->q_first->field.q_prev = NULL;} else (head)->q_last=NULL; \
	} while(0)
	
#endif
