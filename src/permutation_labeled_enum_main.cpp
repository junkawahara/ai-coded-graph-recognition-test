#include "enumerators/permutation_labeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

namespace {

/* Preserve the existing public vertex limit in both modes; the legacy
   algorithm represents candidate neighborhoods with 64-bit masks. */
const int MAX_N = 64;

}  // namespace

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::PermutationLabeledEnumAlgorithm;
    auto algo = PermutationLabeledEnumAlgorithm::MODULAR_DECOMPOSITION;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) != "--algo") {
            std::cerr << "error: unknown option " << argv[i]
                      << " (accepted: --algo modular_decomposition|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires modular_decomposition or reverse_search\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "modular_decomposition") {
            algo = PermutationLabeledEnumAlgorithm::MODULAR_DECOMPOSITION;
        } else if (name == "reverse_search") {
            algo = PermutationLabeledEnumAlgorithm::REVERSE_SEARCH;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: modular_decomposition, reverse_search)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, MAX_N, n)) return 1;

    graph_recognition::PermutationLabeledEnumerationResult res =
        graph_recognition::enumerate_permutation_labeled_graphs_reverse_search(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
