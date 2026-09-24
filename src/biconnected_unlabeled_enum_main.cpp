#include "enumerators/biconnected_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <sstream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::BiconnectedUnlabeledEnumAlgorithm;
    auto algo = BiconnectedUnlabeledEnumAlgorithm::EAR_AUGMENTATION;
    int max_edges = -1;
    for (int i = 1; i < argc; ++i) {
        const std::string option(argv[i]);
        if (option == "--max-edges") {
            if (++i == argc) {
                std::cerr << "error: --max-edges requires a nonnegative integer\n";
                return 1;
            }
            std::istringstream value(argv[i]);
            if (!(value >> max_edges) || max_edges < 0 || !(value >> std::ws).eof()) {
                std::cerr << "error: --max-edges requires a nonnegative integer\n";
                return 1;
            }
            continue;
        }
        if (option != "--algo") {
            std::cerr << "error: unknown option " << option
                      << " (accepted: --algo ear_augmentation|canonical_augmentation, --max-edges M)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires ear_augmentation or canonical_augmentation\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "ear_augmentation") {
            algo = BiconnectedUnlabeledEnumAlgorithm::EAR_AUGMENTATION;
        } else if (name == "canonical_augmentation") {
            algo = BiconnectedUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: ear_augmentation, canonical_augmentation)\n";
            return 1;
        }
    }

    int n;
    const int max_n = 63;
    if (!graph_recognition::cli_read_vertex_count(std::cin, max_n, n)) return 1;
    const auto result = graph_recognition::enumerate_biconnected_unlabeled_graphs(n, algo, max_edges);
    graph_recognition::cli_print_enumerated_graphs(result.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
