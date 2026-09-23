#include "enumerators/bipartite_permutation_labeled_enum.h"
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

    using graph_recognition::BipartitePermutationLabeledEnumAlgorithm;
    auto algo = BipartitePermutationLabeledEnumAlgorithm::STRING_REPRESENTATION;
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
            algo = BipartitePermutationLabeledEnumAlgorithm::STRING_REPRESENTATION;
        } else if (name == "reverse_search") {
            algo = BipartitePermutationLabeledEnumAlgorithm::REVERSE_SEARCH;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: string_representation, reverse_search)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, MAX_N, n)) return 1;

    graph_recognition::BipartitePermutationLabeledEnumerationResult res =
        graph_recognition::enumerate_bipartite_permutation_labeled_graphs_reverse_search(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
