#include "enumerators/planar_labeled_enum.h"
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

    using graph_recognition::PlanarLabeledEnumAlgorithm;
    auto algo = PlanarLabeledEnumAlgorithm::PLANTRI;
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
        if (name == "plantri") algo = PlanarLabeledEnumAlgorithm::PLANTRI;
        else if (name == "reverse_search") algo = PlanarLabeledEnumAlgorithm::REVERSE_SEARCH;
        else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: plantri, reverse_search)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin,
            algo == PlanarLabeledEnumAlgorithm::PLANTRI ? 63 : MAX_N, n)) return 1;

    graph_recognition::PlanarLabeledEnumerationResult res =
        graph_recognition::enumerate_planar_labeled_graphs_reverse_search(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
