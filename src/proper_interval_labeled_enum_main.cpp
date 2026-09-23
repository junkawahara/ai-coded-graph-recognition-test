#include "enumerators/proper_interval_labeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::ProperIntervalLabeledEnumAlgorithm;
    ProperIntervalLabeledEnumAlgorithm algo = ProperIntervalLabeledEnumAlgorithm::STRING_REPRESENTATION;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) != "--algo") {
            std::cerr << "error: unknown option " << argv[i]
                      << " (accepted: --algo string_representation|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires string_representation or reverse_search\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "string_representation") {
            algo = ProperIntervalLabeledEnumAlgorithm::STRING_REPRESENTATION;
        } else if (name == "reverse_search") {
            algo = ProperIntervalLabeledEnumAlgorithm::REVERSE_SEARCH;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: string_representation, reverse_search)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, graph_recognition::max_vertex_count, n)) return 1;

    graph_recognition::ProperIntervalLabeledEnumerationResult res =
        graph_recognition::enumerate_proper_interval_labeled_graphs_reverse_search(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
