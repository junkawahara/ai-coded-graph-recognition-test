#include "biconnected.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);
    if (g.n == 0) return 0;

    graph_recognition::BiconnectedResult res = graph_recognition::check_biconnected(g);
    if (!res.is_biconnected) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
