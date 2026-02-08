#include "distance_hereditary_v2.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);
    if (g.n == 0) return 0;

    graph_recognition::DistanceHereditaryResult res =
        graph_recognition::check_distance_hereditary_v2(g);
    if (!res.is_distance_hereditary) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
