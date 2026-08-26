#include "recognizers/circle.h"
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

    graph_recognition::CircleResult res;
    try {
        res = graph_recognition::check_circle(g);
    } catch (const std::exception& e) {
        /* Resource limit exhausted: the answer is unknown, which is
         * different from NO -- report it as an error, not as a result. */
        std::cerr << e.what() << "\n";
        return 2;
    }
    if (!res.is_circle) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
