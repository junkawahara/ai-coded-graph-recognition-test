#include "enumerators/permutation_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

namespace {

/* Preserve the existing public vertex limit in both modes; the legacy
   algorithm represents candidate neighborhoods with 64-bit masks. */
const int MAX_N = 63;

}  // namespace

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    bool connected_only = false;
    using graph_recognition::PermutationUnlabeledEnumAlgorithm;
    auto algo = PermutationUnlabeledEnumAlgorithm::MODULAR_DECOMPOSITION;
    for (int i = 1; i < argc; ++i) {
        const std::string option(argv[i]);
        if (option == "--connected") {
            connected_only = true;
            continue;
        }
        if (option != "--algo") {
            std::cerr << "error: unknown option " << option
                      << " (accepted: --connected, --algo modular_decomposition|canonical_augmentation)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires modular_decomposition or canonical_augmentation\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "modular_decomposition") {
            algo = PermutationUnlabeledEnumAlgorithm::MODULAR_DECOMPOSITION;
        } else if (name == "canonical_augmentation") {
            algo = PermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: modular_decomposition, canonical_augmentation)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, MAX_N, n)) return 1;

    graph_recognition::PermutationUnlabeledEnumerationResult res =
        graph_recognition::enumerate_permutation_unlabeled_graphs(n, connected_only, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
