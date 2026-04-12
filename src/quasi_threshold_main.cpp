#include "quasi_threshold.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);

    graph_recognition::QuasiThresholdResult res =
        graph_recognition::check_quasi_threshold(g);
    if (!res.is_quasi_threshold) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
