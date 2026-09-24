#include "enumerators/cubic_planar_labeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::CubicPlanarLabeledEnumAlgorithm;
    auto algo = CubicPlanarLabeledEnumAlgorithm::PLANTRI;
    for (int i = 1; i < argc; ++i) {
        const std::string option(argv[i]);
        if (option != "--algo") {
            std::cerr << "error: unknown option " << option
                      << " (accepted: --algo plantri|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires plantri or reverse_search\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "plantri") algo = CubicPlanarLabeledEnumAlgorithm::PLANTRI;
        else if (name == "reverse_search") algo = CubicPlanarLabeledEnumAlgorithm::REVERSE_SEARCH;
        else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: plantri, reverse_search)\n";
            return 1;
        }
    }

    int n;
    const int max_n = algo == CubicPlanarLabeledEnumAlgorithm::PLANTRI
        ? 63 : graph_recognition::max_vertex_count;
    if (!graph_recognition::cli_read_vertex_count(std::cin, max_n, n)) return 1;

    graph_recognition::CubicPlanarLabeledEnumerationResult res =
        graph_recognition::enumerate_cubic_planar_labeled_graphs(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
