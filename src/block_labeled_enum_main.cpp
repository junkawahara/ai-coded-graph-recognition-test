#include "enumerators/block_labeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::BlockLabeledEnumAlgorithm algo =
        graph_recognition::BlockLabeledEnumAlgorithm::BLOCK_GRAPH_GEN;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) != "--algo") {
            std::cerr << "error: unknown option " << argv[i]
                      << " (accepted: --algo block_graph_gen|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires block_graph_gen or reverse_search\n";
            return 1;
        }
        const std::string algorithm(argv[i]);
        if (algorithm == "block_graph_gen")
            algo = graph_recognition::BlockLabeledEnumAlgorithm::BLOCK_GRAPH_GEN;
        else if (algorithm == "reverse_search")
            algo = graph_recognition::BlockLabeledEnumAlgorithm::REVERSE_SEARCH;
        else {
            std::cerr << "error: unknown algorithm '" << algorithm
                      << "'; valid: block_graph_gen, reverse_search\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, graph_recognition::max_vertex_count, n)) return 1;

    graph_recognition::BlockLabeledEnumerationResult res =
        graph_recognition::enumerate_block_labeled_graphs_reverse_search(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
