#include "enumerators/proper_interval_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    bool connected_only = false;
    if (!graph_recognition::cli_parse_enum_options(argc, argv, &connected_only)) return 1;

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, graph_recognition::max_vertex_count, n)) return 1;

    graph_recognition::ProperIntervalUnlabeledEnumerationResult res =
        graph_recognition::enumerate_proper_interval_unlabeled_graphs(n, connected_only);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
