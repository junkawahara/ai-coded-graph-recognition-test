#include "simple_quadrangulation.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);

    graph_recognition::SimpleQuadrangulationResult res = graph_recognition::check_simple_quadrangulation(g);
    if (!res.is_simple_quadrangulation) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
