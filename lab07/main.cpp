#include <iostream>
#include <Profiler.h>
#include <commandline.h>
#include "os_tree.h"

Profiler profiler("ostree");

void demo(const CommandArgs& args) {
    const int n = args.empty() ? 10 : atoi(args[0]);
    lab07::demonstrate(n);
}

void perf(const CommandArgs& args)
{
    lab07::performance(profiler);
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