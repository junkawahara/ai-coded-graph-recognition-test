#include "enumerators/laman_labeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::LamanLabeledEnumAlgorithm;
    auto algo = LamanLabeledEnumAlgorithm::HENNEBERG;
    for (int i = 1; i < argc; ++i) {
        const std::string option(argv[i]);
        if (option != "--algo") {
            std::cerr << "error: unknown option " << option
                      << " (accepted: --algo henneberg|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires henneberg or reverse_search\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "henneberg") {
            algo = LamanLabeledEnumAlgorithm::HENNEBERG;
        } else if (name == "reverse_search") {
            algo = LamanLabeledEnumAlgorithm::REVERSE_SEARCH;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: henneberg, reverse_search)\n";
            return 1;
        }
    }

    int n;
    const int max_n = algo == LamanLabeledEnumAlgorithm::HENNEBERG ? 63 : 64;
    if (!graph_recognition::cli_read_vertex_count(std::cin, max_n, n)) return 1;

    graph_recognition::LamanLabeledEnumerationResult res =
        graph_recognition::enumerate_laman_labeled_graphs(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
