#include "recognizers/poset.h"
#include <iostream>
#include <utility>
#include <vector>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    int n, m;
    std::vector<std::pair<int, int>> arcs;
    n = 0;
    if (std::cin >> n >> m) {
        /* Defend against negative counts (matches read_directed) */
        if (n < 0 || m < 0) { n = 0; m = 0; }
        /* m is untrusted: cap the reserve (cf. Graph::read) */
        const int reserve_cap = 1 << 20;
        arcs.reserve(m < reserve_cap ? m : reserve_cap);
        for (int i = 0; i < m; ++i) {
            int u, v;
            if (!(std::cin >> u >> v)) break;
            arcs.push_back(std::make_pair(u, v));
        }
    }

    graph_recognition::PosetResult res = graph_recognition::check_poset(n, arcs);
    if (!res.is_poset) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
