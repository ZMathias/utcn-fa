// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <Profiler.h>
#include <commandline.h>
#include "dfs.h"

Profiler profiler("disjoint_sets");

void demo(const CommandArgs& args) {

    lab10::demonstrate();
}

void perf(const CommandArgs& args)
{
    lab10::performance(profiler);
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