#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TMAX 300
#define TransmissionRate 5
// 50 percent transmission rate-> very high practically every one infected in few days in dense network
#define RecoveryRate 2
// 20 percent recovery rate -> takes time
#define MAX_VERTICES 1000
#define MAX_EDGES 300

// Graph Nodes
typedef struct graphNode
{
    int uniqueID;// numbers the nodes
    struct graphNode* next;
}node;// models a human in the model

typedef struct nodeHead
{
    node* head;// saves address for all nodes to traverse
    int size;// to control max edges 
    int state;//0 for susceptible , -1 for infected and 1 for recovered
}nodeHead;

typedef struct graph
{
    int numberOfVertexes;// size of that array
    nodeHead* arrayOfHeads;// array to store the heads
}network;

/* Next Node is the node that would be dequeued after the current node , previous node is the one to be dequeued before current node*/
typedef struct _PriorityNode
{
    int dayOfEvent; // functions as priority decider
    int typeOf; // 0 for transmission and 1 for recovery
    int nodeValue;// memory of node needed in graph
    struct _PriorityNode* next;//pointer to next element in Event Queue
    struct _PriorityNode* previous;//pointer to previous element in Event Queue
}eventNode;
typedef struct _system
{
    int dayOfEvent; // functions as priority decider
    int susceptible; // number of susceptible
    int infected;// number of infected
    int recovered;// number of recovered
    struct _system* next;//pointer to next element in Event Queue
    struct _system* previous;//pointer to previous element in Event Queue
}dataNode;

// functions for graph
node* insertVertex(int number);// inserts node with given state
void placeEdge(network* G,int u ,int v);// places edge between vertexes u and v
void printGraph(network* graphHead);
network* generateNetwork(int numberofVertexes);// generates random undirected graph network
void fillNetwork(network*graph ,int numberOfVertices,int maxNumberOfEdges);// populates the graph randomly
int isDuplicate(node* u,int value);// checks if source node has already the target node as neighbour
// functions for queues

/* Day of transmission 
Type of event 0 for transmission 1 for recovery 
Nodevalue concerned(array index)*/
eventNode* Enqueue(eventNode* queueHead, int day , int type,int originNode);// add event to event queue
eventNode* Dequeue(eventNode* queueHead);// remove element at head in event queue
eventNode* goToHead(eventNode* head);// go to head element in event queue
eventNode* goToTail(eventNode* head);// go to tail element in event queue
int FindEvent(eventNode* queueHead, int node,int event,int givenTime);// find if earlier element of a particular type exists
void PrintQueueAscending(eventNode* head);// prints events in queue in ascending manner ( parameter to be printed can be changed)
void PrintQueueDescending(eventNode* head);// prints events in queue in descending manner ( parameter to be printed can be changed)

void processTransmission(network*G,int nodeVal, int currentTime,eventNode* head,dataNode*data);// function to change status of node to infected
void processRecovery(network*G,int nodeVal,dataNode*data, int currentTime);// function to change status of node to recovered
void findTransmission(eventNode* queueHead, int currentTime,network*G,int target,int source);

dataNode* appendValue(dataNode*head, int time,int sus, int inf , int rec);// add new data node to data queue
dataNode* goToDataHead(dataNode* head);// go to head element in data queue
dataNode* goToDataTail(dataNode* head);// go to tail element in data queue
void PrintStatuses(dataNode*head);// print entire queue in terminal
void PrintToCSV(dataNode*head);// save data into csv file to be used to make graph

int main(void)
{
    eventNode* Head=NULL;
    int maxEdge = MAX_EDGES;
    int maxVer = MAX_VERTICES;
    /*number of nodes in a graph*/
    srand ( time(NULL) );
    int numberOfVertices = rand() % maxVer;
    /*number of maximum edges a vertex can have*/
    int maxNumberOfEdges = rand() % maxEdge;
    if(maxNumberOfEdges>((numberOfVertices*(numberOfVertices-1))/2))
    {
        maxNumberOfEdges = ((numberOfVertices*(numberOfVertices-1))/2);
    }// upper limit making sure every node is not connected to every node
    int initialNumber=numberOfVertices/100;// 1 percent population randomly infected
    printf("\nVertex%d , Edge%d , initial number: %d\n",numberOfVertices,maxNumberOfEdges, initialNumber);
    network* epidemic = generateNetwork(numberOfVertices);// generate space for network
    fillNetwork(epidemic,numberOfVertices,maxNumberOfEdges);// fill network randomly
    //printGraph(epidemic); // uncomment to see the graph created
    dataNode* data=NULL;// create data queue
    //Reusing priority queue to store data in queue -> can modify print function for different output
    data=appendValue(data,0,numberOfVertices,0,0);// initaalize the system
    for(int i = 0;i<initialNumber;i++)
    {
        int x = rand()%initialNumber;
        Head=Enqueue(Head,0,0,x);
    }// fills initial infections randomly to 1 percent of population
    while(Head)// while queue is non empty
    {
        int eventType = Head->typeOf;// type of event 0 for transmission and 1 for recovery
        if(eventType==0)// if transmission
        {
            if(epidemic->arrayOfHeads[Head->nodeValue].state==0)// and node is susceptible
            {
                processTransmission(epidemic,Head->nodeValue,Head->dayOfEvent,Head,data);// process the transmission
            }
        }
        else
        {
            processRecovery(epidemic,Head->nodeValue,data,Head->dayOfEvent);// process the recovery
        }
        Head=Dequeue(Head);// remove the processed highest priority element
    }
    PrintStatuses(data);// print data to terminal
    PrintToCSV(data);// save it to file
    printf("\nVertex%d , Edge%d , initial number: %d\n",numberOfVertices,maxNumberOfEdges, initialNumber);
    // printed once again so that no need to scroll to top to see the parameters
}
void processTransmission(network*G,int nodeVal, int currentTime,eventNode* head,dataNode*data)// function to change status of node to infected
{
    if(G->arrayOfHeads[nodeVal].state==0)// if node was susceptible
    {
        G->arrayOfHeads[nodeVal].state=-1;// node infected
    }
    else
    {
        return;// else return without executing other code
    }
    // update the data queue with new parameters
    data=appendValue(data,currentTime,(goToDataTail(data)->susceptible)-1,(goToDataTail(data)->infected)+1,(goToDataTail(data)->recovered)+0);
    int recoveryTime=currentTime+1;// +1 so that no chance that transmission and recovery can occour on same day
    int prob = rand()%10;// to calculate probability of recovery on a day
    while(prob>RecoveryRate)// if not recovered today , increase day count
    {
        prob=rand()%10;
        recoveryTime+=1;
    }
    if(recoveryTime<TMAX)// if recovery occours before max time limit
    {
        head=Enqueue(head,recoveryTime,1,nodeVal);// add recovery event
    }
    node* neighbour = G->arrayOfHeads[nodeVal].head;// variable to store the head list
    while(neighbour->next!=NULL)// for all neighbours to the head list
    {
        findTransmission(head,currentTime,G,neighbour->uniqueID,nodeVal);// find transmission to them
        neighbour=neighbour->next;// traverse head list
    }
}
void processRecovery(network*G,int nodeVal,dataNode*data, int currentTime)// function to change status of node to recovered
{
    if(G->arrayOfHeads[nodeVal].state==-1)// if node was infected
    {
        G->arrayOfHeads[nodeVal].state=1;// node recovered
    }
    else
    {
        return ;// else return without executing other code
    }
    // update the data queue with new parameters
    data=appendValue(data,currentTime,(goToDataTail(data)->susceptible)+0,(goToDataTail(data)->infected)-1,(goToDataTail(data)->recovered)+1);
}
void findTransmission(eventNode* queueHead, int currentTime,network*G,int target, int source)
{
    if(G->arrayOfHeads[target].state==0)
    {
        int infectionTime = currentTime+1;// +1 so that no chance that transmission and recovery can occour on same day
        int prob = rand()%10;// to calculate probability of infection on a day
        while(prob>TransmissionRate)// if not infected today , increase day count
        {
            prob=rand()%10;
            infectionTime+=1;
        }
        //printf("%d ",infectionTime); // uncomment to see infection day
        //&&!FindEvent(queueHead,target,0,infectionTime)&&!FindEvent(queueHead,source,1,infectionTime)
        if(infectionTime<TMAX)
        {
            queueHead=Enqueue(queueHead,infectionTime,0,target);
            // target predicted infection time is infection time
        }
    }
}
eventNode* Enqueue(eventNode* queueHead, int day , int type ,int originNode)// day,type,node
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
    newNode= malloc(sizeof(eventNode));// allocate memory and store data
    newNode->dayOfEvent=day;
    newNode->typeOf=type;
    newNode->nodeValue=originNode;
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
    else// if no node existed
    {
        newNode->previous=NULL;
        newNode->next=NULL;
    }
    queueHead=goToHead(newNode);// return to head and return that address
    return queueHead;
}
dataNode* appendValue(dataNode*head, int time,int sus, int inf , int rec)
{
    if(head!=NULL)
    {
        while(time>=head->dayOfEvent&&head->next!=NULL)// go to 1 step behind position
        {
            head=head->next;
        }
        if(head->dayOfEvent>time)// reach desired node
        {
            head=head->previous;
        }
    }
    dataNode* newNode = NULL;
    newNode= malloc(sizeof(dataNode));// allocate memory and store the data
    newNode->dayOfEvent=time;
    newNode->susceptible=sus;
    newNode->infected=inf;
    newNode->recovered=rec;
    if(head!=NULL)// change links on queueHead and node "previous" to queueHead
    {
        newNode->next=head->next;
        newNode->previous=head;
        if(head->next!=NULL)
        {
            head->next->previous=newNode;
        }
        head->next=newNode;
    }
    else// if no node existed
    {
        newNode->previous=NULL;
        newNode->next=NULL;
    }
    head=goToDataHead(newNode);// go to head and return that address
    return head;
}
dataNode* goToDataHead(dataNode* head)
{
    if(head!=NULL)// if passed value is not NULL
    {
        while(head->previous!=NULL)// go to previous value to reach head
        {
            head=head->previous;
        }
    }
    return head;// return head
}
dataNode* goToDataTail(dataNode* head)
{
    if(head!=NULL)// here head is actually the tail ~ can be seen as the other head
    {
        while(head->next!=NULL)// go to 'next' value to reach head
        {
            head=head->next;
        }
    }
    return head;// return head
}
eventNode* Dequeue(eventNode* queueHead)
{
    eventNode* newHead=queueHead->next;// update head 
    free(queueHead);// free memory allocated with head
    if(newHead!=NULL)// if new head is not null
    {
        newHead->previous=NULL;// update the previous parameter to be NULL
    }
    return newHead;// return new head
}
eventNode* goToHead(eventNode* head)
{
    if(head!=NULL)// if passed value is not NULL
    {
        while(head->previous!=NULL)
        {
            head=head->previous;// go to previous value to reach head
        }
    }
    return head;// return head
}
eventNode* goToTail(eventNode* head)
{
    if(head!=NULL)// here head is actually the tail ~ can be seen as the other head
    {
        while(head->next!=NULL)
        {
            head=head->next;// go to 'next' value to reach head
        }
    }
    return head;// return head
}
void PrintQueueAscending(eventNode* head)
{
    while(head!=NULL)// while end of queue is not reached
    {
        printf("%d ",head->dayOfEvent);// print day by default, parameter can be changed
        head=head->next;// traverse ahead in queue
    }
    printf("\n");
}
void PrintStatuses(dataNode*head)
{
    while(head!=NULL)// while end of queue is not reached
    {
        // print the values
        printf("Time:%d, Susceptible:%d ,Infected:%d ,Recovered:%d\n",head->dayOfEvent,head->susceptible,head->infected,head->recovered);
        head=head->next;// traverse ahead in queue
    }
    printf("\n");
}
void PrintToCSV(dataNode*head)
{
    FILE * csvPtr;
    csvPtr=fopen("dataSet.csv","w");// create new csv file
    while(head!=NULL)// while end of queue is not reached
    {
        // print the values to file
        fprintf(csvPtr,"%d,%d,%d,%d\n",head->dayOfEvent,head->susceptible,head->infected,head->recovered);
        head=head->next;// traverse ahead in queue
    }
    fclose(csvPtr);// close the file
}
void PrintQueueDescending(eventNode* head)
{
    head=goToTail(head);
    while(head!=NULL)// while end of queue is not reached
    {
        printf("%d ",head->dayOfEvent);// print day by default, parameter can be changed
        head=head->previous;// traverse ahead in queue
    }
    printf("\n");
}
node* insertVertex(int number)
{
    node* newVertex = NULL;
    newVertex=malloc(sizeof(node));// allocate memory for neighbour node
    newVertex->uniqueID=number;// store the value passed
    newVertex->next=NULL;
    return newVertex;// return the new vertex
}
void placeEdge(network* G,int u ,int v)// u and v are the UIDs
{
    if(u!=v)// so that edge is not placed on self 
    {
        node* originVertex = NULL;
        // linking u to v
        originVertex=insertVertex(v);// insert edge to v
        originVertex->next=G->arrayOfHeads[u].head;// update head at u
        G->arrayOfHeads[u].head=originVertex;// store the new neighbour value
        G->arrayOfHeads[u].size+=1;// increase size
        // linking v to u 
        originVertex=insertVertex(u);// insert edge to u
        originVertex->next=G->arrayOfHeads[v].head;// update head at v
        G->arrayOfHeads[v].head=originVertex;// store the new neighbour value
        G->arrayOfHeads[v].size+=1;// increase size
    }
}
network* generateNetwork(int numberofVertexes)
{
    network* graphNetwork = NULL;
    graphNetwork= malloc(sizeof(network));
    graphNetwork->numberOfVertexes=numberofVertexes;// save the total number of vertexes globally
    graphNetwork->arrayOfHeads=calloc(numberofVertexes,sizeof(nodeHead));// malloc can be used with noofVertex* sizeof here -> check once
    for (int i = 0; i < numberofVertexes ; i++)// initalize all to NULL
    {
        graphNetwork->arrayOfHeads[i].head=NULL;
        graphNetwork->arrayOfHeads[i].size=0;
        graphNetwork->arrayOfHeads[i].state=0;// initally susceptible
    }
    return graphNetwork;// return the network generated
}
void printGraph(network* graphHead)
{
    for (int i=0;i<graphHead->numberOfVertexes;i++)
    {
        nodeHead temp = graphHead->arrayOfHeads[i];// array of heads
        printf("%d (%d) (%d): ",i,temp.size,temp.state);// prints the entire network along with the state of node
        while(temp.head!=NULL)
        {
            printf("%d ,",temp.head->uniqueID);// prints all neighbours
            temp.head=temp.head->next;// traverse ahead
        }
        printf("\n");
    }
}
void fillNetwork(network*graph ,int numberOfVertices,int maxNumberOfEdges)
{
    srand(time(NULL));
    for(int u = 0 ; u< numberOfVertices ; u++)// u is source vertex
    {
        for(int v = 0 ;v < maxNumberOfEdges&& graph->arrayOfHeads[u].size<maxNumberOfEdges; v++)// until max limit on number of vertex is reached
        {
            if(rand()%2 == 1)// flip a coin
            {
                int linkedVertex = rand() % numberOfVertices;// generate random vertex
                if((!isDuplicate(graph->arrayOfHeads[u].head,linkedVertex))&&(!isDuplicate(graph->arrayOfHeads[linkedVertex].head,u)))// if they are not linked to each other previously
                {
                    if(graph->arrayOfHeads[linkedVertex].size<maxNumberOfEdges)// and the vertex has space to be linked
                    {
                        placeEdge(graph,u,linkedVertex);// join the two vertices
                    }
                }
            }
        }
    }
}
int isDuplicate(node* u,int value)
{
    node* temp = u;// create a temp head
    if(temp!=NULL)
    {
        while(temp!=NULL)// in all elements in list at head
        {
            if(temp->uniqueID==value)// if value passed exists
            {
                return 1;// return true
            }
            temp=temp->next;// traverse list
        }
    }
    return 0;// return false
}
int FindEvent(eventNode* queueHead, int node,int event,int givenTime)
{
    // and no event of source's recovery before this and target doesnt infect before this due to another node
    if(queueHead!=NULL)
    {
        while(queueHead->next!=NULL)
        {
            if(queueHead->nodeValue==node && queueHead->typeOf==event && givenTime>queueHead->dayOfEvent)
            {
                return 1;// event exists
            }
            queueHead=queueHead->next;
        }
    }
    return 0;// no event exists
}
