// Remember: Each file contains every point's sea ice concentration for the given week.

#include <fstream>
#include <iostream>
#include "defn.h"

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
            newCell->readings = NULL;
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
            for (int k = 0; k < 63; k++) // (k,l) are the cells
            {
                for (int l = 0; l < 63; l++)
                {
                    inputFile.read((char *)&dataIn, 4);

                    // Step 1b: Store the sea ice concentration as a node in a linked list for each point.
                    sicNode *newNode = new sicNode();
                    newNode->sic = dataIn;
                    newNode->next = NULL;

                    if (landmask[k][l].readings == NULL)
                    {
                        landmask[k][l].readings = newNode;
                    }
                    else
                    {
                        newNode->next = landmask[k][l].readings;
                        landmask[k][l].readings = newNode;
                    }
                }
            }
            inputFile.close();
        }
    }

    // for (int k = 62; k >= 0; k--) // (k,l) are the cells
    // {
    //     for (int l = 62; l >= 0; l--)
    //     {
    //         sicNode *trav = landmask[k][l].readings;
    //         while (trav != NULL)
    //         {
    //             cout << trav->sic << " ";
    //             trav = trav->next;
    //         }
    //     }
    //     cout << endl;
    // }

    // Step 2: Make the adjacency-list graph for each threshold.
    int adjListSize = 63 * 63; // 63 x 63 landmask
    int adjList[adjListSize];

    double threshold = 0.95;

    // Step 2a: Compute the correlation between every point to every other point.
    for (int k = 0; k < 63; k++) // (k,l) are the cells
    {
        for (int l = k; l < 63; l++)
        {
            // Compute the correlation.
            int upperBound = 1 * 52; // number of Years * 52

            int Xi;
            int Yi;

            int Xmean;
            int Ymean;

            // Step 1: Compute Sxx
            int Sxx = 0;

            for (int i = 0; i < upperBound; i++)
            {
                sicNode *trav = landmask[k][l].readings;
                while (trav != NULL)
                {
                    Sxx += trav.sic;
                    trav = trav->next;
                }
            }

            // Step 2: Compute Syy

            // Step 3: Compute Sxy

            // Step 4: Plug into the formula.
        }
    }

    // Step 2b: If that correlation is above the threshold, add an edge.

    // Step 3: Compute the degrees of each vertex.

    // Step 4: Do a recursive depth-first traversal to compute the number of "trees" and their size.
}