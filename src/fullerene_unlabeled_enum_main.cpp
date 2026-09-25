#include "enumerators/fullerene_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::FullereneUnlabeledEnumAlgorithm;
    auto algo = FullereneUnlabeledEnumAlgorithm::BUCKYGEN;
    for (int i = 1; i < argc; ++i) {
        const std::string option(argv[i]);
        if (option != "--algo") {
            std::cerr << "error: unknown option " << option
                      << " (accepted: --algo buckygen|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires buckygen or reverse_search\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "buckygen") algo = FullereneUnlabeledEnumAlgorithm::BUCKYGEN;
        else if (name == "reverse_search") algo = FullereneUnlabeledEnumAlgorithm::REVERSE_SEARCH;
        else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: buckygen, reverse_search)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, graph_recognition::max_vertex_count, n)) return 1;

    graph_recognition::FullereneUnlabeledEnumerationResult res =
        graph_recognition::enumerate_fullerene_unlabeled_graphs(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
