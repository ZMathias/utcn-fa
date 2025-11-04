#include "direct_sort.h"

#include <climits>

#include "catch2.hpp"

#include <iostream>
#include <bits/stl_multiset.h>

/*
 * Author: Zágoni Mátyás Elemér
 * Group: 30421
 * Lab: FA
 * Observations and analysis regarding the charts and its implications:
 * --------------- AVERAGE CASE ---------------
 * In the average case we can clearly see that bubble sort does the most amount of assignments
 * Following that is insertion and binary insertion sort a bit more efficient but it still fits to a quadratic growth curve
 * Much more efficient in this manner is the selection sort that consistently does around ~3x the number of assignments
 * in terms of the input datas size.
 * It however does the most comparisons, so where assignments are costly, selection sort is preferred
 * Where comparisons are expensive, binary insertion sort does the least amount of comparisons
 *
 * For total operations, we can see that all four algorithms are of complexity O(n^2) and
 * the worst offender is bubble sort
 * more efficient algorithms are selection and insertion sorting algorithms that perform around the same total number of operations
 *
 * binary insertion sort does the least amount comparisons out of the four
 * --------------- BEST CASE ---------------
 * In the best case we can see that bubble sort and selection sort does no assignments
 * while insertion sort algorithms including the binary version are growing linearly in
 * in terms of the size of the input data. This happens because the key
 * value is always copied before the loop even runs
 *
 *
 * when analyzing comparisons we can see that selection sort is quadratic in comparison
 * complexity even in the best case since it has to search
 * for the minimum even if the array is sorted
 * all other algorithms are much much more efficient in this case
 *
 * the same is true for total number of operations in the best case
 * --------------- WORST CASE ---------------
 * we can see clearly that in the worst case bubble sort does the most assignments
 * and somewhat more efficient are the insertion sort based algorithms
 *
 * in the worst case we can see that bubble sort, insertion sort and selection sort do
 * a similar amount of comparisons while binary insertion sort does much
 * better
 *
 * in terms of total operations all algorithms are quadratic in complexity
 * and selection sort is quadratic in all cases
 */



namespace lab01
{

void bubbleSort(int* values, int n, Operation* opAsg, Operation* opCmp)
{
    bool ok;
    do {
        ok = false;
        for (int i = 0 ; i < n - 1; i++) {
            if (opCmp) opCmp->count();
            if (values[i] > values[i + 1]) {
                if (opAsg) opAsg->count(3);
                std::swap(values[i], values[i+1]);
                ok = true;
            }
        }

        n--;
    } while (ok);
}

void selectionSort(int* values, int n, Operation* opAsg, Operation* opCmp)
{
    // selection sort always tries to search for the minimum in the
    // array and swap it with the first element of the unsorted subarray
    // expanding the sorted subarray by doing so

    // the search window
    for (int i = 0; i < n - 1; i++) {

        int min_idx = i;
        // search for the smallest value in the array
        for (int j = i + 1; j < n; j++) {
            if (opCmp) opCmp->count();
            if (values[j] < values[min_idx]) {
                min_idx = j;
            }
        }

        if (min_idx != i) {
            if (opAsg) opAsg->count(3);
            std::swap(values[i], values[min_idx]);
        }
    }
}

void insertionSort(int* values, int n, Operation* opAsg, Operation* opCmp)
{
    // we try to insert a value into a sorted subarray defined by i, which is at the interval values[0, i)
    // we then shift everything right each step and find the corresponding place for the key in the subarray
    // 1, 3, 5, 7, 0, 4
    for (int i = 1; i < n; i++) {
        if (opAsg) opAsg->count();
        int key = values[i];

        int j = i - 1;

        while (j >= 0 && values[j] > key) {
            // we shift the values right
            if (opCmp) opCmp->count();
            if (opAsg) opAsg->count();
            values[j + 1] = values[j];
            j--;
        }
        if (opCmp) opCmp->count(2); // we always undercount by one because the body doesn't execute when the condition fails but comparison still occurred, this compensates
        // and we have to add another comparison because of the if statement below
        if (values[j + 1] != key) {
            values[j + 1] = key;
            if (opAsg) opAsg->count();
        }
    }
}

int binary_search(int* values, int left, int right, int key, Operation* opAsg, Operation* opCmp) {
    // we search for the insertion index of the key using binary search

    if (left > right) {
        return left;
    }

    int mid = (left + right) / 2;

    if (opCmp) opCmp->count();
    if (key < values[mid]) {
        return binary_search(values, left, mid - 1, key, opAsg, opCmp);
    }

    return binary_search(values, mid + 1, right, key, opAsg, opCmp);
}

void binaryInsertionSort(int* values, int n, Operation* opAsg, Operation* opCmp)
{
    for (int i = 1; i < n; i++) {
        if (opAsg) opAsg->count();
        int key = values[i];

        int x = binary_search(values, 0, i - 1, key, opAsg, opCmp);
        // we need to shift all the values
        for (int j = i; j > x; j--) {
            if (opAsg) opAsg->count();
            values[j] = values[j - 1];
        }

        if (opCmp) opCmp->count();
        if (values[x] != key) { // this can be left as a redundant assignment but costs more comparisons
            if (opAsg) opAsg->count();
            values[x] = key;
        }
    }
}

void demonstrate(int size)
{
    auto values = new int[size];
    auto values_orig = new int[size];
    FillRandomArray(values_orig, size, 1, 20, false, false);
    CopyArray(values, values_orig, size);

    printf("Original array: ");
    for (int i = 0; i < size; i++) {
        printf("%i ", values[i]);
    }
    bubbleSort(values, size);

    printf("\nSorted using bubble sort: ");
    for (int i = 0; i < size; i++) {
        printf("%i ", values[i]);
    }

    CopyArray(values, values_orig, size); /// restore the array

    selectionSort(values, size);
    printf("\nSorted using selection sort: ");
    for (int i = 0; i < size; i++) {
        printf("%i ", values[i]);
    }

    CopyArray(values, values_orig, size); // restore the array
    insertionSort(values, size);
    printf("\nSorted using insertion sort: ");
    for (int i = 0; i < size; i++) {
        printf("%i ", values[i]);
    }

    CopyArray(values, values_orig, size);
    binaryInsertionSort(values, size);
    printf("\nSorted using insertion sort: ");
    for (int i = 0; i < size; i++) {
        printf("%i ", values[i]);
    }

    putchar('\n');

    delete[] values;
    delete[] values_orig;
}

TEST_CASE("bubbleSort") {
    printf("Testing bubbleSort on randomized input of size 40000...\n");
    int data[100000];
    FillRandomArray(data, 40000);

    bubbleSort(data, 40000);
    REQUIRE( IsSorted(data, 40000) );
}

TEST_CASE("selectionSort") {
    printf("Testing selectionSort on randomized input of size 40000...\n");
    int data[100000];
    FillRandomArray(data, 40000);

    selectionSort(data, 40000);

    // assert that it is sorted
    REQUIRE( IsSorted(data, 40000) );
}

TEST_CASE("insertionSort") {
    printf("Testing insertionSort on randomized input of size 40000...\n");
    int data[100000];
    FillRandomArray(data, 40000);

    insertionSort(data, 40000);

    // assert that it is sorted
    REQUIRE( IsSorted(data, 40000) );
}

TEST_CASE("binaryInsertionSort") {
    printf("Testing binaryInsertionSort on randomized input of size 40000...\n");
    int data[100000];
    FillRandomArray(data, 40000);

    binaryInsertionSort(data, 40000);

    // assert that it is sorted
    REQUIRE( IsSorted(data, 40000) );
}

void performance(Profiler& profiler, AnalysisCase whichCase)
{
    switch (whichCase) {
        case AVERAGE: {
            int values[10000], values_to_be_sorted[10000];
            for (int i = 0; i < 5; i++) {
                for (int n = 100; n <= 10000; n += 100) {
                    printf("i: %i with n: %i\n", i, n);
                    FillRandomArray(values, n);
                    Operation bubbleAsg = profiler.createOperation("bubbleAsg", n);
                    Operation bubbleCmp = profiler.createOperation("bubbleCmp", n);

                    Operation selectionAsg = profiler.createOperation("selectionAsg", n);
                    Operation selectionCmp = profiler.createOperation("selectionCmp", n);

                    Operation insertionAsg = profiler.createOperation("insertionAsg", n);
                    Operation insertionCmp = profiler.createOperation("insertionCmp", n);

                    Operation binInsertionAsg = profiler.createOperation("binInsertionAsg", n);
                    Operation binInsertionCmp = profiler.createOperation("binInsertionCmp", n);

                    CopyArray(values_to_be_sorted, values, n);
                    bubbleSort(values_to_be_sorted, n, &bubbleAsg, &bubbleCmp);

                    CopyArray(values_to_be_sorted, values, n);
                    selectionSort(values_to_be_sorted, n, &selectionAsg, &selectionCmp);

                    CopyArray(values_to_be_sorted, values, n);
                    insertionSort(values_to_be_sorted, n, &insertionAsg, &insertionCmp);

                    CopyArray(values_to_be_sorted, values, n);
                    binaryInsertionSort(values_to_be_sorted, n, &binInsertionAsg, &binInsertionCmp);
                }
            }

            profiler.addSeries("bubbleOp", "bubbleAsg", "bubbleCmp");
            profiler.addSeries("selectionOp", "selectionAsg", "selectionCmp");
            profiler.addSeries("insertionOp", "insertionAsg", "insertionCmp");
            profiler.addSeries("binInsertionOp", "binInsertionAsg", "binInsertionCmp");
            profiler.createGroup("Assignments", "bubbleAsg", "selectionAsg", "insertionAsg", "binInsertionAsg");
            profiler.createGroup("Comparisons", "bubbleCmp", "selectionCmp", "insertionCmp", "binInsertionCmp");
            profiler.createGroup("Operations", "bubbleOp", "selectionOp", "insertionOp", "binInsertionOp");
            profiler.divideValues("bubbleAsg", 5);
            profiler.divideValues("bubbleCmp", 5);
            profiler.divideValues("selectionAsg", 5);
            profiler.divideValues("selectionCmp", 5);
            profiler.divideValues("insertionAsg", 5);
            profiler.divideValues("insertionCmp", 5);
            profiler.divideValues("binInsertionAsg", 5);
            profiler.divideValues("binInsertionCmp", 5);
            break;
        }
        case BEST: {
            int values[10000], values_to_be_sorted[10000];
            for (int i = 0; i < 5; i++) {
                for (int n = 100; n <= 10000; n+=100) {
                    printf("i: %i with n: %i\n", i, n);
                    FillRandomArray(values, n, 10, 50000, false, ASCENDING);
                    Operation bubbleAsg = profiler.createOperation("bubbleAsg", n);
                    Operation bubbleCmp = profiler.createOperation("bubbleCmp", n);

                    Operation selectionAsg = profiler.createOperation("selectionAsg", n);
                    Operation selectionCmp = profiler.createOperation("selectionCmp", n);

                    Operation insertionAsg = profiler.createOperation("insertionAsg", n);
                    Operation insertionCmp = profiler.createOperation("insertionCmp", n);

                    Operation binInsertionAsg = profiler.createOperation("binInsertionAsg", n);
                    Operation binInsertionCmp = profiler.createOperation("binInsertionCmp", n);

                    CopyArray(values_to_be_sorted, values, n);
                    bubbleSort(values_to_be_sorted, n, &bubbleAsg, &bubbleCmp);

                    CopyArray(values_to_be_sorted, values, n);
                    selectionSort(values_to_be_sorted, n, &selectionAsg, &selectionCmp);

                    CopyArray(values_to_be_sorted, values, n);
                    insertionSort(values_to_be_sorted, n, &insertionAsg, &insertionCmp);

                    CopyArray(values_to_be_sorted, values, n);
                    binaryInsertionSort(values_to_be_sorted, n, &binInsertionAsg, &binInsertionCmp);
                }
            }

            profiler.addSeries("bubbleOp", "bubbleAsg", "bubbleCmp");
            profiler.addSeries("selectionOp", "selectionAsg", "selectionCmp");
            profiler.addSeries("insertionOp", "insertionAsg", "insertionCmp");
            profiler.addSeries("binInsertionOp", "binInsertionAsg", "binInsertionCmp");
            profiler.createGroup("Assignments", "bubbleAsg", "selectionAsg", "insertionAsg", "binInsertionAsg");
            profiler.createGroup("Comparisons", "bubbleCmp", "selectionCmp", "insertionCmp", "binInsertionCmp");
            profiler.createGroup("Operations", "bubbleOp", "selectionOp", "insertionOp", "binInsertionOp");
            profiler.divideValues("bubbleAsg", 5);
            profiler.divideValues("bubbleCmp", 5);
            profiler.divideValues("selectionAsg", 5);
            profiler.divideValues("selectionCmp", 5);
            profiler.divideValues("insertionAsg", 5);
            profiler.divideValues("insertionCmp", 5);
            profiler.divideValues("binInsertionAsg", 5);
            profiler.divideValues("binInsertionCmp", 5);
            break;
        }
        case WORST: {
            int values[10000], values_to_be_sorted[10000];
            for (int i = 0; i < 5; i++) {
                for (int n = 100; n <= 10000; n+=100) {
                    printf("i: %i with n: %i\n", i, n);
                    FillRandomArray(values, n, 10, 50000, false, DESCENDING);
                    Operation bubbleAsg = profiler.createOperation("bubbleAsg", n);
                    Operation bubbleCmp = profiler.createOperation("bubbleCmp", n);

                    Operation selectionAsg = profiler.createOperation("selectionAsg", n);
                    Operation selectionCmp = profiler.createOperation("selectionCmp", n);

                    Operation insertionAsg = profiler.createOperation("insertionAsg", n);
                    Operation insertionCmp = profiler.createOperation("insertionCmp", n);

                    Operation binInsertionAsg = profiler.createOperation("binInsertionAsg", n);
                    Operation binInsertionCmp = profiler.createOperation("binInsertionCmp", n);

                    CopyArray(values_to_be_sorted, values, n);
                    bubbleSort(values_to_be_sorted, n, &bubbleAsg, &bubbleCmp);

                    /*create the worst case by using a sorted array but with the minimum being at the start and everything is shifted right
                    like 1, 5, 4, 3, 2 */
                    int max = values[0];
                    for (int x = 0; x < n - 1; x++) {
                        values_to_be_sorted[x] = values[x + 1];
                    }
                    values_to_be_sorted[n - 1] = max;
                    selectionSort(values_to_be_sorted, n, &selectionAsg, &selectionCmp);

                    CopyArray(values_to_be_sorted, values, n);
                    insertionSort(values_to_be_sorted, n, &insertionAsg, &insertionCmp);

                    CopyArray(values_to_be_sorted, values, n);
                    binaryInsertionSort(values_to_be_sorted, n, &binInsertionAsg, &binInsertionCmp);
                }
            }

            profiler.addSeries("bubbleOp", "bubbleAsg", "bubbleCmp");
            profiler.addSeries("selectionOp", "selectionAsg", "selectionCmp");
            profiler.addSeries("insertionOp", "insertionAsg", "insertionCmp");
            profiler.addSeries("binInsertionOp", "binInsertionAsg", "binInsertionCmp");
            profiler.createGroup("Assignments", "bubbleAsg", "selectionAsg", "insertionAsg", "binInsertionAsg");
            profiler.createGroup("Comparisons", "bubbleCmp", "selectionCmp", "insertionCmp", "binInsertionCmp");
            profiler.createGroup("Operations", "bubbleOp", "selectionOp", "insertionOp", "binInsertionOp");
            profiler.divideValues("bubbleAsg", 5);
            profiler.divideValues("bubbleCmp", 5);
            profiler.divideValues("selectionAsg", 5);
            profiler.divideValues("selectionCmp", 5);
            profiler.divideValues("insertionAsg", 5);
            profiler.divideValues("insertionCmp", 5);
            profiler.divideValues("binInsertionAsg", 5);
            profiler.divideValues("binInsertionCmp", 5);
            break;
        }
    }
}

void benchmark(Profiler& profiler, AnalysisCase whichCase)
{
    // TODO: Write your benchmarks here, as explained in the readme
    // You should use profiler.startTimer/stopTimer for measuring and plotting measured time
}

} // namespace lab01
