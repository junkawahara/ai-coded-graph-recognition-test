#include "recognizers/five_leaf_power.h"
#include <exception>
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

    graph_recognition::FiveLeafPowerResult res;
    try {
        res = graph_recognition::check_five_leaf_power(g);
    } catch (const std::exception& e) {
        /* Budget / memory guard exhausted: the answer is unknown, which is
         * different from NO -- report it as an error, not as a result. */
        std::cerr << e.what() << "\n";
        return 2;
    }

    std::cout << (res.is_five_leaf_power ? "YES" : "NO") << "\n";
    return 0;
}
