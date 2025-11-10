#ifndef LAB05_HASH_TABLE_H
#define LAB05_HASH_TABLE_H

#include "Profiler.h"
#include "commandline.h"

namespace lab05
{
    // required default data structure from lab-assignment
    struct Entry {
        int id;
        char name[30];
    };

    constexpr int PRIME_DIVIDER = 10007;
    #define TOMBSTONE (reinterpret_cast<Entry*>(0xDEADBEEF))

    struct HashMapT
    {
        Entry** arr;
        int size;
    };

    /**
     *
     * @return Returns a pointer to the data structure of type HashMapT
     */
    HashMapT* create_hashmap();
    void delete_hashmap(HashMapT* h_map, int size);

    /**
     * The hash function implements modulo hashing using a prime number.
     * @param key The key to hash.
     * @param idx The search iteration the hash is called in.
     * @return hashed index
     */
    int hash(int key, int idx = 0);

    Entry** search(HashMapT* h_map, int key);

    bool insert(HashMapT* h_map, Entry* value);

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