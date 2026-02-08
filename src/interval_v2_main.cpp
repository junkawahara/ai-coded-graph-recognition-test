#include "interval_v2.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);
    if (g.n == 0) return 0;

    graph_recognition::IntervalResult res = graph_recognition::check_interval_v2(g);
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
