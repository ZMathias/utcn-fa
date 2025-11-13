#include "hash_table.h"

#define CATCH_CONFIG_RUNNER
#include "catch2.hpp"

#include "commandline.h"
#include <cstdio>
#include <string>

using namespace lab05;

void demo(const CommandArgs& args)
{
    const int size = args.empty()? 10: atoi(args[0]);
    demonstrate(size);
}

void test(const CommandArgs& args)
{
    static Catch::Session session;
    session.run();
}

void perf(const CommandArgs& args)
{
    const auto whichCase = args.empty()? AVERAGE: strToCase(args[0]);
    performance(whichCase);
}

void init_menu(const CommandArgs& args) {
    const int size = args.empty() ? 10 : atoi(args[0]);
    if (size == 0) {
        delete_global_hashmap();
        return;
    }
    init_global_hashmap(size);
}

void search_menu(const CommandArgs& args) {
    if (args.empty()) {
        printf("id required!\n");
        return;
    }
    const int id = atoi(args[0]);
    search_global_hashmap(id);
}

void delete_menu(const CommandArgs& args) {
    if (args.empty()) {
        printf("id required!\n");
        return;
    }
    const int id = atoi(args[0]);
    delete_from_global_hashmap(id);
}

void print_menu(const CommandArgs& args) {
    print_global_hashmap();
}

void insert_menu(const CommandArgs& args) {
    if (args.empty()) {
        printf("usage: insert [id] [name]\n");
        return;
    }
    insert_global_hashmap(atoi(args[0]), args[1]);
}

int main()
{
    const std::vector<CommandSpec> commands =
    {
        {"demo", demo, "run demo"},
        {"test", test, "run unit-tests"},
        {"perf", perf, "[avg(default)|best|worst] - run performance analysis on selected case"},
        {"init", init_menu, "init id (opt) -- Creates and inserts mock data into a global hashmap for testing"},
        {"search", search_menu, "search [id] -- Searches the global hashmap and prints the entry if available."},
        {"delete", delete_menu, "delete [id] -- Deletes an entry"},
        {"print", print_menu, "Prints the hashmap."},
        {"insert", insert_menu, "insert [id] [name]"}
    };

    return runCommandLoop(commands);
}