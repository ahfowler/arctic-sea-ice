#include <vector>

#include <iostream>

using namespace std;

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
    int index;
    int color; // 0 = white, 1 = grey, 2 = black
    vertexNode *parent;
    vertexNode *next;
};

// How to build the adjecancy list:
// 1. Make an array of nertexNode pointers the size of the amount of vertexes there are. Initialize them all to NULL.
// 2. If there is an edge between two vertexNodes, use addEdge().

// bool contains(vertexNode node, vertexNode currentNode)
// {
//     vertexNode trav = node;

//     while (trav.next != NULL)
//     {
//         if (trav.next->xCoordinate == currentNode.next->xCoordinate && trav.next->yCoordinate == currentNode.next->yCoordinate)
//         {
//             return true;
//         }
//     }

//     return false;
// }

void addEdge(vertexNode adjList[], int vertex, int currentVertex)
{
    //cout << "Comparing " << vertex << " " << currentVertex << endl;
    // if (!contains(adjList[vertex], adjList[currentVertex]))
    // {
    // Create currentVertex node
    vertexNode *newCurrentVertex = new vertexNode();
    newCurrentVertex->xCoordinate = currentVertex / 63;
    newCurrentVertex->yCoordinate = currentVertex % 63;
    newCurrentVertex->index = currentVertex;
    newCurrentVertex->next = NULL;

    // Add to vertex's edges
    if (adjList[vertex].next == NULL)
    {
        adjList[vertex].next = newCurrentVertex;
    }
    else
    {
        newCurrentVertex->next = adjList[vertex].next;
        adjList[vertex].next = newCurrentVertex;
    }

    //cout << "Just added " << adjList[vertex].next->xCoordinate << ", " << adjList[vertex].next->yCoordinate << " to " << vertex / 63 << ", " << vertex % 63 << endl;

    // Create vertex node
    vertexNode *newVertex = new vertexNode();
    newVertex->xCoordinate = vertex / 63;
    newVertex->yCoordinate = vertex % 63;
    newVertex->index = vertex;
    newVertex->next = NULL;

    // Add to currentVertex's edges
    if (adjList[currentVertex].next == NULL)
    {
        adjList[currentVertex].next = newVertex;
    }
    else
    {
        newVertex->next = adjList[currentVertex].next;
        adjList[currentVertex].next = newVertex;
    }

    //cout << "Just added " << adjList[currentVertex].next->xCoordinate << ", " << adjList[currentVertex].next->yCoordinate << " to " << currentVertex / 63 << ", " << currentVertex % 63 << endl;
    // }
};
