#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TMAX 300
#define TransmissionRate 0.5
#define RecoveryRate 0.2
#define MAX_VERTICES 10000
#define MAX_EDGES 3000

/* Next Node is the node that would be dequeued after the current node , previous node is the one to be dequeued before current node*/
typedef struct _PriorityNode
{
    int dayOfEvent; // functions as priority decider
    int typeOf; // 0 for transmission and 1 for recovery
    // memory of node needed in graph
    struct _PriorityNode* next;//pointer to next element in Event Queue
    struct _PriorityNode* previous;//pointer to previous element in Event Queue
}eventNode;

eventNode* Enqueue(eventNode* queueHead, int day , int type);
eventNode* Dequeue(eventNode* queueHead);
eventNode* goToHead(eventNode* node);
eventNode* goToTail(eventNode* node);
void PrintQueueAscending(eventNode* head);
void PrintQueueDescending(eventNode* head);

int main(void)
{
    eventNode* Head=NULL;
    Head=Enqueue(Head,0,0);
    Head=Enqueue(Head,3,0);
    Head=Enqueue(Head,1,0);
    Head=Enqueue(Head,4,0);
    Head=Enqueue(Head,2,0);
    PrintQueueAscending(Head);
    PrintQueueDescending(Head);
    Head=Dequeue(Head);
    PrintQueueAscending(Head);
    Head=Dequeue(Head);
    PrintQueueAscending(Head);
}
eventNode* Enqueue(eventNode* queueHead, int day , int type)
{
    if(queueHead!=NULL)
    {
        while(day>=queueHead->dayOfEvent&&queueHead->next!=NULL)// go to 1 step behind position
        {
            queueHead=queueHead->next;
        }
        if(queueHead->dayOfEvent>day)// reach desired node
        {
            queueHead=queueHead->previous;
        }
    }
    eventNode* newNode = NULL;
    newNode= malloc(sizeof(eventNode));
    newNode->dayOfEvent=day;
    newNode->typeOf=type;
    if(queueHead!=NULL)// change links on queueHead and node "previous" to queueHead
    {
        newNode->next=queueHead->next;
        newNode->previous=queueHead;
        if(queueHead->next!=NULL)
        {
            queueHead->next->previous=newNode;
        }
        queueHead->next=newNode;
    }
    else
    {
        newNode->previous=NULL;
        newNode->next=NULL;
    }
    queueHead=goToHead(newNode);
    return queueHead;
}
eventNode* Dequeue(eventNode* queueHead)
{
    eventNode* newHead=queueHead->next;
    free(queueHead);
    if(newHead!=NULL)
    {
        newHead->previous=NULL;
    }
    return newHead;
}
eventNode* goToHead(eventNode* node)
{
    if(node!=NULL)
    {
        while(node->previous!=NULL)
        {
            node=node->previous;
        }
    }
    return node;
}
eventNode* goToTail(eventNode* node)
{
    if(node!=NULL)
    {
        while(node->next!=NULL)
        {
            node=node->next;
        }
    }
    return node;
}
void PrintQueueAscending(eventNode* head)
{
    while(head!=NULL)
    {
        printf("%d ",head->dayOfEvent);
        head=head->next;
    }
    printf("\n");
}
void PrintQueueDescending(eventNode* head)
{
    head=goToTail(head);
    while(head!=NULL)
    {
        printf("%d ",head->dayOfEvent);
        head=head->previous;
    }
    printf("\n");
}