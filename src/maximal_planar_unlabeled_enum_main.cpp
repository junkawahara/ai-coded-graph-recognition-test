#include "enumerators/maximal_planar_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

namespace {

/* Largest n the enumerator can represent (vertex neighbourhoods are 64-bit masks); its entry function
   throws std::invalid_argument beyond it. Printing the empty result it used
   to return there as the count "0" was a false answer: every class has a
   member on every n. */
const int MAX_N = 63;

}  // namespace

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::MaximalPlanarUnlabeledEnumAlgorithm;
    auto algo = MaximalPlanarUnlabeledEnumAlgorithm::PLANTRI;
    for (int i = 1; i < argc; ++i) {
        const std::string option(argv[i]);
        if (option != "--algo") {
            std::cerr << "error: unknown option " << option
                      << " (accepted: --algo plantri|canonical_augmentation)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires plantri or canonical_augmentation\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "plantri") algo = MaximalPlanarUnlabeledEnumAlgorithm::PLANTRI;
        else if (name == "canonical_augmentation") algo = MaximalPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION;
        else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: plantri, canonical_augmentation)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, MAX_N, n)) return 1;

    graph_recognition::MaximalPlanarUnlabeledEnumerationResult res =
        graph_recognition::enumerate_maximal_planar_unlabeled_graphs(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
