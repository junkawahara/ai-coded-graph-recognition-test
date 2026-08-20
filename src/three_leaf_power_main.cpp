#include "three_leaf_power.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    bool input_ok = false;
    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin, input_ok);
    if (!input_ok) {
        std::cerr << "error: malformed graph input\n";
        return 1;
    }

    graph_recognition::ThreeLeafPowerResult res =
        graph_recognition::check_three_leaf_power(g);

    std::cout << (res.is_three_leaf_power ? "YES" : "NO") << "\n";
    return 0;
}
