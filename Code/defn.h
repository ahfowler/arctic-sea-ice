// Components for storing SICs
struct sicNode
{
    int sic;
    sicNode *next;
};

struct cell
{
    sicNode *readings;
};

// Components for building the graph
struct vertexNode
{
    int vertex;
    vertexNode *next;
};

// How to build the adjecancy list:
// 1. Make an array of nertexNode pointers the size of the amount of vertexes there are. Initialize them all to NULL.
// 2. If there is an edge between two vertexNodes, use addEdge().

void addEdge(vertexNode *adjList[], int nodeOne, int nodeTwo)
{
    vertexNode *newNode = new vertexNode();
    newNode->vertex = nodeTwo;
    newNode->next = NULL;

    if (adjList[nodeOne] == NULL)
    {
        adjList[nodeOne] = newNode;
    }
    else
    {
        newNode->next = adjList[nodeOne];
        adjList[nodeOne] = newNode;
    }
};
