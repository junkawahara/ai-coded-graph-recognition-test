#include "biconvex_bipartite.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);

    graph_recognition::BiconvexBipartiteResult res =
        graph_recognition::check_biconvex_bipartite(g);
    if (!res.is_biconvex_bipartite) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
