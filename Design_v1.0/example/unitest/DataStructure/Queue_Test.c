#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Queue.h"

/* Header file */
#define MAX_STRING_LENGTH 100

struct ChildNodeQ{
	LST_Q_ENTRY(ChildNodeQ) connector;
    char string[MAX_STRING_LENGTH];		
};

struct HeadNodeQ{
	LST_Q_HEAD(QueueExample, ChildNodeQ) connectorHead;
};

typedef struct ChildNodeQ ChildNode;
typedef struct HeadNodeQ HeadNode;

/* Test Source code */
int main(void)
{
	ChildNode child1; 
	ChildNode child2; 
	ChildNode *pChild;	
	HeadNode dlistExampleHead;

	/* Init */
	LST_Q_INIT(&dlistExampleHead.connectorHead);

	strcpy(child1.string,"Child1");
	
	strcpy(child2.string,"Child2");

    /* Test 1: Add into the head the list 
            Expect sequence: child2 child1 .....
	*/
	LST_Q_INSERT_HEAD(&dlistExampleHead.connectorHead, &child1, connector);
	
    LST_Q_INSERT_HEAD(&dlistExampleHead.connectorHead, &child2, connector);

    /* Loop testing */
	for(pChild=LST_Q_FIRST(&dlistExampleHead.connectorHead);pChild;pChild=LST_Q_NEXT(pChild,connector)) {
		printfMessage("Char string is %s\n",pChild->string);
    }

    /* Remove list entries */
	LST_Q_REMOVE(&dlistExampleHead.connectorHead, &child1, connector);

    LST_Q_REMOVE(&dlistExampleHead.connectorHead, &child2, connector);
	
    assert(LST_Q_EMPTY(&dlistExampleHead.connectorHead));
	
	/* Test 2:Add one element after another element 
           Expect sequence: child1 child2 .....
	*/
	LST_Q_INSERT_HEAD(&dlistExampleHead.connectorHead, &child1, connector);

	LST_Q_INSERT_AFTER(&dlistExampleHead.connectorHead,&child1,&child2,connector);
	
	 /* Loop testing */
	for(pChild=LST_Q_FIRST(&dlistExampleHead.connectorHead);pChild;pChild=LST_Q_NEXT(pChild,connector)) {
		printfMessage("Char string is %s\n",pChild->string);
    }
	    /* Remove list entries */
	LST_Q_REMOVE(&dlistExampleHead.connectorHead, &child1, connector);

    LST_Q_REMOVE(&dlistExampleHead.connectorHead, &child2, connector);

    assert(LST_Q_EMPTY(&dlistExampleHead.connectorHead));

	/* Test 3: Add one element before another element
	    Expect sequence: child2 child1 .....
	*/
	LST_Q_INSERT_HEAD(&dlistExampleHead.connectorHead, &child1, connector);
	
	LST_Q_INSERT_BEFORE(&dlistExampleHead.connectorHead,&child1,&child2,connector);

    /* Loop testing */
	for(pChild=LST_Q_FIRST(&dlistExampleHead.connectorHead);pChild;pChild=LST_Q_NEXT(pChild,connector)) {
		printfMessage("Char string is %s\n",pChild->string);
    }
	    /* Remove list entries */
	LST_Q_REMOVE(&dlistExampleHead.connectorHead, &child1, connector);

    LST_Q_REMOVE(&dlistExampleHead.connectorHead, &child2, connector);	
	
    assert(LST_Q_EMPTY(&dlistExampleHead.connectorHead));

	/* Test 4: Add ChildNode 1 after Head,then Add ChildNode2 after ChildNode1(Add to Tail Test)
	*/
	LST_Q_INSERT_TAIL(&dlistExampleHead.connectorHead,&child1,connector);	
	
	LST_Q_INSERT_TAIL(&dlistExampleHead.connectorHead,&child2,connector);

	 /* Loop testing */
	for(pChild=LST_Q_FIRST(&dlistExampleHead.connectorHead);pChild;pChild=LST_Q_NEXT(pChild,connector)) {
		printfMessage("Char string is %s\n",pChild->string);
    }

}

