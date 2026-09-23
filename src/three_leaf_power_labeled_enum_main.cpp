#include "enumerators/three_leaf_power_labeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::ThreeLeafPowerLabeledEnumAlgorithm;
    ThreeLeafPowerLabeledEnumAlgorithm algo = ThreeLeafPowerLabeledEnumAlgorithm::VERTEX_INCREMENTAL;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) != "--algo") {
            std::cerr << "error: unknown option " << argv[i]
                      << " (accepted: --algo vertex_incremental|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires vertex_incremental or reverse_search\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "vertex_incremental") {
            algo = ThreeLeafPowerLabeledEnumAlgorithm::VERTEX_INCREMENTAL;
        } else if (name == "reverse_search") {
            algo = ThreeLeafPowerLabeledEnumAlgorithm::REVERSE_SEARCH;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: vertex_incremental, reverse_search)\n";
            return 1;
        }
    }

    int n;
    const int max_n = algo == ThreeLeafPowerLabeledEnumAlgorithm::VERTEX_INCREMENTAL
                          ? 63 : graph_recognition::max_vertex_count;
    if (!graph_recognition::cli_read_vertex_count(std::cin, max_n, n)) return 1;

    graph_recognition::ThreeLeafPowerLabeledEnumerationResult res =
        graph_recognition::enumerate_three_leaf_power_labeled_graphs_reverse_search(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
