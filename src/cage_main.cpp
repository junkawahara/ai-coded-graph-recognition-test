#include "recognizers/cage.h"
#include <iostream>

// Input deviates from the standard format because the class is
// parameterized: the first line is `k g`, then the usual `n m` + edge
// lines. YES/NO reports cage-ness (is_cage), the strictest of the
// result's fields.
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    int k, girth;
    if (!(std::cin >> k >> girth)) {
        std::cerr << "error: malformed input (expected `k g` on the first line)\n";
        return 1;
    }

    bool input_ok = false;
    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin, input_ok);
    if (!input_ok) {
        std::cerr << "error: malformed graph input\n";
        return 1;
    }

    graph_recognition::CageResult res =
        graph_recognition::check_cage(g, k, girth);
    if (!res.is_cage) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
