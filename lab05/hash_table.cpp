#include "hash_table.h"
#include "catch2.hpp"
#include <iostream>

namespace lab05
{
    HashMapT* create_hashmap() {
        auto* h_map = new HashMapT;
        h_map->arr = new Entry*[PRIME_DIVIDER]; // memory management is deferred to the user
        h_map->size = PRIME_DIVIDER;
        memset(h_map->arr, 0, PRIME_DIVIDER); // zero memory

        return h_map;
    }

    void delete_hashmap(HashMapT* h_map) {
        for (int i = 0; i < h_map->size; i++) {
            delete h_map->arr[i]; // delete each entry
        }

        delete[] h_map->arr;
        delete h_map; // delete the array itself
    }

    int hash(const int key, const int idx) {
        constexpr int c1 = 1;
        constexpr int c2 = 1;
        return (key + c1*idx + c2*idx*idx) % PRIME_DIVIDER;
    }

    Entry** search(HashMapT* h_map, const int key) {
        Entry** arr = h_map->arr;

        for (int i = 0; i < h_map->size; i++) {
            const int h = hash(key, i);
            if (arr[h] == nullptr) {
                return nullptr; // empty slot found, no need to continue
            }

            if (arr[h] == TOMBSTONE) {
                continue; // can not reference deleted slots, must continue
            }

            if (arr[h]->id == key) {
                return arr + h; // key found, return pointer
            }
        }

        return nullptr; // none found, nullptr signifies miss
    }

    bool insert(HashMapT* h_map, Entry* value) {
        if (value == nullptr) {
            return false;
        }

        // we must first find the empty slot into which we insert
        Entry** arr = h_map->arr;
        int first_tombstone = -1;
        for (int i = 0; i < h_map->size; i++) {
            const int h = hash(value->id, i);

            if (arr[h] == nullptr) {
                // free slot found, check if no tombstone was found previously and insert
                if (first_tombstone != -1) {
                    arr[first_tombstone] = value;
                    return true;
                }

                arr[h] = value;
                return true;
            }

            if (arr[h] == TOMBSTONE) {
                if (first_tombstone == -1) {
                    first_tombstone = h;
                }
            }
        }

        return false; // no slot found
    }

    bool delete_entry(HashMapT* h_map, const int key) {
        Entry** index = search(h_map, key);
        if (index == nullptr) {
            return false; // not in table
        }

        delete *index; // delete structure pointed to by index
        *index = TOMBSTONE;

        return true;
    }



    void demonstrate(int size) {
        printf("demonstrate!\n");
    }

    void performance(Profiler& profiler, AnalysisCase whichCase) {
        printf("performance!\n");
    }
}
