#include "hash_table.h"

#include <climits>

#include "catch2.hpp"
#include <iostream>
#include <random>

namespace lab05
{
    HashMapT* global_hmap;

    int random_number(int lo, int hi) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> distrib(lo, hi);
        return distrib(gen);
    }

    int random_id(const HashMapT *h_map) {
        if (h_map == nullptr || h_map->inserted == 0) return -1; // raise SIGSEGV

        for (int i = 0; i < HASHMAP_SIZE; i++) {
            const int index = random_number(0, HASHMAP_SIZE - 1);
            if (h_map->arr[index] != nullptr && h_map->arr[index] != TOMBSTONE) {
                return h_map->arr[index]->id;
            }
        }

        return -1; // timeout
    }

    HashMapT* create_hashmap() {
        // NOLINTNEXTLINE
        auto* h_map = new HashMapT;

        h_map->arr = new Entry*[HASHMAP_SIZE]; // memory management is deferred to the user
        h_map->size = HASHMAP_SIZE;
        h_map->inserted = 0;

        memset(h_map->arr, 0, HASHMAP_SIZE * sizeof(Entry*)); // zero memory

        return h_map;
    }

    Entry* copy_entry(const Entry* val) {
        // NOLINTNEXTLINE
        Entry* copy = new Entry;

        copy->id = val->id;
        strcpy(copy->name, val->name);

        return copy;
    }

    void delete_hashmap(HashMapT** h_map) {
        if (*h_map == nullptr) return;

        for (int i = 0; i < (*h_map)->size; i++) {
            if ((*h_map)->arr[i] != nullptr && (*h_map)->arr[i] != TOMBSTONE) {
                delete (*h_map)->arr[i]; // delete each entry
            }
        }

        delete[] (*h_map)->arr;

        delete *h_map; // delete the hashmap structure
        (*h_map) = nullptr;
    }

    int hash(const int key, const int idx) {
        constexpr int c1 = 1;
        constexpr int c2 = 1;
        return (key + c1*idx + c2*idx*idx) % HASHMAP_SIZE;
    }

    Entry** search(const HashMapT* h_map, const int key, int* effort) {
        Entry** arr = h_map->arr;

        for (int i = 0; i < HASHMAP_SIZE; i++) {
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
        for (int i = 0; i < HASHMAP_SIZE; i++) {
            const int h = hash(value->id, i);

            if (arr[h] == nullptr) {
                // free slot found, check if no tombstone was found previously and insert
                if (first_tombstone != -1) {
                    // NOLINTNEXTLINE
                    arr[first_tombstone] = copy_entry(value);
                    h_map->inserted++;
                    return true;
                }

                // NOLINTNEXTLINE
                arr[h] = copy_entry(value);
                h_map->inserted++;
                return true;
            }

            if (arr[h] == TOMBSTONE) {
                if (first_tombstone == -1) {
                    first_tombstone = h;
                }
                continue;
            }

            // update if already in the map
            if (arr[h]->id == value->id) {
                if (arr[h] != value) {
                    delete arr[h];
                    arr[h] = copy_entry(value);
                } // else no need to update because they are already the same structure

                return true;
            }
        }

        return false; // no slot found
    }

    bool delete_entry(HashMapT *h_map, const int key) {
        Entry** index = search(h_map, key);
        if (index == nullptr) {
            return false; // not in table
        }

        delete *index; // delete structure pointed to by index
        *index = TOMBSTONE;
        h_map->inserted--;
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
        auto entries = new Entry[size];
        int *keys = new int[size];
        FillRandomArray(keys, size, 0, 50000, true, UNSORTED);

        for (int i = 0; i < size; i++) {
            entries[i].id = keys[i];
            strcpy(entries[i].name, names[random_number(0, names_count - 1)].c_str());
        }


        delete[] keys;
        return entries;
    }

    std::vector<int> fill_hashmap(HashMapT *h_map, float desired_fill) {
        constexpr int mock_size = HASHMAP_SIZE + 1500; // + 1500 unused for not found tests
        Entry* mock = generate_mock_data(mock_size);

        const int to_insert = HASHMAP_SIZE * desired_fill;

        int i = 0;
        int miss_cnt = 0;
        for (; i <= to_insert; i++) {
            if (!insert(h_map, mock + i)) {
                miss_cnt++;
            }
        }

        // in case of misses
        while (h_map->inserted < to_insert && i < mock_size) {
            if (!insert(h_map, mock + i)) {
                miss_cnt++;
            }
        }

        if (miss_cnt != 0) {
            printf("Fill factor: %.2f vs desired_fill: %.2f; miss_cnt: %d; inserted: %d\n",
                   static_cast<float>(h_map->inserted) / static_cast<float>(HASHMAP_SIZE), desired_fill, miss_cnt, h_map->inserted);
        }

        std::vector<int> unique_unused;
        unique_unused.reserve(1500);
        for (int x = 0; x < 1500; x++) {
            unique_unused.push_back(mock[HASHMAP_SIZE + x].id);
        }

        delete[] mock;

        return unique_unused;
    }

    void decrease_fill(HashMapT *h_map, const float desired_fill) {
        if (h_map == nullptr) {
            return;
        }

        const int desired_elems = HASHMAP_SIZE * desired_fill;
        while (h_map->inserted > desired_elems) {
            delete_entry(h_map, random_id(h_map));
        }

        const float new_fill = static_cast<float>(h_map->inserted) / static_cast<float>(HASHMAP_SIZE);
    }

    void print_hashmap(const HashMapT* h_map) {
        for (int i = 0; i < HASHMAP_SIZE; i++) {
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


    void performance(AnalysisCase whichCase) {
        constexpr float alphas[] = {0.8f, 0.85f, 0.9f, 0.95f, 0.99f};
        constexpr int num_factors = sizeof alphas / sizeof alphas[0];

        std::vector<Record> statistics;
        statistics.reserve(num_factors);

        for (float alpha : alphas) {

            int total_effort_f = 0;
            int total_max_effort_f = 0;

            int total_effort_nf = 0;
            int total_max_effort_nf = 0;

            for (int it = 0; it < 5; it++) {
                auto h_map = create_hashmap();
                auto unused = fill_hashmap(h_map, alpha);
                // hashmap is filled and now we begin sampling

                int max_effort_f = -1; // effort will never be negative

                for (int x = 0; x < 1500; x++) {
                    int effort = 0;

                    search(h_map, random_id(h_map), &effort);

                    if (effort > max_effort_f) {
                        max_effort_f = effort;
                    }

                    total_effort_f += effort;
                }

                total_max_effort_f += max_effort_f;

                int max_effort_nf = -1; // effort is never negative

                for (auto id : unused) {
                    int effort = 0;

                    search(h_map, id, &effort);

                    total_effort_nf += effort;

                    if (effort > max_effort_nf) {
                        max_effort_nf = effort;
                    }
                }

                total_max_effort_nf += max_effort_nf;

                delete_hashmap(&h_map);
            }

            const float avg_effort_f = static_cast<float>(total_effort_f) / 7500.f;
            const float avg_max_effort_f = static_cast<float>(total_max_effort_f) / 5.f;

            const float avg_effort_nf = static_cast<float>(total_effort_nf) / (7500.f);
            const float avg_max_effort_nf = static_cast<float>(total_max_effort_nf) / 5.f;

            statistics.push_back({alpha, avg_effort_f, avg_max_effort_f, avg_effort_nf, avg_max_effort_nf});
        }

        printf("\nFill factor | AVG Effort | Max Effort | AVG Effort NF | Max Effort NF\n");
        for (const auto record : statistics) {
            printf("0.0 -> %.2f |   %.4f   |   %.4f  |    %.4f     | %.4f\n", record.alpha, record.avg_effort_f, record.max_effort_f_avg, record.avg_effort_nf, record.max_effort_nf_avg);
        }

        // put in block to scope variables like previous tests
        {
            // TEST AFTER DELETIONS
            // First fill to 0.99f then decrease fill by deletion to 0.8f
            HashMapT* h_map = create_hashmap();
            const auto unused = fill_hashmap(h_map, 0.99f);
            decrease_fill(h_map, 0.8f);

            // now we test 1500 searches for found and 1500 for not found
            int total_effort_f = 0;
            int total_max_effort_f = 0;

            int total_effort_nf = 0;
            int total_max_effort_nf = 0;
            for (int it = 0; it < 5; it++) {

                int max_effort = 0;

                for (int i = 0; i < 1500; i++) {
                    const int id = random_id(h_map);
                    int effort = 0;

                    search(h_map, id, &effort);

                    total_effort_f += effort;
                    if (effort > max_effort) {
                        max_effort = effort;
                    }
                }

                total_max_effort_f += max_effort;

                // unused id tests
                max_effort = 0;
                for (const auto id : unused) {
                    int effort = 0;

                    search(h_map, id, &effort);

                    total_effort_nf += effort;
                    if (effort > max_effort) {
                        max_effort = effort;
                    }
                }

                total_max_effort_nf += max_effort;
            }

            const float avg_effort_f = static_cast<float>(total_effort_f) / 7500.f;
            const float avg_max_effort_f = static_cast<float>(total_max_effort_f) / 5.f;

            const float avg_effort_nf = static_cast<float>(total_effort_nf) / 7500.f;
            const float avg_max_effort_nf = static_cast<float>(total_max_effort_nf) / 5.f;

            printf("\n===PERFORMANCE AFTER DELETION===\nFill factor | AVG Effort | Max Effort | AVG Effort NF | Max Effort NF\n0.99 -> 0.8 |   %.4f   |  %.4f  |    %.4f    | %.4f\n", avg_effort_f, avg_max_effort_f, avg_effort_nf, avg_max_effort_nf);

            delete_hashmap(&h_map);
        }
        // NOLINTNEXTLINE
    }


    TEST_CASE("Hash function produces valid indices") {
        for (int i = 0; i < 100; i++) {
            int h = hash(12345, i);
            REQUIRE(h >= 0);
            REQUIRE(h < HASHMAP_SIZE);
        }
    }

    TEST_CASE("Create hashmap initializes correctly") {
        HashMapT* h_map = create_hashmap();
        REQUIRE(h_map != nullptr);
        REQUIRE(h_map->arr != nullptr);
        REQUIRE(h_map->size == HASHMAP_SIZE);
        REQUIRE(h_map->inserted == 0);

        // Check all slots are initialized to nullptr
        for (int i = 0; i < HASHMAP_SIZE; i++) {
            REQUIRE(h_map->arr[i] == nullptr);
        }

        delete_hashmap(&h_map);
        REQUIRE(h_map == nullptr);
    }

    TEST_CASE("Insert single entry") {
        HashMapT* h_map = create_hashmap();
        Entry entry = {100, "Test User"};

        bool result = insert(h_map, &entry);
        REQUIRE(result == true);
        REQUIRE(h_map->inserted == 1);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Search finds inserted entry") {
        HashMapT* h_map = create_hashmap();
        Entry entry = {200, "Alice"};

        insert(h_map, &entry);
        int effort = 0;
        Entry** found = search(h_map, 200, &effort);

        REQUIRE(found != nullptr);
        REQUIRE((*found)->id == 200);
        REQUIRE(strcmp((*found)->name, "Alice") == 0);
        REQUIRE(effort > 0);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Search returns nullptr for non-existent entry") {
        HashMapT* h_map = create_hashmap();
        Entry entry = {300, "Bob"};

        insert(h_map, &entry);
        Entry** found = search(h_map, 999);

        REQUIRE(found == nullptr);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Insert multiple entries with different keys") {
        HashMapT* h_map = create_hashmap();

        for (int i = 0; i < 100; i++) {
            Entry entry = {i, "User"};
            bool result = insert(h_map, &entry);
            REQUIRE(result == true);
        }

        REQUIRE(h_map->inserted == 100);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Insert updates existing entry with same key") {
        HashMapT* h_map = create_hashmap();
        Entry entry1 = {500, "Original"};
        Entry entry2 = {500, "Updated"};

        insert(h_map, &entry1);
        REQUIRE(h_map->inserted == 1);

        insert(h_map, &entry2);
        REQUIRE(h_map->inserted == 1); // Count should not increase

        Entry** found = search(h_map, 500);
        REQUIRE(found != nullptr);
        REQUIRE(strcmp((*found)->name, "Updated") == 0);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Delete removes entry") {
        HashMapT* h_map = create_hashmap();
        Entry entry = {600, "ToDelete"};

        insert(h_map, &entry);
        REQUIRE(h_map->inserted == 1);

        bool deleted = delete_entry(h_map, 600);
        REQUIRE(deleted == true);
        REQUIRE(h_map->inserted == 0);

        Entry** found = search(h_map, 600);
        REQUIRE(found == nullptr);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Delete non-existent entry returns false") {
        HashMapT* h_map = create_hashmap();

        bool deleted = delete_entry(h_map, 999);
        REQUIRE(deleted == false);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Delete creates tombstone") {
        HashMapT* h_map = create_hashmap();
        Entry entry = {700, "TombTest"};

        insert(h_map, &entry);
        int h = hash(700, 0);

        delete_entry(h_map, 700);
        REQUIRE(h_map->arr[h] == TOMBSTONE);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Insert reuses tombstone slot") {
        HashMapT* h_map = create_hashmap();
        Entry entry1 = {800, "First"};
        Entry entry2 = {800, "Second"};

        insert(h_map, &entry1);
        delete_entry(h_map, 800);

        // Insert new entry that hashes to same location
        insert(h_map, &entry2);
        REQUIRE(h_map->inserted == 1);

        Entry** found = search(h_map, 800);
        REQUIRE(found != nullptr);
        REQUIRE(strcmp((*found)->name, "Second") == 0);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Search continues past tombstone") {
        HashMapT* h_map = create_hashmap();

        // Create collision scenario
        Entry entry1 = {900, "Entry1"};
        Entry entry2 = {901, "Entry2"};

        insert(h_map, &entry1);
        insert(h_map, &entry2);
        delete_entry(h_map, 900);

        // Should still find entry2
        Entry** found = search(h_map, 901);
        REQUIRE(found != nullptr);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Effort tracking increments correctly") {
        HashMapT* h_map = create_hashmap();
        Entry entry = {1000, "EffortTest"};

        insert(h_map, &entry);

        int effort = 0;
        search(h_map, 1000, &effort);
        REQUIRE(effort >= 1); // At least one probe

        delete_hashmap(&h_map);
    }

    TEST_CASE("Copy entry creates independent copy") {
        Entry original = {1100, "Original"};
        Entry* copy = copy_entry(&original);

        REQUIRE(copy != nullptr);
        REQUIRE(copy != &original);
        REQUIRE(copy->id == original.id);
        REQUIRE(strcmp(copy->name, original.name) == 0);

        // Modify copy
        strcpy(copy->name, "Modified");
        REQUIRE(strcmp(original.name, "Original") == 0);

        delete copy;
    }

    TEST_CASE("Fill hashmap achieves target fill factor") {
        HashMapT* h_map = create_hashmap();
        float target_alpha = 0.8f;

        fill_hashmap(h_map, target_alpha);
        float actual_alpha = static_cast<float>(h_map->inserted) / HASHMAP_SIZE;

        REQUIRE(actual_alpha >= target_alpha);
        REQUIRE(actual_alpha < target_alpha + 0.01f); // Within 1%

        delete_hashmap(&h_map);
    }

    TEST_CASE("Insert returns false for null entry") {
        HashMapT* h_map = create_hashmap();

        bool result = insert(h_map, nullptr);
        REQUIRE(result == false);
        REQUIRE(h_map->inserted == 0);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Random ID returns valid ID from filled table") {
        HashMapT* h_map = create_hashmap();
        fill_hashmap(h_map, 0.5f);

        int id = random_id(h_map);
        REQUIRE(id != -1);

        // Verify the ID exists
        Entry** found = search(h_map, id);
        REQUIRE(found != nullptr);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Random ID returns -1 for empty table") {
        HashMapT* h_map = create_hashmap();

        int id = random_id(h_map);
        REQUIRE(id == -1);

        delete_hashmap(&h_map);
    }

    TEST_CASE("Decrease fill reduces elements correctly") {
        HashMapT* h_map = create_hashmap();
        fill_hashmap(h_map, 0.95f);

        decrease_fill(h_map, 0.8f);
        float actual_alpha = static_cast<float>(h_map->inserted) / HASHMAP_SIZE;

        REQUIRE(actual_alpha <= 0.8f);
        REQUIRE(actual_alpha >= 0.79f);

        delete_hashmap(&h_map);
    }
}
