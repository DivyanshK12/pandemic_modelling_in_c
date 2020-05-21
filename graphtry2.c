#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>

#define MAX_VERTICES 100
// actually 10000
#define MAX_EDGES 50
// actually 3000
typedef unsigned char vertex;

typedef struct graphNode
{
    int uniqueID;// numbers the nodes
    int state;//0 for susceptible , -1 for infected and 1 for recovered
    struct graphNode* next;
}node;// models a human in the model

typedef struct nodeHead
{
    node* head;// saves address for all nodes to traverse
    int size;// to control max edges 
}nodeHead;
typedef struct graph
{
    int numberOfVertexes;// size of that array
    nodeHead* arrayOfHeads;// array to store the heads
}network;

node* insertVertex(int number, int state);// inserts node with given state
void placeEdge(network* G,int u ,int v);// places edge between vertexes u and v
void printGraph(network* graphHead);
network* generateNetwork(int numberofVertexes);// generates random undirected graph network
void fillNetwork(network*graph ,int numberOfVertices,int maxNumberOfEdges);// populates the graph randomly
int isDuplicate(node* u,int value);

int main(void)
{
    /*number of nodes in a graph*/
    srand ( time(NULL) );
    int numberOfVertices = rand() % MAX_VERTICES;
    /*number of maximum edges a vertex can have*/
    srand ( time(NULL) );
    int maxNumberOfEdges = rand() % MAX_EDGES;
    printf("Vertex%d , Edge%d\n",numberOfVertices,maxNumberOfEdges);
    network* epidemic = generateNetwork(numberOfVertices);
    fillNetwork(epidemic,numberOfVertices,maxNumberOfEdges);
    printGraph(epidemic);
}
node* insertVertex(int number, int state)
{
    node* newVertex = NULL;
    newVertex=malloc(sizeof(node));
    newVertex->uniqueID=number;
    newVertex->state=state;
    newVertex->next=NULL;
    return newVertex;
}
void placeEdge(network* G,int u ,int v)// u and v are the UIDs
{
    if(u!=v)// so that edge is not placed on self 
    {
        node* originVertex = NULL;
        // linking u to v
        originVertex=insertVertex(v,0);// default is susceptible
        originVertex->next=G->arrayOfHeads[u].head;
        G->arrayOfHeads[u].head=originVertex;
        G->arrayOfHeads[u].size+=1;
        // linking v to u 
        originVertex=insertVertex(u,0);// default is susceptible
        originVertex->next=G->arrayOfHeads[v].head;
        G->arrayOfHeads[v].head=originVertex;
        G->arrayOfHeads[v].size+=1;
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
    }
    return graphNetwork;
}
void printGraph(network* graphHead)
{
    for (int i=0;i<graphHead->numberOfVertexes;i++)
    {
        nodeHead temp = graphHead->arrayOfHeads[i];
        printf("%d (%d) : ",i,temp.size);
        while(temp.head!=NULL)
        {
            printf("%d ,",temp.head->uniqueID);
            temp.head=temp.head->next;
        }
        printf("\n");
    }
}
void fillNetwork(network*graph ,int numberOfVertices,int maxNumberOfEdges)
{
    srand(time(NULL));
    for(int u = 0 ; u< numberOfVertices ; u++)// u is source vertex
    {
        for(int v = 0 ;v < maxNumberOfEdges&& graph->arrayOfHeads[u].size<maxNumberOfEdges; v++)
        {
            if(rand()%2 == 1)
            {
                int linkedVertex = rand() % numberOfVertices;
                if((!isDuplicate(graph->arrayOfHeads[u].head,linkedVertex))&&(!isDuplicate(graph->arrayOfHeads[linkedVertex].head,u)))
                {
                    if(graph->arrayOfHeads[linkedVertex].size<maxNumberOfEdges)
                    {
                        placeEdge(graph,u,linkedVertex);
                    }
                }
            }
        }
    }
}
int isDuplicate(node* u,int value)
{
    node* temp = u;
    if(temp!=NULL)
    {
        while(temp!=NULL)
        {
            if(temp->uniqueID==value)
            {
                return 1;
            }
            temp=temp->next;
        }
    }
    return 0;
}