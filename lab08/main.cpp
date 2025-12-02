#include <iostream>
#include <Profiler.h>
#include <commandline.h>
#include "sets.h"

Profiler profiler("disjoint_sets");

void demo(const CommandArgs& args) {

    lab08::demonstrate();
}

void perf(const CommandArgs& args)
{
    lab08::performance(profiler);
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