#include "polyhedral.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);

    graph_recognition::PolyhedralResult res = graph_recognition::check_polyhedral(g);
    if (!res.is_polyhedral) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
