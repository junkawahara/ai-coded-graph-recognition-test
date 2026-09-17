#include "enumerators/cubic_planar_labeled_enum.h"
#include "util/cli_main.h"
#include <iostream>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    if (!graph_recognition::cli_parse_enum_options(argc, argv, 0)) return 1;

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, graph_recognition::max_vertex_count, n)) return 1;

    graph_recognition::CubicPlanarLabeledEnumerationResult res =
        graph_recognition::enumerate_cubic_planar_labeled_graphs(n);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
