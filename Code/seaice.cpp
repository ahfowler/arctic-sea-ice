// Remember: Each file contains every point's sea ice concentration for the given week.

#include <fstream>
#include <iostream>
#include "dfs.h"
#include <vector>
#include <cmath>

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

    for (int i = 1990; i < 1991; i++)
    {
        year = i;
        for (int j = 1; j < 53; j++)
        {
            week = j;

            // Create path name for file.
            pathName = to_string(year);

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
            int numberOfReadings = 0;

            for (int j = 0; j < landmask[index / 63][index % 63].readings.size(); j++)
            {
                if (landmask[index / 63][index % 63].readings[j] != 157 && landmask[index / 63][index % 63].readings[j] != 168)
                {
                    sum += landmask[index / 63][index % 63].readings[j];
                    numberOfReadings++;
                }
            }

            if (numberOfReadings != 0)
            {
                float mean = sum / numberOfReadings;
                landmask[index / 63][index % 63].mean = mean;
            }
            else
            {
                landmask[index / 63][index % 63].mean = std::numeric_limits<float>::quiet_NaN();
            }

            // Create vertex cell for adjacency list.
            vertexNode *newVertex = new vertexNode();
            newVertex->xCoordinate = index / 63;
            newVertex->yCoordinate = index % 63;
            newVertex->next = NULL;
            newVertex->index = index;

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
            // Step 1: Compute Sxx
            cell vertexCell = landmask[vertex / 63][vertex % 63];

            // cout << "Cell ( " << vertex / 63 << ", " << vertex % 63 << ") vertex mean : " << vertexCell.mean << endl;

            if (!isnan(vertexCell.mean))
            { // This is a valid cell.
                // cout << "Xmean: " << Xmean << endl;
                float Sxx = 0;

                for (int i = 0; i < vertexCell.readings.size(); i++)
                {
                    if (vertexCell.readings[i] != 157 && vertexCell.readings[i] != 168)
                    {                                                                                                   // If it is not missing,
                        Sxx += (vertexCell.readings[i] - vertexCell.mean) * (vertexCell.readings[i] - vertexCell.mean); // Add to Sxx summation
                    }
                }

                // cout << "Cell ( " << vertex / 63 << ", " << vertex % 63 << ") Sxx : " << Sxx << endl;

                // Find edges in the rest of the graph
                for (int comparingVertex = vertex + 1; comparingVertex < adjListSize; comparingVertex++)
                {
                    // Step 2: Compute Syy
                    cell comparingVertexCell = landmask[comparingVertex / 63][comparingVertex % 63];

                    if (!isnan(comparingVertexCell.mean))
                    { // This a valid cell to compare to.
                        // cout << "Ymean: " << Ymean << endl;
                        float Syy = 0;

                        for (int k = 0; k < comparingVertexCell.readings.size(); k++)
                        {
                            if (comparingVertexCell.readings[k] != 157 && comparingVertexCell.readings[k] != 168)
                            {                                                                                                                                       // If it is not missing,
                                Syy += (comparingVertexCell.readings[k] - comparingVertexCell.mean) * (comparingVertexCell.readings[k] - comparingVertexCell.mean); // Add to Syy summation
                            }
                        }

                        // cout << "Cell ( " << comparingVertex / 63 << ", " << comparingVertex % 63 << ") Syy : " << Syy << endl;

                        // Step 3: Compute Sxy
                        float Sxy = 0; // Both are valid cells!
                        for (int j = 0; j < vertexCell.readings.size(); j++)
                        {
                            if (comparingVertexCell.readings[j] != 157 && comparingVertexCell.readings[j] != 168 && vertexCell.readings[j] != 157 && vertexCell.readings[j] != 168)
                            {                                                                                                                     // If it is not missing,
                                Sxy += (vertexCell.readings[j] - vertexCell.mean) * (comparingVertexCell.readings[j] - comparingVertexCell.mean); // Add to Sxy summation
                            }
                        }

                        // cout << "Sxy : " << Sxy << endl;

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
        vector<int> degrees(1, 0);

        for (int i = 0; i < adjListSize; i++)
        {
            int numOfDegrees = 0;
            vertexNode trav = adjList[i];

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
            // cout << numOfDegrees << endl;
            // cout << degrees[numOfDegrees] << endl;
        }

        cout << "Graph Characteristics with Threshold: " << threshold << endl;
        cout << "Degree Histogram: " << endl;

        for (int i = 0; i < degrees.size(); i++)
        {
            cout << i << ": ";
            for (int j = 0; j < degrees[i]; j++)
            {
                cout << "*";
            }
            cout << endl;
        }
        cout << endl;

        // Step 4: Do a recursive depth-first traversal to compute the number of "trees" and their size.
        cout << "Connected Components with Sizes" << endl;
        dfs(adjList, adjListSize);

        // for (int i = 0; i < adjListSize; i++)
        // {
        //     vertexNode *trav = &adjList[i];
        //     cout << "(" << trav->xCoordinate << ", " << trav->yCoordinate << ") : " << trav->color << endl;
        //     trav = trav->next;
        // }

        cout << endl;
    }
}