#include "bipartite_permutation.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);
    if (g.n == 0) return 0;

    graph_recognition::BipartitePermutationResult res =
        graph_recognition::check_bipartite_permutation(g);
    if (!res.is_bipartite_permutation) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
