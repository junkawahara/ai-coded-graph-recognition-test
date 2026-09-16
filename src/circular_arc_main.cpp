#include "recognizers/circular_arc.h"
#include "util/cli_main.h"
#include <iostream>

static int cli_main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    bool input_ok = false;
    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin, input_ok);
    if (!input_ok) {
        std::cerr << "error: malformed graph input\n";
        return 1;
    }

    graph_recognition::CircularArcResult res =
        graph_recognition::check_circular_arc(g);
    if (!res.is_circular_arc) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}

int main() { return graph_recognition::run_cli_main(cli_main); }
