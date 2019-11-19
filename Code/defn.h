#include <vector>

struct cell
{
    std::vector<double> readings;
    float mean;
};

// Components for building the graph
struct vertexNode
{
    int xCoordinate;
    int yCoordinate;
    vertexNode *next;
};

// How to build the adjecancy list:
// 1. Make an array of nertexNode pointers the size of the amount of vertexes there are. Initialize them all to NULL.
// 2. If there is an edge between two vertexNodes, use addEdge().

void addEdge(vertexNode *adjList[], int vertex, int currentVertex)
{
    // Create currentVertex node
    vertexNode *newCurrentVertex = new vertexNode();
    newCurrentVertex->xCoordinate = currentVertex / 63;
    newCurrentVertex->yCoordinate = currentVertex % 63;
    newCurrentVertex->next = NULL;

    // Add to vertex's edges
    if (adjList[vertex] == NULL)
    {
        adjList[vertex] = newCurrentVertex;
    }
    else
    {
        newCurrentVertex->next = adjList[vertex];
        adjList[vertex] = newCurrentVertex;
    }

    // Create vertex node
    vertexNode *newVertex = new vertexNode();
    newVertex->xCoordinate = vertex / 63;
    newVertex->yCoordinate = vertex % 63;
    newVertex->next = NULL;

    // Add to currentVertex's edges
    if (adjList[currentVertex] == NULL)
    {
        adjList[currentVertex] = newVertex;
    }
    else
    {
        newVertex->next = adjList[currentVertex];
        adjList[currentVertex] = newVertex;
    }
};
