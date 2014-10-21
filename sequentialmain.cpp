/*
 * MergeSort.cilk
 * 
 * Frederic Marchand
 * 100817579
 * Sequential Version of Parallel Merge Sort
 */

#include <cilk/cilk.h>
#include <cilktools/cilkview.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string.h>
#include <stdlib.h>
using namespace std;

#define PRINTING 1 

int *inputArray;
int *outputArray;

int binarySearch(int x, int *array, int low, int high)
{
    int mid;
    while (low <= high)
    {
        mid = (low + high) / 2;
        if (array[mid] > x)
        {
            high = mid - 1;
        }
        else if (array[mid] < x)
        {
            low = mid + 1;
        }
        else
        {
            return mid;
        }
    }
    return low;
}

void parallelMerge(int *C, int *A, int *B, int na, int nb)
{
    if (na < nb)
    {
        parallelMerge(C, B, A, nb, na);
    }
    else if (na == 0)
    {
        return;
    }
    else
    {
        int ma = na/2;
        int mb = binarySearch(A[ma], B, 0, nb-1);
        C[ma + mb] = A[ma];
        parallelMerge(C, A, B, ma, mb);
        parallelMerge(C+ma+mb+1, A+ma+1, B+mb, na-ma-1, nb-mb);
    }

}

void parallelMergeSort(int *B, int *A, int n)
{
    if (n == 1)
    {
        B[0] = A[0];
    }
    else
    {
        int *C = (int *) malloc(n * sizeof(int));
        parallelMergeSort(C,     A,     n/2);
        parallelMergeSort(C+n/2, A+n/2, n-n/2); 
        parallelMerge(B, C, C+n/2, n/2, n-n/2);
        free (C);
    }
}

void printArray(int n)
{
    for (int i = 0; i < n; ++i)
    {
        cout << outputArray[i] << " " << endl;
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
    int n;
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

    /// Run the program
    time1 = __cilkview_getticks();

    // Run Merge Sort
    parallelMergeSort(outputArray, inputArray, n);

    time2 = __cilkview_getticks();

    par_time = time2-time1;
    cout << "Parallel Merge Sort took " << par_time << " milliseconds." << endl;

#if PRINTING == 1
    /// Write to output file
    ofstream outputFile (outputFilepath);
    if (!outputFile.is_open())
    {
        cerr << "Failed to open output file" << endl;
        return 1;   /// Error
    }
  
    outputFile << n << endl;
    for (int i = 0; i < n; ++i)
    {
        outputFile << outputArray[i] << endl;
    }

    outputFile.close();
#endif

    freeMem();

    return 0;
}


