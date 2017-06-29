#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "DoubleLinkList.h"

/* Header file */
#define MAX_STRING_LENGTH 100

struct ChildNode{
	LST_D_ENTRY(ChildNode) connector;
    char string[MAX_STRING_LENGTH];		
};

struct HeadNode{
	LST_D_HEAD(DlinkListExample, ChildNode) connectorHead;
};

typedef struct ChildNode ChildNode;
typedef struct HeadNode HeadNode;

/* Test Source code */
int main(void)
{
	ChildNode child1; 
	ChildNode child2; 
	ChildNode *pChild;	
	HeadNode dlistExampleHead;

	/* Init */
	LST_D_INIT(&dlistExampleHead.connectorHead);

	strcpy(child1.string,"Child1");
	
	strcpy(child2.string,"Child2");

    /* Test 1: Add into the head the list 
            Expect sequence: child2 child1 .....
	*/
	LST_D_INSERT_HEAD(&dlistExampleHead.connectorHead, &child1, connector);
	
    LST_D_INSERT_HEAD(&dlistExampleHead.connectorHead, &child2, connector);

    /* Loop testing */
	for(pChild=LST_D_FIRST(&dlistExampleHead.connectorHead);pChild;pChild=LST_D_NEXT(pChild,connector)) {
		printfMessage("Char string is %s\n",pChild->string);
    }

    /* Remove list entries */
	LST_D_REMOVE(&dlistExampleHead.connectorHead, &child1, connector);

    LST_D_REMOVE(&dlistExampleHead.connectorHead, &child2, connector);
    assert(LST_D_EMPTY(&dlistExampleHead.connectorHead));
	/* Test 2:Add one element after another element 
           Expect sequence: child1 child2 .....
	*/
	LST_D_INSERT_HEAD(&dlistExampleHead.connectorHead, &child1, connector);

	LST_D_INSERT_AFTER(&dlistExampleHead.connectorHead,&child1,&child2,connector);
	
	 /* Loop testing */
	for(pChild=LST_D_FIRST(&dlistExampleHead.connectorHead);pChild;pChild=LST_D_NEXT(pChild,connector)) {
		printfMessage("Char string is %s\n",pChild->string);
    }
	    /* Remove list entries */
	LST_D_REMOVE(&dlistExampleHead.connectorHead, &child1, connector);

    LST_D_REMOVE(&dlistExampleHead.connectorHead, &child2, connector);

    assert(LST_D_EMPTY(&dlistExampleHead.connectorHead));

	/* Test 3: Add one element before another element
	    Expect sequence: child2 child1 .....
	*/
	LST_D_INSERT_HEAD(&dlistExampleHead.connectorHead, &child1, connector);
	
	LST_D_INSERT_BEFORE(&dlistExampleHead.connectorHead,&child1,&child2,connector);

    /* Loop testing */
	for(pChild=LST_D_FIRST(&dlistExampleHead.connectorHead);pChild;pChild=LST_D_NEXT(pChild,connector)) {
		printfMessage("Char string is %s\n",pChild->string);
    }
	    /* Remove list entries */
	LST_D_REMOVE(&dlistExampleHead.connectorHead, &child1, connector);

    LST_D_REMOVE(&dlistExampleHead.connectorHead, &child2, connector);	

}
