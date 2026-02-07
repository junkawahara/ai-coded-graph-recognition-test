#include "chordal_bipartite.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);
    if (g.n == 0) return 0;

    graph_recognition::ChordalBipartiteResult res =
        graph_recognition::check_chordal_bipartite(g);
    if (!res.is_chordal_bipartite) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
