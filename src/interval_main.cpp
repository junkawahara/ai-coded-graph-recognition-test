#include "interval.h"
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

    graph_recognition::IntervalResult res = graph_recognition::check_interval(g);
    if (!res.is_interval) {
        std::cout << "NO\n";
        return 0;
    }

    std::cout << "YES\n";
    for (int v = 1; v <= g.n; ++v) {
        std::cout << res.intervals[v].first << " " << res.intervals[v].second << "\n";
    }
    return 0;
}
