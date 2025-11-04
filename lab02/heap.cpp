#include "heap.h"
#include <climits>
#include "catch2.hpp"
#include <iostream>

/*
 * ---------- AVERAGE CASE ----------
 * In the average case, we can see that
 * the iterative and recursive versions of insertion sort
 * are head to head in terms of number of operations.
 * In normal operation, both perform about the same
 * Heap sort however is much faster because it is O(nlogn) compared to O(n^2) for insertion sort.
 *
 * When looking at the heap building algorithms, we can clearly see that the bottom-up fashion
 * is much faster because of its linear complexity compared to the O(nlogn) complexity of the top-down version.
 * Bottom-up is consistently more efficient in assignment, comparisons and therefore total number of operations.
 * ---------- WORST CASE ----------
 * In the worst case, we can make out the performance gap between the heap building function even easier.
 * In the case of an ascendingly sorted input array, top-down build heap must bubble up elements almost all the time
 * to the root, therefore it is much slower in worst case than bottom-up build heap that is consistently O(n) in the worst case
 * aswell.
 *
 * ---------- BENCHMARK AVG ----------
 * In terms of running time, they are practically identical but a trend can be observed
 * over many runs that the recursive one is smaller by a very small margin
 */

namespace lab02
{
    void printArray(const int *arr, int size) {
        for (int i = 0; i < size; i++) {
            printf("%d ", arr[i]);
        }
        putchar('\n');
    }

    void iterativeSort(int* values, int n, Operation* opAsg, Operation* opCmp)
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

            if (i >= 0) {
                if (opCmp) opCmp->count();
            }

            values[j + 1] = key;
            if (opAsg) opAsg->count();
        }
    }

    void recursiveInsertionSort(int *values, int n, int x, Operation* opAsg, Operation* opCmp) {
        // we want to implement insertion sort recursively
        // the basic principle will be to try and insert the next value from the
        // limit x into the sorted subarray to the left of the limit
        // we then recursively call to insert the next value
        // we stop when we reach end of array
        if (x >= n) { // if we reach the end
            return;
        }

        // insert the first value into the sorted subarray
        if (opAsg) opAsg->count();
        const int key = values[x];
        int i = x - 1;
        while (i >= 0 && values[i] > key) { // find the place for the key
            if (opCmp) opCmp->count();
            if (opAsg) opAsg->count();
            // shift the values right
            values[i + 1] = values[i];
            i--;
        }

        if (i >= 0) {
            if (opCmp) opCmp->count(); // count comparison missed when loop exits because values[i] is no longer bigger than key
        }
        values[i + 1] = key; // place the key into the corresponding position
        if (opAsg) opAsg->count();

        // recursively trigger the insertion of the next element as well
        recursiveInsertionSort(values, n, x + 1, opAsg, opCmp);
    }

    void recursiveSort(int* values, int n, Operation* opAsg, Operation* opCmp) // wrapper function calling the actual implementation
    {
        recursiveInsertionSort(values, n, 1, opAsg, opCmp);
    }

    constexpr int parent(const int i) {
        return (i - 1) / 2;
    }

    constexpr int left(const int i) {
        return 2 * i + 1;
    }

    constexpr int right (const int i) {
        return 2 * i + 2;
    }

    void swim(int* values, int n, int i, Operation* opAsg, Operation* opCmp) {
        // we check if it is smaller than parent, if so swap and recursively go up
        if (i < 0 || i >= n) {
            return; // check out of bounds
        }

        const int par_idx = parent(i);
        if (opCmp && par_idx >= 0) opCmp->count();
        if (par_idx >= 0 && values[i] > values[par_idx]) {
            std::swap(values[par_idx], values[i]);
            if (opAsg) opAsg->count(3);
            swim(values, n, par_idx, opAsg, opCmp);
        }
    }

    void buildHeap_TopDown(int* values, int n, Operation* opAsg, Operation* opCmp)
    {
        for (int i = 1; i < n; i++) {
            swim(values, i + 1, i, opAsg, opCmp);
        }
    }

    void heapify(int* values, const int n, const int i, Operation* opAsg, Operation* opCmp) {
        if (i >= n) {
            return; // if index is out of bounds
        }

        const int lc_idx = left(i);
        const int rc_idx = right(i);


        int max_idx = i;
        if (opCmp && lc_idx < n) opCmp->count(); // count the comparison between values in the next line
        if (lc_idx < n && values[lc_idx] > values[max_idx]) {
            max_idx = lc_idx;
        }

        if (opCmp && rc_idx < n) opCmp->count();
        if (rc_idx < n && values[rc_idx] > values[max_idx]) {
            max_idx = rc_idx;
        }

        if (max_idx != i) { // if children are smaller, swap
            std::swap(values[max_idx], values[i]);
            if (opAsg) opAsg->count(3); // 3 assignments for swap
            heapify(values, n, max_idx, opAsg, opCmp);
        }
    }

    void buildHeap_BottomUp(int* values, int n, Operation* opAsg, Operation* opCmp)
    {
        // bottom-up heap building happens from the first non-leaf node n / 2 - 1
        // and runs heapify for each node that check if the parent
        // is really the smallest of its children
        for (int i = n / 2 - 1; i >=0; i--) {
            heapify(values, n, i, opAsg, opCmp);
        }
    }

    int extractMax(int* values, int n, Operation* opAsg, Operation* opCmp) {
        const int max = values[0];
        std::swap(values[0], values[n - 1]);
        heapify(values, n - 1, 0, opAsg, opCmp);
        return max;
    }

    void maxAtEnd(int* values, int n, Operation* opAsg, Operation* opCmp) { // function only for sorting to avoid an assignment at every extraction
        std::swap(values[0], values[n - 1]);
        if (opAsg) opAsg->count(3);
        heapify(values, n - 1, 0, opAsg, opCmp);
    }

    void heapSort(int* values, int n, Operation* opAsg, Operation* opCmp)
    {
        // we build a heap from the values array
        buildHeap_BottomUp(values, n, opAsg, opCmp);
        // then we extract a value and put it at the end, therefore we sort ascending with a max-heap in place
        for (int i = n; i > 1; i--) { // we only need to do it n - 1 times since a heap of size 1 is trivial
            maxAtEnd(values, i, opAsg, opCmp); // this puts the value at the end and restores heap property
        }
    }

    void demonstrate(int size)
    {
        int *values = new int[size];
        int *values_to_be_processed = new int[size];
        FillRandomArray(values, size, 1, 10);
        CopyArray(values_to_be_processed, values, size);

        printf("Array before sorting: ");
        printArray(values_to_be_processed, size);

        recursiveSort(values_to_be_processed, size);

        printf("After sorting with recursive insertion sort: ");
        printArray(values_to_be_processed, size);

        CopyArray(values_to_be_processed, values, size);
        heapSort(values_to_be_processed, size);
        printf("After sorting with heapsort: ");
        printArray(values_to_be_processed, size);

        CopyArray(values_to_be_processed, values, size);
        buildHeap_BottomUp(values_to_be_processed, size);
        printf("Array after building heap using bottom-up: ");
        printArray(values_to_be_processed, size);

        CopyArray(values_to_be_processed, values, size);
        buildHeap_TopDown(values_to_be_processed, size);
        printf("Array after building heap using top-down: ");
        printArray(values_to_be_processed, size);

        delete[] values;
        delete[] values_to_be_processed;
    }

    bool IsMaxHeap(int* values, int n) {
        const int limit = n / 2;
        for (int i = 0; i < limit; i++) {
            const int ldc_idx = left(i);
            const int rdc_idx = right(i);

            if (ldc_idx < n && values[i] < values[ldc_idx]) {
                return false;
            }

            if (rdc_idx < n && values[i] < values[rdc_idx]) {
                return false;
            }
        }

        return true;
    }

    TEST_CASE("recursive InsertionSort") {
        constexpr int size = 40000;
        printf("Running recursive insertionSort test for %d elements...\n", size);
        int data[size];
        FillRandomArray(data, size);
        recursiveSort(data, size);
        REQUIRE( IsSorted(data, size) );
    }

    TEST_CASE("heapSort") {
        constexpr int size = 40000;
        printf("Running heapSort for %d elements...\n", size);
        int data[size];
        FillRandomArray(data, size);
        heapSort(data, size);
        REQUIRE( IsSorted(data, size) );
    }

    TEST_CASE("buildHeap_BottomUp") {
        constexpr int size = 40000;
        printf("Running bottom-up build heap for %d elements...\n", size);
        int data[size];
        FillRandomArray(data, size);
        buildHeap_BottomUp(data, size);
        REQUIRE( IsMaxHeap(data, size) );
    }

    TEST_CASE("buildHeap_TopDown") {
        constexpr int size = 40000;
        printf("Running top-down build heap for %d elements...\n", size);
        int data[size];
        FillRandomArray(data, size);
        buildHeap_TopDown(data, size);
        REQUIRE( IsMaxHeap(data, size) );
    }

    void performance(Profiler& profiler, AnalysisCase whichCase)
    {
        switch (whichCase) {
        case AVERAGE:
            {
                int values[10000], values_to_be_processed[10000];
                for (int i = 0; i < 5; i++) {
                    for (int n = 100; n <= 10000; n += 100) {
                        printf("i(%d): %d\n", i + 1, n);
                        FillRandomArray(values, n);
                        Operation iterAsg = profiler.createOperation("iterAsg", n);
                        Operation iterCmp = profiler.createOperation("iterCmp", n);

                        Operation recAsg = profiler.createOperation("recAsg", n);
                        Operation recCmp = profiler.createOperation("recCmp", n);

                        Operation heapAsg = profiler.createOperation("heapAsg", n);
                        Operation heapCmp = profiler.createOperation("heapCmp", n);

                        Operation buAsg = profiler.createOperation("buAsg", n);
                        Operation buCmp = profiler.createOperation("buCmp", n);

                        Operation tdAsg = profiler.createOperation("tdAsg", n);
                        Operation tdCmp = profiler.createOperation("tdCmp", n);

                        CopyArray(values_to_be_processed, values, n);
                        iterativeSort(values_to_be_processed, n, &iterAsg, &iterCmp);

                        CopyArray(values_to_be_processed, values, n);
                        recursiveSort(values_to_be_processed, n, &recAsg, &recCmp);

                        CopyArray(values_to_be_processed, values, n);
                        heapSort(values_to_be_processed, n, &heapAsg, &heapCmp);

                        CopyArray(values_to_be_processed, values, n);
                        buildHeap_BottomUp(values_to_be_processed, n, &buAsg, &buCmp);

                        CopyArray(values_to_be_processed, values, n);
                        buildHeap_TopDown(values_to_be_processed, n, &tdAsg, &tdCmp);
                    }
                }

                profiler.addSeries("iterOp", "iterAsg", "iterCmp");
                profiler.addSeries("recOp", "recAsg", "recCmp");
                profiler.addSeries("heapOp", "heapAsg", "heapCmp");
                profiler.addSeries("buOp", "buAsg", "buCmp");
                profiler.addSeries("tdOp", "tdAsg", "tdCmp");
                profiler.createGroup("Assignments", "iterAsg", "recAsg", "heapAsg");
                profiler.createGroup("Comparisons", "iterCmp", "recCmp", "heapCmp");
                profiler.createGroup("Operations", "iterOp", "recOp", "heapOp");
                profiler.createGroup("Heap build asg", "buAsg", "tdAsg");
                profiler.createGroup("Heap build cmp", "buCmp", "tdCmp");
                profiler.createGroup("Heap build ops", "buOp", "tdOp");

                profiler.divideValues("iterAsg", 5);
                profiler.divideValues("iterCmp", 5);

                profiler.divideValues("recAsg", 5);
                profiler.divideValues("recCmp", 5);

                profiler.divideValues("heapAsg", 5);
                profiler.divideValues("heapCmp", 5);

                profiler.divideValues("buAsg", 5);
                profiler.divideValues("buCmp", 5);

                profiler.divideValues("tdAsg", 5);
                profiler.divideValues("tdCmp", 5);
                break;
            }
        case WORST:
            {
                // we perform the worst case analysis for the build heap methods showcased
                int values[10000], values_to_be_processed[10000];
                for (int i = 0; i < 5; i++) {
                    for (int n = 100; n <= 10000; n += 100) {
                        printf("i(%d): %d\n", i + 1, n);
                        FillRandomArray(values, n, 10, 50000, false, ASCENDING);

                        Operation buAsg = profiler.createOperation("buAsg", n);
                        Operation buCmp = profiler.createOperation("buCmp", n);

                        Operation tdAsg = profiler.createOperation("tdAsg", n);
                        Operation tdCmp = profiler.createOperation("tdCmp", n);

                        CopyArray(values_to_be_processed, values, n);
                        buildHeap_BottomUp(values_to_be_processed, n, &buAsg, &buCmp);

                        CopyArray(values_to_be_processed, values, n);
                        buildHeap_TopDown(values_to_be_processed, n, &tdAsg, &tdCmp);
                    }
                }

                profiler.addSeries("buOp", "buAsg", "buCmp");
                profiler.addSeries("tdOp", "tdAsg", "tdCmp");
                profiler.createGroup("Heap build asg", "buAsg", "tdAsg");
                profiler.createGroup("Heap build cmp", "buCmp", "tdCmp");
                profiler.createGroup("Heap build ops", "buOp", "tdOp");

                profiler.divideValues("buAsg", 5);
                profiler.divideValues("buCmp", 5);

                profiler.divideValues("tdAsg", 5);
                profiler.divideValues("tdCmp", 5);
                break;
            }
        default:
            printf("Only average and worst case are required for this exercise!\n");
            break;
        }
    }

    void benchmark(Profiler& profiler, AnalysisCase whichCase)
    {
        int values[10000], values_to_be_sorted[10000];
        for (int n = 1000; n <= 4000; n += 10) {
            printf("size n(%d)\n", n);
            FillRandomArray(values, n);

            profiler.startTimer("iterSort", n);
            for (int i = 0; i < 300; i++) { // execution time measured across many runs
                CopyArray(values_to_be_sorted, values, n);
                iterativeSort(values_to_be_sorted, n);
            }
            profiler.stopTimer("iterSort", n);

            profiler.startTimer("recSort", n);
            for (int i = 0; i < 300; i++) { // execution time measured across many runs
                CopyArray(values_to_be_sorted, values, n);
                recursiveSort(values_to_be_sorted, n);
            }
            profiler.stopTimer("recSort", n);
        }
        profiler.createGroup("runTimes", "iterSort", "recSort");
        profiler.showReport();
    }

} // namespace lab02
