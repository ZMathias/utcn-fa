#ifndef LAB05_HASH_TABLE_H
#define LAB05_HASH_TABLE_H

#include "Profiler.h"
#include "commandline.h"

namespace lab05
{
    /**
     * @brief Demo code for the sorting algorithms
     *
     * @param size number of elements to demonstrate on
     */
    void demonstrate(int size);

    /**
     * @brief Performance analysis for the sorting algorithms
     *
     * @param profiler profiler to use
     * @param whichCase one of AVERAGE, BEST or WORST cases
     */
    void performance(Profiler& profiler, AnalysisCase whichCase);
}

#endif //LAB05_HASH_TABLE_H