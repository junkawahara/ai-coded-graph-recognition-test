#include "recognizers/poset.h"
#include "recognizers/tournament.h" /* read_directed */
#include "util/cli_main.h"
#include <iostream>
#include <utility>
#include <vector>

static int cli_main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    /* Same contract as the undirected CLIs (Graph::read with its ok flag):
       an unreadable header, a negative count or a missing arc line is a
       malformed input, reported on stderr with exit status 1, never an
       (empty) YES. */
    int n;
    std::vector<std::pair<int, int>> arcs;
    bool ok;
    graph_recognition::read_directed(std::cin, n, arcs, ok);
    if (!ok) {
        std::cerr << "error: malformed graph input\n";
        return 1;
    }

    graph_recognition::PosetResult res = graph_recognition::check_poset(n, arcs);
    if (!res.is_poset) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}

int main() { return graph_recognition::run_cli_main(cli_main); }
