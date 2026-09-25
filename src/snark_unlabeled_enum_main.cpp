#include "enumerators/snark_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    using graph_recognition::SnarkUnlabeledEnumAlgorithm;
    auto algo = SnarkUnlabeledEnumAlgorithm::SNARKHUNTER;
    for (int i = 1; i < argc; ++i) {
        const std::string option(argv[i]);
        if (option != "--algo") {
            std::cerr << "error: unknown option " << option
                      << " (accepted: --algo snarkhunter|canonical_augmentation)\n";
            return 1;
        }
        if (++i == argc) {
            std::cerr << "error: --algo requires snarkhunter or canonical_augmentation\n";
            return 1;
        }
        const std::string name(argv[i]);
        if (name == "snarkhunter") algo = SnarkUnlabeledEnumAlgorithm::SNARKHUNTER;
        else if (name == "canonical_augmentation") algo = SnarkUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION;
        else {
            std::cerr << "error: unknown algorithm " << name
                      << " (accepted: snarkhunter, canonical_augmentation)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, 63, n)) return 1;

    graph_recognition::SnarkUnlabeledEnumerationResult res =
        graph_recognition::enumerate_snark_unlabeled_graphs(n, algo);

    graph_recognition::cli_print_enumerated_graphs(res.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
