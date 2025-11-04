#include "quick_sort.h"

#include "catch2.hpp"

#include <iostream>

/*
 * ---------- AVG CASE ----------
 * In the average case we had to compare three sorting algorithms: quicksort, hybrid quicksort and heapsort
 * The goal was to demonstrate that even though heap sort is optimal in theory, quick sort still outperforms
 * it in practice. This is because quick sort is cache friendly and the CPU can take advantage of various
 * memory optimizations when algorithms are running in a cache friendly regime. Both show a complexity of O(nlogn)
 * But as the charts prove, quicksort and hybrid quicksort are faster by significant amount.
 * Comparing the operations between hybrid quicksort and quicksort shows that hybrid does a bit more operations.
 * This is a bit counter intuitive but expected, since insertion sort is an algorithm with a much bigger complexity O(n^2)
 * and replacing significant amounts of the sorting run with a higher complexity algorithm leads to more operations being done in total
 * The speedup comes primarily from the fact that at smaller array sizes, what dominates is the constant factor that we ignore which is
 * a lot smaller for insertion sort.
 *
 * ---------- BEST CASE ----------
 * Best case is pretty special for quick sort because it is not trivially constructed.
 * For a partitioning method such as Lomuto which deterministically picks the last element
 * as the pivot, we should force the algorithm to return a pivot index that is in the
 * middle of the subarray. This is to minimize the recursion depth to ideally O(log n).
 * constructBestCase is given an ascending array and recurses to the last subarrays and interchanges
 * the median with the pivot going upwards in the recursion.
 *
 * ---------- WORST CASE ----------
 * Sadly, in this case, quick sort performs much worse with a complexity of O(n^2)
 * This is achieved by giving it a descending array because it creates unbalanced partitions.
 * The returned pivot indices are always at the extremes of the subarray. This can be improved with
 * Hoare partitioning or randomized pivot selections that are statistically really unlikely to
 * degrade to O(n^2) and basically always keep O(nlogn) complexity.
 *
 * ---------- OPTIMUM THRESHOLD ----------
 * Because of the overhead that quicksort introduces by recursion and function calling and a much larger
 * constant factor that is ignored in Big-O notation, it is actually slower for a trivial amount of elements
 * compared to quadratic sorting algorithms like insertion sort. This yields a newer method by combining the two
 * and finding a threshold value to decide which algorithm to employ. This was done by iterating threshold values
 * from 5 to 50 and running repeated benchmarks on a fixed size array and this yield a chart in the form of a "down
 * arrow". The data is really noisy due to the os conditions imposing unpredictability on the runtime.
 * , therefore we pick the most promising interval from 25 to 45 and run another pass with much more resolution
 * to even out the data. As it seems, the optimal threshold value is 33, confirmed by the graphs.
 *
 * ---------- QSORT vs Hybrid QSORT ----------
 * For a really small amount of elements (up to 33), hybrid quicksort performs better than regular qsort, it especially excels in big arrays.
 * Because insertion sort has a higher complexity, it does more operations for the same input but still performs
 * better because, as previously mentioned, it has a much lower constant factor and that dominates for small sizes of data.
 *
 * ---------- QuickSelect ----------
 * Quick select is a simple algorithm that uses the same partition function to find the k-th smallest element in an array.
 * It is basically a partial quicksort that only sorts the corresponding partitions that the smallest element we are looking for will be in.
 * This yields an O(n) complexity and is quite a good algorithm for finding k-th smallest elements in an unordered array.
 */

namespace lab03
{
    int partition(int* values, int l, int r, Operation* opAsg, Operation* opCmp) {
        int pivot = values[r]; // always pick the last element as pivot
        if (opAsg) opAsg->count();
        int i = l;
        for (int j = l; j < r; j++) {
            if (opCmp) opCmp->count();
            if (values[j] <= pivot) {
                if (i != j) {
                    std::swap(values[i], values[j]);
                    if (opAsg) opAsg->count(3);
                }
                i++;
            }
        }
        std::swap(values[r], values[i]);
        if (opAsg) opAsg->count(3);
        return i;
    }

    void qsort(int* values, int l, int r, Operation* opAsg, Operation* opCmp) {
        if (l >= r) {
            return;
        }

        const int pivot = partition(values, l, r, opAsg, opCmp);

        qsort(values, l, pivot - 1, opAsg, opCmp);
        qsort(values, pivot + 1, r, opAsg, opCmp);
    }

    void quickSort(int* values, int n, Operation* opAsg, Operation* opCmp)
    {
        qsort(values, 0, n - 1, opAsg, opCmp);
    }

    constexpr int left(const int i) {
        return 2 * i + 1;
    }

    constexpr int right (const int i) {
        return 2 * i + 2;
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

    void maxAtEnd(int* values, int n, Operation* opAsg, Operation* opCmp) { // function only for sorting to avoid an assignment at every extraction
        std::swap(values[0], values[n - 1]);
        if (opAsg) opAsg->count(3);
        heapify(values, n - 1, 0, opAsg, opCmp);
    }

    void heapSort(int* values, int n, Operation* opAsg, Operation* opCmp) {
        // we build a heap from the values array
        buildHeap_BottomUp(values, n, opAsg, opCmp);
        // then we extract a value and put it at the end, therefore we sort ascending with a max-heap in place
        for (int i = n; i > 1; i--) { // we only need to do it n - 1 times since a heap of size 1 is trivial
            maxAtEnd(values, i, opAsg, opCmp); // this puts the value at the end and restores heap property
        }
    }

    void insertionSort(int* values, int n, Operation* opAsg, Operation* opCmp) {
        // we try to insert a value into a sorted subarray defined by i, which is at the interval values[0, i)
        // we then shift everything right each step and find the corresponding place for the key in the subarray
        // 1, 3, 5, 7, 0, 4
        for (int i = 1; i < n; i++) {
            if (opAsg) opAsg->count();
            const int key = values[i];

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

    void hb_qsort(int* values, int l, int r, Operation* opAsg, Operation* opCmp, const int threshold) {
        const int n = r - l + 1;

        if (n <= threshold)
            insertionSort(values + l, n, opAsg, opCmp);
        else {
            if (l >= r) {
                return;
            }

            const int pivot = partition(values, l, r, opAsg, opCmp);

            hb_qsort(values, l, pivot - 1, opAsg, opCmp, threshold);
            hb_qsort(values, pivot + 1, r, opAsg, opCmp, threshold);
        }
    }

    void hybridizedQuickSort(int* values, int n, Operation* opAsg, Operation* opCmp, const int threshold)
    {
        hb_qsort(values, 0, n - 1, opAsg, opCmp, threshold);
    }

    int q_select(int* values, int l, int r, int k, Operation* opAsg, Operation* opCmp)
    {
        if (opAsg) opAsg->count();
        const int p = partition(values, l, r, opAsg, opCmp);
        if (p == k) {
            if (opCmp) opCmp->count();
            return values[k];
        }
        if (k < p) {
            if (opCmp) opCmp->count();
            return q_select(values, l, p - 1, k, opAsg, opCmp);
        }
        return q_select(values, p + 1, r, k, opAsg, opCmp);
    }

    int quickSelect(int* values, int n, int k, Operation* opAsg, Operation* opCmp) {
        return q_select(values, 0, n - 1, k, opAsg, opCmp);
    }

    void printArray(const int* values, const int n) {
        for (int i = 0; i < n; i++) {
            printf("%d ", values[i]);
        }
        putchar('\n');
    }

    void demonstrate(int size)
    {
        int values[size], values_to_sort[size];
        FillRandomArray(values, size, 1, 20);
        CopyArray(values_to_sort, values, size);
        printf("Original array: ");
        printArray(values_to_sort, size);

        quickSort(values_to_sort, size);
        printf("After quicksort: ");
        printArray(values_to_sort, size);

        CopyArray(values_to_sort, values, size);
        hybridizedQuickSort(values_to_sort, size);
        printf("After hybrid quicksort: ");
        printArray(values_to_sort, size);

        CopyArray(values_to_sort, values, size);
        heapSort(values_to_sort, size);
        printf("After heapsort: ");
        printArray(values_to_sort, size);

        printf("The 3rd smallest element from that array is (quickselect): %d\n", quickSelect(values, size, 2));
    }

    TEST_CASE("Insertion sort")
    {
        constexpr int size = 40000;
        int data[size];
        FillRandomArray(data, size);
        insertionSort(data, size);
        REQUIRE( IsSorted(data, size) );
    }

    TEST_CASE("Quick sort")
    {
        constexpr int size = 40000;
        printf("Testing quicksort with an input size of %d...\n", size);
        int data[size];
        FillRandomArray(data, size);
        quickSort(data, size);
        REQUIRE( IsSorted(data, size) );
    }

    TEST_CASE("Hybrid quick sort < 33")
    {
        constexpr int size = 10;
        int data1[size];

        FillRandomArray(data1, size);

        hybridizedQuickSort(data1, size);

        REQUIRE( IsSorted(data1, size) );
    }

    TEST_CASE("Hybrid quick sort with big data")
    {
        constexpr int size = 40000;
        int data1[size];

        FillRandomArray(data1, size);

        hybridizedQuickSort(data1, size);

        REQUIRE( IsSorted(data1, size) );
    }

    TEST_CASE("heapsort")
    {
        constexpr int size = 40000;
        printf("Testing heapsort with an input size of %d...\n", size);
        int data[size];
        FillRandomArray(data, size);
        heapSort(data, size);
        REQUIRE( IsSorted(data, size) );
    }

    void constructBestCase(int* values, const int l, const int r) {
        if (l >= r) {
            return;
        }

        const int median = (r + l) / 2;

        // should recursively hold in each half
        constructBestCase(values, l, median - 1);
        constructBestCase(values, median + 1, r);

        std::swap(values[median], values[r]);
    }

    void performance(Profiler& profiler, AnalysisCase whichCase)
    {
        switch (whichCase) {
        case AVERAGE:
            {
                int values[10000], values_to_process[10000];
                for (int i = 0; i < 5; i++) {
                    for (int n = 100; n <= 10000; n += 100) {
                        printf("i(%d): %d\n", i, n);
                        FillRandomArray(values, n);

                        Operation qAsg = profiler.createOperation("qAsg", n);
                        Operation qCmp = profiler.createOperation("qCmp", n);

                        Operation hqAsg = profiler.createOperation("hqAsg", n);
                        Operation hqCmp = profiler.createOperation("hqCmp", n);

                        Operation hAsg = profiler.createOperation("hAsg", n);
                        Operation hCmp = profiler.createOperation("hCmp", n);

                        CopyArray(values_to_process, values, n);
                        quickSort(values_to_process, n, &qAsg, &qCmp);

                        CopyArray(values_to_process, values, n);
                        hybridizedQuickSort(values_to_process, n, &hqAsg, &hqCmp);

                        CopyArray(values_to_process, values, n);
                        heapSort(values, n, &hAsg, &hCmp);
                    }
                }

                profiler.divideValues("qAsg", 5);
                profiler.divideValues("qCmp", 5);

                profiler.divideValues("hqAsg", 5);
                profiler.divideValues("hqCmp", 5);

                profiler.divideValues("hAsg", 5);
                profiler.divideValues("hCmp", 5);


                profiler.addSeries("qOp", "qAsg", "qCmp");
                profiler.addSeries("hqOp", "hqAsg", "hqCmp");
                profiler.addSeries("hOp", "hAsg", "hCmp");

                profiler.createGroup("Assignments", "qAsg", "hqAsg", "hAsg");
                profiler.createGroup("Comparisons", "qCmp", "hqCmp", "hCmp");
                profiler.createGroup("Operations", "qOp", "hqOp", "hOp");
                break;
            }
        case BEST:
            {
                int values[10000], values_to_process[10000];
                for (int i = 0; i < 5; i++) {
                    for (int n = 100; n <= 10000; n += 100) {
                        printf("i(%d): %d\n", i, n);

                        FillRandomArray(values, n, 10, 50000, false, ASCENDING);
                        constructBestCase(values, 0, n - 1);

                        Operation qAsg = profiler.createOperation("qAsg", n);
                        Operation qCmp = profiler.createOperation("qCmp", n);

                        Operation hqAsg = profiler.createOperation("hqAsg", n);
                        Operation hqCmp = profiler.createOperation("hqCmp", n);

                        CopyArray(values_to_process, values, n);
                        quickSort(values_to_process, n, &qAsg, &qCmp);

                        CopyArray(values_to_process, values, n);
                        hybridizedQuickSort(values_to_process, n, &hqAsg, &hqCmp);
                    }
                }

                profiler.divideValues("qAsg", 5);
                profiler.divideValues("qCmp", 5);

                profiler.divideValues("hqAsg", 5);
                profiler.divideValues("hqCmp", 5);

                profiler.addSeries("qOp", "qAsg", "qCmp");
                profiler.addSeries("hqOp", "hqAsg", "hqCmp");

                profiler.createGroup("Assignments", "qAsg", "hqAsg");
                profiler.createGroup("Comparisons", "qCmp", "hqCmp");
                profiler.createGroup("Operations", "qOp", "hqOp");
                break;
            }
        case WORST:
            {
                int values[10000], values_to_process[10000];
                for (int i = 0; i < 5; i++) {
                    for (int n = 100; n <= 10000; n += 100) {
                        printf("i(%d): %d\n", i, n);

                        FillRandomArray(values, n, 10, 50000, false, DESCENDING);

                        Operation qAsg = profiler.createOperation("qAsg", n);
                        Operation qCmp = profiler.createOperation("qCmp", n);

                        Operation hqAsg = profiler.createOperation("hqAsg", n);
                        Operation hqCmp = profiler.createOperation("hqCmp", n);

                        CopyArray(values_to_process, values, n);
                        quickSort(values_to_process, n, &qAsg, &qCmp);

                        CopyArray(values_to_process, values, n);
                        hybridizedQuickSort(values_to_process, n, &hqAsg, &hqCmp);
                    }
                }

                profiler.divideValues("qAsg", 5);
                profiler.divideValues("qCmp", 5);

                profiler.divideValues("hqAsg", 5);
                profiler.divideValues("hqCmp", 5);

                profiler.addSeries("qOp", "qAsg", "qCmp");
                profiler.addSeries("hqOp", "hqAsg", "hqCmp");


                profiler.createGroup("Assignments", "qAsg", "hqAsg");
                profiler.createGroup("Comparisons", "qCmp", "hqCmp");
                profiler.createGroup("Operations", "qOp", "hqOp");
                break;
            }
        default:
            break;
        }
    }

    void benchmark(Profiler& profiler, AnalysisCase whichCase)
    {
        switch (whichCase) {
        case BEST: // we use the best case to find the optimal threshold value for hybrid quicksort
            {
                constexpr int size = 10000;
                int values[size], values_to_process[size];
                FillRandomArray(values, size);

                printf("Finding optimal operation threshold for hybrid quicksort...\n");
                for (int m = 0; m < 5; m++) {
                    FillRandomArray(values, size);
                    for (int t = 10; t <= 50; t++) {
                        printf("t(%d)\n", t);
                        Operation hbAsg = profiler.createOperation("hbAsgT", t);
                        Operation hbCmp = profiler.createOperation("hbCmpT", t);

                        CopyArray(values_to_process, values, size);
                        hybridizedQuickSort(values_to_process, size, &hbAsg, &hbCmp, t);
                    }
                }

                profiler.divideValues("hbAsgT", 5);
                profiler.divideValues("hbCmpT", 5);

                profiler.addSeries("hbOpT", "hbAsgT", "hbCmpT");

                printf("Finding optimal runtime threshold for hybrid quicksort...\n");
                for (int t = 10; t <= 50; t++) { // threshold value
                    profiler.startTimer("hqPerf", t);
                    for (int i = 0; i < 1000; i++) {
                        CopyArray(values_to_process, values, size);
                        hybridizedQuickSort(values_to_process, size, nullptr, nullptr, t);
                    }
                    profiler.stopTimer("hqPerf", t);
                    printf("t(%d)\n", t);
                }

                profiler.createGroup("Time", "hqPerf");
                break;
            }
        default:
            {
                printf("Comparing quicksort and hybrid quicksort running times on array inputs [100, 10000]\n");
                int values[10000], values_to_process[10000];
                for (int n = 100; n <= 10000; n+=100) {
                    FillRandomArray(values, n);

                    profiler.startTimer("qSort", n);
                    for (int i = 0; i < 1000; i++) {
                        printf("n(%d): i(%d)\n", n, i + 1);
                        CopyArray(values_to_process, values, n);
                        quickSort(values_to_process, n);
                    }
                    profiler.stopTimer("qSort", n);

                    profiler.startTimer("hqSort", n);
                    for (int i = 0; i < 1000; i++) {
                        printf("n(%d): i(%d)\n", n, i + 1);
                        CopyArray(values_to_process, values, n);
                        hybridizedQuickSort(values_to_process, n);
                    }
                    profiler.stopTimer("hqSort", n);
                }
                profiler.createGroup("Runtime", "qSort", "hqSort");
                break;
            }
        }
        profiler.showReport();
    }

} // namespace lab03
