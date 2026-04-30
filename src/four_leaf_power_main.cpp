#include "four_leaf_power.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);

    graph_recognition::FourLeafPowerResult res =
        graph_recognition::check_four_leaf_power(g);

    std::cout << (res.is_four_leaf_power ? "YES" : "NO") << "\n";
    return 0;
}
