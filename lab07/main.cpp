#include <iostream>
#include <Profiler.h>
#include <commandline.h>
#include "os_tree.h"

Profiler profiler("ostree");

void demo(const CommandArgs& args) {
    int l = 1, r = 11;
    if (!args.empty()) {
        l = atoi(args[0]);
        r = atoi(args[1]);
    }
    lab07::demonstrate(l, r);
}

void perf(const CommandArgs& args)
{
    lab07::performance(profiler);
    profiler.showReport();
    profiler.reset();
}

int main() {
    const std::vector<CommandSpec> commands =
    {
        {"demo", demo, "Run demo, optional argument for size"},
        {"perf", perf, "Generate charts"}
    };
    return runCommandLoop(commands);
}