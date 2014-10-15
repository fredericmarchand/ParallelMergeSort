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
#include <stdlib.h>
using namespace std;

#define DEBUG 0 

int *inputArray;
int *outputArray;

int binarySearch(int x, int *array, int upperBound)
{
    int lowerBound = 0;
    int position = (upperBound / 2);

    while ((array[position] != x) && (lowerBound <= upperBound))
    {
        if (array[position] > x)
        {
            upperBound = position - 1;
        }
        else
        {
            lowerBound = position + 1;
        }
        position = (lowerBound + upperBound) / 2;
    }

    return position;
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
            C[1] = (A[0] < B[0]) ? B[0] : A[0];
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

void parallelMergeSort(int *B, int *A, int n)
{
    if (n == 1)
    {
        B[0] = A[0];
    }
    else
    {
        int *C = (int *)malloc(n * sizeof(int));
        cilk_spawn parallelMergeSort(C, A, n/2);
        cilk_spawn parallelMergeSort(C+n/2, A+n/2, n-n/2); 
        cilk_sync;
        parallelMerge(B, C, C+n/2, n/2, n-n/2);
        free(C);
    }
}

void printArray(int n)
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

#if DEBUG == 1
    printArray(n);
#endif

    /// Run the program
    time1 = __cilkview_getticks();

    // Run Merge Sort

    parallelMergeSort(outputArray, inputArray, n);

    time2 = __cilkview_getticks();

    par_time = time2-time1;
    cout << "\nParallel Merge Sort took " << par_time << " milliseconds." << endl;

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

    freeMem();

    return 0;
}


