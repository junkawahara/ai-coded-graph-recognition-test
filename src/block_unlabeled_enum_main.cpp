#include "enumerators/block_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>
#include <string>

static int cli_main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    bool connected_only = false;
    for (int i = 1; i < argc; ++i) {
        const std::string option(argv[i]);
        if (option == "--connected") {
            connected_only = true;
        } else if (option == "--algo") {
            if (++i == argc) {
                std::cerr << "error: --algo requires block_graph_gen\n";
                return 1;
            }
            if (std::string(argv[i]) != "block_graph_gen") {
                std::cerr << "error: unknown algorithm '" << argv[i]
                          << "'; valid: block_graph_gen\n";
                return 1;
            }
        } else {
            std::cerr << "error: unknown option " << option
                      << " (accepted: --connected, --algo block_graph_gen)\n";
            return 1;
        }
    }

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, graph_recognition::max_vertex_count, n)) return 1;
    const graph_recognition::BlockUnlabeledEnumerationResult result =
        graph_recognition::enumerate_block_unlabeled_graphs(n, connected_only);
    graph_recognition::cli_print_enumerated_graphs(result.graphs);
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
