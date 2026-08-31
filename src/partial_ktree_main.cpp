#include "recognizers/partial_ktree.h"
#include <iostream>

// Input deviates from the standard format because the class is
// parameterized: the first line is k, then the usual `n m` + edge lines.
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    int k;
    if (!(std::cin >> k)) {
        std::cerr << "error: malformed input (expected k on the first line)\n";
        return 1;
    }

    bool input_ok = false;
    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin, input_ok);
    if (!input_ok) {
        std::cerr << "error: malformed graph input\n";
        return 1;
    }

    graph_recognition::PartialKTreeResult res =
        graph_recognition::check_partial_ktree(g, k);
    if (!res.is_partial_ktree) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
