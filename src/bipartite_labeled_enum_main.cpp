#include "enumerators/bipartite_labeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

namespace {

/* Largest n the enumerator can represent (the candidate neighbourhoods of a new vertex are 64-bit masks); its entry function
   throws std::invalid_argument beyond it. Printing the empty result it used
   to return there as the count "0" was a false answer: every class has a
   member on every n. */
const int MAX_N = 64;

}  // namespace

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::BipartiteLabeledEnumAlgorithm;
    BipartiteLabeledEnumAlgorithm algo = BipartiteLabeledEnumAlgorithm::BIPARTITE_COLORING;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) != "--algo") {
            std::cerr << "error: unknown option " << argv[i]
                      << " (accepted: --algo bipartite_coloring|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires bipartite_coloring or reverse_search\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "bipartite_coloring") {
            algo = BipartiteLabeledEnumAlgorithm::BIPARTITE_COLORING;
        } else if (name == "reverse_search") {
            algo = BipartiteLabeledEnumAlgorithm::REVERSE_SEARCH;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: bipartite_coloring, reverse_search)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, MAX_N, n)) return 1;

    graph_recognition::BipartiteLabeledEnumerationResult res =
        graph_recognition::enumerate_bipartite_labeled_graphs_reverse_search(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
