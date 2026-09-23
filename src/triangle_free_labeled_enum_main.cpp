#include "enumerators/triangle_free_labeled_enum.h"
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

    using graph_recognition::TriangleFreeLabeledEnumAlgorithm;
    TriangleFreeLabeledEnumAlgorithm algo = TriangleFreeLabeledEnumAlgorithm::INDEPENDENT_SETS;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) != "--algo") {
            std::cerr << "error: unknown option " << argv[i]
                      << " (accepted: --algo independent_sets|reverse_search)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires independent_sets or reverse_search\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "independent_sets") {
            algo = TriangleFreeLabeledEnumAlgorithm::INDEPENDENT_SETS;
        } else if (name == "reverse_search") {
            algo = TriangleFreeLabeledEnumAlgorithm::REVERSE_SEARCH;
        } else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: independent_sets, reverse_search)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, MAX_N, n)) return 1;

    graph_recognition::TriangleFreeLabeledEnumerationResult res =
        graph_recognition::enumerate_triangle_free_labeled_graphs_reverse_search(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
