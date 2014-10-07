/*
 * MergeSort.cilk
 * 
 * Frederic Marchand
 * 100817579
 */

#include <cilk/cilk.h>
#include <cilktools/cilkview.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string.h>
using namespace std;

#define DEBUG 0 

int n;
int *inputArray;
int *outputArray;

int binarySearch(int *array, int x, int lowerBound, int upperBound)
{
    int middle;

    if (lowerBound > upperBound)
    {
        return -1;
    }
    
    middle = (lowerBound + upperBound)/2;

    if (array[middle] == x)
    {
        return middle;
    }
    else if (array[middle] < x)
    {
        return binarySearch(array, x, middle+1, upperBound);
    }
    else
    {
        return binarySearch(array, x, lowerBound, middle-1);
    }
}

void parallelMerge(int *C, int *A, int *B, int na, int nb)
{
    if (na < nb)
    {
        cilk_spawn parallelMerge(C, B, A, nb, na);
    }
    else if (na == 1)
    {
        if (nb == 0)
        {
            C[0] = A[0];
        }
        else
        {
            C[0] = (A[0] < B[0]) ? A[0] : B[0];
            C[1] = (A[0] < B[0]) ? B[0] : A[1];
        }
    }
    else
    {
        int ma = na/2;
        int mb = binarySearch(A[ma], B, nb);
        cilk_spawn parallelMerge(C, A, B, ma, mb);
        cilk_spawn parallelMerge(C+ma+mb, A+ma, B+mb, na-ma, nb-mb);
        cilk_sync;
    }

}

void parallelMergeSort(int leftIndex, int rightIndex)
{
    if ((rightIndex - leftIndex) == 1)
    {
        outputArray[leftIndex] = inputArray[leftIndex];
    }
    else
    {
        cilk_spawn parallelMergeSort(leftIndex, rightIndex/2);
        cilk_spawn parallelMergeSort(leftIndex + rightIndex/2, rightIndex); 
        cilk_sync;
        parallelMerge();
    }
}

void printArray()
{
    for (int i = 0; i < n; ++i)
    {
        cout << inputArray[i] << " " << endl;
    }
}

void freeMem()
{
    delete [] inputArray;
    delete [] outputArray;
}

int main(int argc, char** argv) 
{
    char * inputFilepath;
    char * outputFilepath;
    int time1;
    int time2;
    int par_time;
    string buffer;

    if (argc <= 2) 
    {
        cerr << "The program is missing an argument" << endl; 
        return 1;   /// Error
    }

    inputFilepath = argv[1]; 
    outputFilepath = argv[2];

    ifstream input;
    input.open(inputFilepath);

    if (!input.is_open())
    {
        cerr << "Could not open file " << inputFilepath << endl;
        return 1;   /// Error
    }

    getline(input, buffer);
    n = atoi(buffer.c_str());

    cout << "n: " << n << endl;

    inputArray = new int[n];
    outputArray = new int[n];

    for (int i = 0; i < n; ++i)
    {
        getline(input, buffer);
        inputArray[i] = atoi(buffer.c_str());
    }

    input.close();

#if DEBUG == 1
    printArray();
#endif

    /// Run the program
    time1 = __cilkview_getticks();

    // Run Merge Sort


    time2 = __cilkview_getticks();

    par_time = time2-time1;
    cout << "\nMedian Filter took " << par_time << " milliseconds." << endl;

    /// Write to output file
    ofstream outputFile (outputFilepath);
    if (!outputFile.is_open())
    {
        cerr << "Failed to open output file" << endl;
        return 1;   /// Error
    }
   
    for (int i = 0; i < n; ++i)
    {
        outputFile << outputArray[i] << endl;
    }

    outputFile.close();

    freeMem();

    return 0;
}


