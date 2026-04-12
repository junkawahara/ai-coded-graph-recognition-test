#include "odd_hole_free.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);
    if (g.n == 0) return 0;

    graph_recognition::OddHoleFreeResult res =
        graph_recognition::check_odd_hole_free(g);
    if (!res.is_odd_hole_free) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
