#include "enumerators/tournament_unlabeled_enum.h"
#include "util/cli_main.h"
#include <iostream>

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

    if (!graph_recognition::cli_parse_enum_options(argc, argv, 0)) return 1;

    int n;
    if (!graph_recognition::cli_read_vertex_count(std::cin, MAX_N, n)) return 1;

    graph_recognition::TournamentUnlabeledEnumerationResult res =
        graph_recognition::enumerate_tournament_unlabeled_graphs(n);

    std::cout << res.graphs.size() << "\n";
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        graph_recognition::cli_print_edge_line(res.graphs[i].arcs);
    }
    return 0;
}

int main(int argc, char** argv) {
    return graph_recognition::run_cli_main(cli_main, argc, argv);
}
