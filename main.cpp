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

#define NOALLOC 1 

#define DEBUG 0 
#if NOALLOC == 1
#define ARRAY_A 1
#define ARRAY_B 2
#endif

int *inputArray;
int *outputArray;

int binarySearch(int x, int *array, int low, int high)
{
    
    if (high < low)
    {
        return low;
    }
    int mid = (low + high) / 2;
    if (array[mid] > x)
    {
        return binarySearch(x, array, low, mid-1);
    }
    else if (array[mid] < x)
    {
        return binarySearch(x, array, mid+1, high);
    }
    else
    {
        return mid;
    }

}

void parallelMerge(int *C, int *A, int *B, int na, int nb)
{
    if (na < nb)
    {
        cilk_spawn parallelMerge(C, B, A, nb, na);
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
        cilk_spawn parallelMerge(C, A, B, ma, mb);
        parallelMerge(C+ma+mb+1, A+ma+1, B+mb, na-ma-1, nb-mb);
        cilk_sync;
    }

}

#if NOALLOC == 0
void parallelMergeSort(int *B, int *A, int n)
{
    if (n == 1)
    {
        B[0] = A[0];
    }
    else
    {
        int *C = (int *) malloc(n * sizeof(int));
        cilk_spawn parallelMergeSort(C,     A,     n/2);
        parallelMergeSort(C+n/2, A+n/2, n-n/2); 
        cilk_sync;
        parallelMerge(B, C, C+n/2, n/2, n-n/2);
        cilk_sync;
        free (C);
    }
}
#endif

#if NOALLOC == 1
void parallelMergeSort(int *A, int *B, int n, int AorB)
{
    if (n <= 1)
    {
        B[0] = A[0];
    }
    
    if (AorB == ARRAY_A)
    {
        cilk_spawn parallelMergeSort(A, B, n/2, ARRAY_B);
        parallelMergeSort(A+n/2, B+n/2, n-n/2, ARRAY_B); 
        cilk_sync;
        parallelMerge(A, B, B+n/2, n/2, n-n/2);
    }
    else
    {
        cilk_spawn parallelMergeSort(A, B, n/2, ARRAY_A);
        parallelMergeSort(A+n/2, B+n/2, n-n/2, ARRAY_A); 
        cilk_sync;
        parallelMerge(B, A, A+n/2, n/2, n-n/2);
    }
}
#endif

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

#if DEBUG == 1
    printArray(n);
#endif

    /// Run the program
    time1 = __cilkview_getticks();

    // Run Merge Sort
#if NOALLOC == 1
    parallelMergeSort(inputArray, outputArray, n, ARRAY_A);
#else
    parallelMergeSort(outputArray, inputArray, n);
#endif

    time2 = __cilkview_getticks();

    par_time = time2-time1;
    cout << "\nParallel Merge Sort took " << par_time << " milliseconds." << endl;

#if DEBUG == 2 
    printArray(n);
#endif

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
#if NOALLOC == 1
        outputFile << inputArray[i] << endl;
#else 
        outputFile << outputArray[i] << endl;
#endif
    }

    outputFile.close();

    freeMem();

    return 0;
}


