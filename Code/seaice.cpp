// Remember: Each file contains every point's sea ice concentration for the given week.

#include <fstream>
#include <iostream>
#include "dfs.h"
#include <vector>
#include <cmath>
#include <limits>

#define inf INT_MAX

using namespace std;

int main()
{
    // Step 1: Read the binary file and store all the sea ice concentrations.
    int week, year;
    string fileName, pathName;

    cell **landmask = new cell *[63];
    for (int i = 0; i < 63; ++i)
    {
        landmask[i] = new cell[63];
    }

    // Create the cells for the landmask.
    for (int k = 0; k < 63; k++) // (k,l) are the cells
    {
        for (int l = 0; l < 63; l++)
        {
            cell *newCell = new cell();
            landmask[k][l] = *newCell;
        }
    }

    for (int i = 1990; i < 2006; i++)
    {
        year = i;
        for (int j = 1; j < 53; j++)
        {
            week = j;

            // Create path name for file.
            pathName = "CS310_project_subregion/" + to_string(year);

            if (week < 10)
            {
                fileName = "Beaufort_Sea_diffw0" + to_string(week) + "y" + to_string(year) + "+landmask";
            }
            else
            {
                fileName = "Beaufort_Sea_diffw" + to_string(week) + "y" + to_string(year) + "+landmask";
            }

            pathName += "/" + fileName;

            // Step 1a: Open the file and read each line for the week's sea ice concentrations.
            ifstream inputFile(pathName, ios::in | ios::binary);
            float dataIn = 0;
            int Xcount = 0;
            int Ycount = 0;

            while (Ycount < 63)
            {
                while (Xcount < 63)
                {
                    inputFile.read((char *)&dataIn, 4);

                    // Step 1b: Store the sea ice concentration in the cell's readings.

                    landmask[Ycount][Xcount].readings.push_back(dataIn);

                    Xcount++;
                }

                Xcount = 0;
                Ycount++;
            }

            inputFile.close();
        }
    }

    // for (int k = 0; k < 63; k++) // (k,l) are the cells
    // {
    //     for (int l = 0; l < 63; l++)
    //     {
    //         cout << "Cell (" << k << "," << l << ") : ";
    //         for (int m = 0; m < 52 * 2; m++) // 52 * number of Years
    //         {
    //             cout << landmask[k][l].readings[m] << " ";
    //         }
    //         cout << endl;
    //     }
    //     cout << endl;
    // }

    // Step 2: Make the adjacency-list graph for each threshold.
    int adjListSize = 63 * 63; // 63 x 63 landmask
    vertexNode *adjList = NULL;
    adjList = new vertexNode[adjListSize];
    // fill_n(adjList, adjListSize, NULL);

    float thresholds[3] = {0.95, 0.925, 0.9};

    // Step 2a: Compute the correlation between every point to every other point.
    for (int threshIndex = 0; threshIndex < 3; threshIndex++)
    {
        float threshold = thresholds[threshIndex];
        // Compute all the means and fill adjList
        for (int index = 0; index < adjListSize; index++)
        {
            // Compute the mean of each vertex
            float sum = 0;
            float numberOfReadings = 0;

            cell *vertexCell = &landmask[index / 63][index % 63];

            for (int j = 0; j < vertexCell->readings.size(); j++)
            {
                if (vertexCell->readings[j] != 157 && vertexCell->readings[j] != 168)
                {
                    sum += vertexCell->readings[j];
                    numberOfReadings++;
                }
            }

            if (numberOfReadings != 0)
            {
                vertexCell->mean = sum / numberOfReadings;
            }
            else
            {
                vertexCell->mean = std::numeric_limits<float>::quiet_NaN();
            }

            // cout << "Cell ( " << vertex / 63 << ", " << vertex % 63 << ") vertex mean : " << vertexCell.mean << endl;
            float Sxx = 0;

            for (int i = 0; i < vertexCell->readings.size(); i++)
            {
                if (vertexCell->readings[i] != 157 && vertexCell->readings[i] != 168)
                {                                                                                                       // If it is not missing,
                    Sxx += (vertexCell->readings[i] - vertexCell->mean) * (vertexCell->readings[i] - vertexCell->mean); // Add to Sxx summation
                }
            }

            vertexCell->Sxx = Sxx;
            //cout << landmask[index / 63][index % 63].Sxx << endl;

            // Create vertex cell for adjacency list.
            vertexNode *newVertex = new vertexNode();
            newVertex->xCoordinate = index / 63;
            newVertex->yCoordinate = index % 63;
            newVertex->next = NULL;

            if (landmask[index / 63][index % 63].readings[0] == 168)
            {
                newVertex->land = true;
            }
            else
            {
                newVertex->land = false;
            }

            adjList[index] = *newVertex;

            // cout << "Cell ( " << index / 63 << ", " << index % 63 << ") mean : " << landmask[index / 63][index % 63].mean << endl;
        }

        for (int vertex = 0; vertex < adjListSize; vertex++)
        {
            cell *vertexCell = &landmask[vertex / 63][vertex % 63];
            float Sxx = vertexCell->Sxx;
            //cout << "Sxx Stored for " << vertex << ": " << vertexCell->Sxx << endl;

            if (!isnan(vertexCell->mean))
            {
                // Find edges in the rest of the graph
                for (int comparingVertex = vertex + 1; comparingVertex < adjListSize; comparingVertex++)
                {
                    // Step 2: Compute Syy
                    cell *comparingVertexCell = &landmask[comparingVertex / 63][comparingVertex % 63];
                    float Syy = comparingVertexCell->Sxx;
                    //cout << "Syy Stored for " << comparingVertex << ": " << comparingVertexCell->Sxx << endl;

                    if (!isnan(comparingVertexCell->mean))
                    { // This a valid cell to compare to.
                        // Step 3: Compute Sxy
                        float Sxy = 0; // Both are valid cells!
                        for (int j = 0; j < vertexCell->readings.size(); j++)
                        {
                            if (comparingVertexCell->readings[j] != 157 && comparingVertexCell->readings[j] != 168 && vertexCell->readings[j] != 157 && vertexCell->readings[j] != 168)
                            {                                                                                                                         // If it is not missing,
                                Sxy += (vertexCell->readings[j] - vertexCell->mean) * (comparingVertexCell->readings[j] - comparingVertexCell->mean); // Add to Sxy summation
                            }
                        }

                        //cout << "Sxy : " << Sxy << endl;

                        // Step 4: Plug into the formula.
                        float coorelation = Sxy / sqrt(Sxx * Syy);
                        // cout << "Coorelation: " << abs(coorelation) << endl;

                        // Step 2b: If that correlation is above the threshold, add an edge.
                        if (abs(coorelation) > threshold)
                        {
                            addEdge(adjList, vertex, comparingVertex);
                        }
                    }
                    else
                    {
                        // This is not a valid cell, just skip it.
                    }
                }
            }
            else
            {
                // This is not a valid cell, just skip it.
            }
        }

        // for (int i = 0; i < adjListSize; i++)
        // {
        //     vertexNode *trav = &adjList[i];
        //     cout << "(" << trav->xCoordinate << ", " << trav->yCoordinate << ") : ";
        //     while (trav->next != NULL)
        //     {
        //         cout << "(" << trav->next->xCoordinate << ", " << trav->next->yCoordinate << "), ";
        //         trav = trav->next;
        //     }
        //     cout << endl;
        // }

        // Step 3: Compute the degrees of each vertex.
        float averageDegrees = 0;
        float numOfVertices = 0;

        vector<int> degrees(1, 0);

        for (int i = 0; i < adjListSize; i++)
        {
            int numOfDegrees = 0;
            vertexNode trav = adjList[i];
            if (!trav.land)
            {
                numOfVertices++;
                while (trav.next != NULL)
                {
                    numOfDegrees++;
                    trav = *trav.next;
                }

                if (numOfDegrees > (degrees.size() - 1))
                {
                    degrees.resize(numOfDegrees + 1, 0);
                }

                degrees[numOfDegrees] += 1;
                averageDegrees += numOfDegrees;
                // cout << numOfDegrees << endl;
                // cout << degrees[numOfDegrees] << endl;
            }
        }
        averageDegrees = averageDegrees / numOfVertices; // Quanity of degrees / number of vertices

        cout << "Graph Characteristics with Threshold: " << threshold << endl;
        cout << "DEGREE HISTOGRAM: " << endl;

        for (int i = 0; i < degrees.size(); i++)
        {
            cout << i << ": ";
            for (int j = 0; j < degrees[i]; j++)
            {
                cout << "|";
            }
            cout << " (" << degrees[i] << ")";
            cout << endl;
        }
        cout << endl;

        // Step 4: Do a recursive depth-first traversal to compute the number of "trees" and their size.
        cout << "CONNECTED COMPONENTS (WITH SIZES)" << endl;
        dfs(adjList, adjListSize);

        // for (int i = 0; i < adjListSize; i++)
        // {
        //     vertexNode *trav = &adjList[i];
        //     cout << "(" << trav->xCoordinate << ", " << trav->yCoordinate << ") : " << trav->color << endl;
        //     trav = trav->next;
        // }

        cout << endl;

        // Step 5: Compute the clustering coefficient
        float sum = 0;
        float size = 0;
        float clusteringCoefficient = 0;
        for (int i = 0; i < adjListSize; i++)
        {
            if (!adjList[i].land)
            {
                sum += computeCC(i, adjList);
                size++;
            }
        }

        clusteringCoefficient = sum / size;
        cout << "COORELATION GRAPH CHARACTERISTICS\n";
        cout << "Clustering Coefficient: " << sum / size << endl;

        // Step 6: Compute the characteristic path length
        int **table = new int *[adjListSize];
        for (int i = 0; i < adjListSize; ++i)
        {
            table[i] = new int[adjListSize];
        }

        for (int i = 0; i < adjListSize; i++)
        {
            for (int j = i; j < adjListSize; j++)
            {
                if (i == j)
                {
                    table[i][j] = 0;
                }
                else
                {
                    int value = connected(i, j, adjList);
                    table[i][j] = value;
                    table[j][i] = value;
                }
            }
        }

        floydWarshall(table, adjListSize);

        sum = 0;
        size = 0;
        float characteristicPathLength = 0;

        for (int i = 0; i < adjListSize; i++)
        {
            for (int j = i + 1; j < adjListSize; j++)
            {
                // if (table[i][j] == 4000)
                // {
                //     cout << "inf" << ' ';
                // }
                // else
                // {
                //     cout << table[i][j] << ' ';
                // }

                if (table[i][j] != inf)
                {
                    sum += table[i][j];
                    size++;
                }
            }
        }

        characteristicPathLength = sum / size;

        cout << "Characteristic Path Length: " << characteristicPathLength << endl;
        cout << endl;

        // Step 7: Determine if the graph is a small world graph

        // Create a random graph
        float randomClusteringCoefficient = averageDegrees / numOfVertices;
        float randomCharacteristicPathLength = log(numOfVertices) / log(averageDegrees);

        cout << "RANDOM GRAPH CHARACTERISTICS\n";
        cout << "Clustering Cofficient: " << randomClusteringCoefficient << endl;
        cout << "Characteristic Path Length: " << randomCharacteristicPathLength << endl
             << endl;
        cout << "---------------------------------------------------------------------------------------------------" << endl
             << endl;

        // Is Gr >> Grandom && Gr >= Grandom?
    }
}