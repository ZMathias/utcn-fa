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

    struct Record
    {
        float alpha;
        float avg_effort_f;
        float max_effort_f_avg;
        float avg_effort_nf;
        float max_effort_nf_avg;
    };

    constexpr int HASHMAP_SIZE = 10007;
    #define TOMBSTONE (reinterpret_cast<Entry*>(0xDEADBEEF))

    struct HashMapT
    {
        Entry** arr;
        int size;
        int inserted;
    };

    /**
     * Copies the value of an entry
     */
    Entry* copy_entry(const Entry* val);

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

    Entry** search(const HashMapT* h_map, int key, int* effort = nullptr);

    Entry* generate_mock_data(const int size);

    bool insert(HashMapT* h_map, Entry* value);

    std::vector<int> fill_hashmap(HashMapT *h_map, float desired_fill);

    void print_hashmap(const HashMapT* h_map);
    void print_global_hashmap();

    /**
     * @brief Demo code for the sorting algorithms
     *
     * @param size number of elements to demonstrate on
     */
    void demonstrate(int size);

    /**
     * @brief Performance analysis for the sorting algorithms
     *
     * @param whichCase one of AVERAGE, BEST or WORST cases
     */
    void performance(AnalysisCase whichCase);

    void init_global_hashmap(int size);
    void search_global_hashmap(int id);
    void delete_from_global_hashmap(int id);
    void delete_global_hashmap();
    void insert_global_hashmap(const int id, const char* name);
}

#endif //LAB05_HASH_TABLE_H