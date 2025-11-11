#include "hash_table.h"
#include "catch2.hpp"
#include <iostream>

namespace lab05
{
    HashMapT* global_hmap;

    HashMapT* create_hashmap() {
        auto* h_map = new HashMapT;
        h_map->arr = new Entry*[HASHMAP_SIZE]; // memory management is deferred to the user
        h_map->size = HASHMAP_SIZE;
        memset(h_map->arr, 0, HASHMAP_SIZE * sizeof(Entry*)); // zero memory

        return h_map;
    }

    Entry* copy_entry(Entry* val) {
        Entry* copy = new Entry;

        copy->id = val->id;
        strcpy(copy->name, val->name);

        return copy;
    }

    void delete_hashmap(HashMapT** h_map) {
        for (int i = 0; i < (*h_map)->size; i++) {
            delete (*h_map)->arr[i]; // delete each entry
        }

        delete[] (*h_map)->arr;
        delete *h_map; // delete the array itself
        (*h_map) = nullptr;
    }

    int hash(const int key, const int idx) {
        constexpr int c1 = 1;
        constexpr int c2 = 1;
        return (key + c1*idx + c2*idx*idx) % HASHMAP_SIZE;
    }

    Entry** search(HashMapT* h_map, const int key, int* effort) {
        Entry** arr = h_map->arr;

        for (int i = 0; i < h_map->size; i++) {
            const int h = hash(key, i);
            if (effort) (*effort)++;
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
                    // NOLINTNEXTLINE
                    arr[first_tombstone] = copy_entry(value);
                    return true;
                }

                // NOLINTNEXTLINE
                arr[h] = copy_entry(value);
                return true;
            }

            if (arr[h] == TOMBSTONE) {
                if (first_tombstone == -1) {
                    first_tombstone = h;
                }
                continue;
            }

            if (arr[h]->id == value->id) {
                if (arr[h] != value) {
                    delete arr[h];
                    arr[h] = value;
                } // else no need to update because they are already the same structure

                return true;
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

    Entry* generate_mock_data(const int size) {
        std::string names[] = {
            "Alice Johnson", "Bob Smith", "Charlie Brown", "Diana Prince",
            "Ethan Hunt", "Fiona Green", "George Miller", "Hannah Davis",
            "Isaac Newton", "Julia Roberts", "Kevin Lee", "Laura Palmer",
            "Michael Chen", "Nina Patel", "Oliver Stone", "Paula White",
            "Quinn Taylor", "Rachel Green", "Samuel Jackson", "Tina Turner",
            "Uma Thurman", "Victor Hugo", "Wendy Williams", "Xavier Woods",
            "Yara Martinez", "Zachary King"
        };
        constexpr int names_count = sizeof(names) / sizeof(names[0]);
        Entry *entries = new Entry[size];
        int *keys = new int[size];
        FillRandomArray(keys, size, 0, 50000, true, UNSORTED);

        srand(clock());

        for (int i = 0; i < size; i++) {
            entries[i].id = keys[i];
            strcpy(entries[i].name, names[rand() % names_count].c_str());
        }


        delete[] keys;
        return entries;
    }

    std::vector<int> fill_hashmap(HashMapT* h_map, float desired_fill) {
        constexpr int mock_size = HASHMAP_SIZE + 1500; // + 1500 unused for not found tests
        Entry* mock = generate_mock_data(mock_size);
        std::vector<int> inserted_ids;

        const int to_insert = HASHMAP_SIZE * desired_fill;
        inserted_ids.reserve(to_insert);

        int i = 0;
        int miss_cnt = 0;
        for (; i <= to_insert; i++) {
            if (insert(h_map, mock + i)) {
                inserted_ids.push_back(mock[i].id);
            } else miss_cnt++;
        }

        // in case of misses
        while (inserted_ids.size() < to_insert && i < mock_size) {
            if (insert(h_map, mock + i)) {
                inserted_ids.push_back(mock[i].id);
            } else miss_cnt++;
        }

        printf("Fill factor: %.2f vs desired_fill: %.2f; miss_cnt: %d\n", (float)inserted_ids.size() / (float)HASHMAP_SIZE, desired_fill, miss_cnt);

        std::vector<int> unique_unused;
        unique_unused.reserve(1500);
        for (int i = 0; i < 1500; i++) {
            unique_unused.push_back(mock[HASHMAP_SIZE + i].id);
        }

        delete[] mock;

        return unique_unused;
    }

    void print_hashmap(HashMapT* h_map) {
        for (int i = 0; i < h_map->size; i++) {
            if (h_map->arr[i] != nullptr && h_map->arr[i] != TOMBSTONE) {
                printf("id: %d; name: %s\n", h_map->arr[i]->id, h_map->arr[i]->name);
            }
        }
    }

    void print_global_hashmap() {
        print_hashmap(global_hmap);
    }

    void demonstrate(int size) {
        Entry *entries = generate_mock_data(size);
        HashMapT* h_map = create_hashmap();

        for (int i = 0; i < size; i++) {
            insert(h_map, entries + i);
        }
        delete[] entries;

        print_hashmap(h_map);

        delete_hashmap(&h_map);
    }

    void init_global_hashmap(int size) {
        if (global_hmap != nullptr) {
            printf("Already initialized, reinitializing.\n");
            delete_hashmap(&global_hmap);
        }

        Entry *entries = generate_mock_data(size);
        global_hmap = create_hashmap();

        for (int i = 0; i < size; i++) {
            insert(global_hmap, entries + i);
        }
        delete[] entries;

        print_hashmap(global_hmap);
    }

    void search_global_hashmap(int id) {
        if (global_hmap == nullptr) {
            printf("Hashmap not initialized! Please initialize using init (size)!\n");
            return;
        }

        Entry** entry = search(global_hmap, id);
        if (entry != nullptr) {
            printf("id: %d; name: %s\n", (*entry)->id, (*entry)->name);
        } else {
            printf("Entry not found!\n");
        }
    }

    void delete_from_global_hashmap(int id) {
        if (global_hmap == nullptr) {
            printf("Hashmap not initialized! Please initialize using init (size)!\n");
            return;
        }

        if (delete_entry(global_hmap, id)) {
            printf("Successfully deleted entry with id %d.\n", id);
        }
        else {
            printf("Not found!\n");
        }
    }

    void delete_global_hashmap() {
        delete_hashmap(&global_hmap);
    }


    void performance(Profiler& profiler, AnalysisCase whichCase) {
        const float alphas[] = {0.8f, 0.85f, 0.9f, 0.95f, 0.99f};
        constexpr int num_factors = sizeof alphas / sizeof alphas[0];

        std::vector<Record> statistics;
        statistics.reserve(num_factors);
        for (float alpha : alphas) {
            HashMapT* h_map = create_hashmap();
            std::vector<int> unused_ids = fill_hashmap(h_map, alpha);
            std::vector<int> uniform_inserted;
            uniform_inserted.reserve(1500);

            // now we sample 1500 entries from the hash table randomly
            srand(clock());
            int cnt = 0;
            while (cnt < 1500) {
                int idx = rand() % HASHMAP_SIZE;
                if (h_map->arr[idx] != nullptr && h_map->arr[idx] != TOMBSTONE) {
                    uniform_inserted.push_back(h_map->arr[idx]->id);
                    cnt++;
                }
            }
            // now we have precisely 1500 used and unused, we can run the test for different fill factors

            // search for inserted and record effort
            int total_effort_f = 0;
            int max_effort_f = INT_MIN;
            for (int repeat = 0; repeat < 5; repeat++) {
                for (const auto id : uniform_inserted) {
                    int effort = 0;
                    search(h_map, id, &effort);
                    total_effort_f += effort;

                    if (effort > max_effort_f) {
                        max_effort_f = effort;
                    }
                }
            }
            total_effort_f /= 5;
            float avg_effort_f = (float)total_effort_f / (float)uniform_inserted.size();

            // search for known unused ids and record effort
            int total_effort_nf = 0;
            int max_effort_nf = INT_MIN;
            for (int repeat = 0; repeat < 5; repeat++) {
                for (const auto id : unused_ids) {
                    int effort = 0;
                    search(h_map, id, &effort);
                    total_effort_nf += effort;

                    if (effort > max_effort_nf) {
                        max_effort_nf = effort;
                    }
                }
            }

            total_effort_nf /= 5;
            float avg_effort_nf = (float)total_effort_nf / (float)unused_ids.size();
            statistics.push_back({alpha, avg_effort_f, max_effort_f, avg_effort_nf, max_effort_nf});

            delete_hashmap(&h_map);
        }

        printf("Fill factor | AVG Effort | Max Effort | AVG Effort NF | Max Effort NF\n");
        for (const auto record : statistics) {
            printf("alpha: %.2f |   %.4f   |     %d     |     %.4f     | %d\n", record.alpha, record.avg_effort_f, record.max_effort_f, record.avg_effort_nf, record.max_effort_nf);
        }
    }
}
