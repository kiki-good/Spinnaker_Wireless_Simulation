#ifndef DOUBLE_LINK_LIST_H
#define DOUBLE_LINK_LIST_H
#include "stdWrapperApi.h"
/*
Example:
     struct block {
        LST_D_ENTRY(block) link;
        char string[256];
     };

     LST_D_HEAD(blocklist_type, block) list;
     LST_D_INIT(&list);
*/

#define LST_D_HEAD(name, type) struct name { struct type *l_first; }

/*
Example:
     struct block {
        LST_D_ENTRY(block) link;
        char string[256];
     };
*/

#define LST_D_ENTRY(type)  struct { struct type *l_next, *l_prev; const void *l_head; }

/*
LST_D_INIT(&head);
*/

#define LST_D_INIT(head) ((head)->l_first=NULL)


/*
static struct block_head  head = LST_D_INITIALIZER(head);
*/

#define LST_D_INITIALIZER(head) {NULL}

/*
    if (LST_D_EMPTY(&head) { return ; }

*/

#define LST_D_EMPTY(head) ((head)->l_first == NULL)

/*
struct block *first=LST_D_FIRST(&head);
*/

#define LST_D_FIRST(head) ((head)->l_first)

/*
struct block *second=LST_D_NEXT(first);
*/
#define LST_D_NEXT(elm, field) ((elm)->field.l_next)

/*
struct block *first=LST_D_PREV(second);
*/

#define LST_D_PREV(elm, field) ((elm)->field.l_prev)

/*
	LST_D_INSERT_HEAD(&head, new_block, link);
*/

#define LST_D_INSERT_HEAD(head, new_elm, field) do { \
    (new_elm)->field.l_head = (const void *)head; \
    if ( ((new_elm)->field.l_next = (head)->l_first) != NULL ) (head)->l_first->field.l_prev = (new_elm); \
    (head)->l_first = (new_elm); (new_elm)->field.l_prev = NULL; \
    } while(0)

/*
 	LST_D_INSERT_AFTER(&head, first, second, link);
*/

#define LST_D_INSERT_AFTER(head, elm, new_elm, field) do { \
	assertAction((elm)->field.l_head == (const void *)head);\
    (new_elm)->field.l_head = (const void *)head; \
    (new_elm)->field.l_prev = (elm); \
    if (((new_elm)->field.l_next = (elm)->field.l_next)!=NULL)  (elm)->field.l_next->field.l_prev = new_elm; \
    (elm)->field.l_next = (new_elm); \
    } while(0)

/*
 	LST_D_INSERT_BEFORE(&head, second, first, link);
*/

#define LST_D_INSERT_BEFORE(head, elm, new_elm, field) do { \
	assertAction((elm)->field.l_head == (const void *)head);\
	(new_elm)->field.l_head = (const void *)head; \
	(new_elm)->field.l_next = (elm); \
	if (((new_elm)->field.l_prev = (elm)->field.l_prev)!=NULL) \
		(elm)->field.l_prev->field.l_next = new_elm; \
	else \
		(head)->l_first = (new_elm); \
    (elm)->field.l_prev = (new_elm); \
    } while(0)

/*
	LST_D_REMOVE(&head, first, link);
*/

#define LST_D_REMOVE(head, elm, field) do { \
	assertAction((elm)->field.l_head == (const void *)head); \
    (elm)->field.l_head = NULL; \
    if ((elm)->field.l_next) (elm)->field.l_next->field.l_prev = (elm)->field.l_prev;  \
    if ((elm)->field.l_prev) (elm)->field.l_prev->field.l_next = (elm)->field.l_next; else (head)->l_first = (elm)->field.l_next; \
    } while(0)

/*
	LST_D_REMOVE_HEAD(&head, link);
*/

#define LST_D_REMOVE_HEAD(head, field) do { \
    assertAction((head)->l_first); \
    assertAction((head)->l_first->field.l_head == (const void *)head); \
    (head)->l_first->field.l_head = NULL; \
    (head)->l_first = (head)->l_first->field.l_next; \
    if ((head)->l_first) { (head)->l_first->field.l_prev = NULL;} \
    } while(0)
#endif
