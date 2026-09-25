#include "enumerators/simple_quadrangulation_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::SimpleQuadrangulationUnlabeledEnumAlgorithm;
    auto algo = SimpleQuadrangulationUnlabeledEnumAlgorithm::PLANTRI;
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
        if (name == "plantri") algo = SimpleQuadrangulationUnlabeledEnumAlgorithm::PLANTRI;
        else if (name == "reverse_search") algo = SimpleQuadrangulationUnlabeledEnumAlgorithm::REVERSE_SEARCH;
        else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: plantri, reverse_search)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, graph_recognition::max_vertex_count, n)) return 1;

    graph_recognition::SimpleQuadrangulationUnlabeledEnumerationResult res =
        graph_recognition::enumerate_simple_quadrangulation_unlabeled_graphs(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
