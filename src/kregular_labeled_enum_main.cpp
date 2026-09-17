#include "enumerators/kregular_labeled_enum.h"
#include "util/cli_main.h"
#include <climits>
#include <iostream>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    if (!graph_recognition::cli_parse_enum_options(argc, argv, 0)) return 1;

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, graph_recognition::max_vertex_count, n)) return 1;
    int k;
    if (!graph_recognition::cli_read_int(std::cin, "k", INT_MIN, INT_MAX, k)) return 1;

    graph_recognition::KRegularLabeledEnumerationResult res =
        graph_recognition::enumerate_kregular_labeled_graphs_reverse_search(n, k);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
