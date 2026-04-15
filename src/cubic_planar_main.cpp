#include "cubic_planar.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);

    graph_recognition::CubicPlanarResult res = graph_recognition::check_cubic_planar(g);
    if (!res.is_cubic_planar) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
