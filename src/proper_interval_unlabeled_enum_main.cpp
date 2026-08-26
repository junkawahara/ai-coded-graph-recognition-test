#include "proper_interval_unlabeled_enum.h"
#include <cstring>
#include <iostream>

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    bool connected_only = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--connected") == 0) connected_only = true;
    }

    int n;
    if (!(std::cin >> n)) return 0;

    graph_recognition::ProperIntervalUnlabeledEnumerationResult res =
        graph_recognition::enumerate_proper_interval_unlabeled_graphs(n, connected_only);

    std::cout << res.graphs.size() << "\n";
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        const graph_recognition::ProperIntervalUnlabeledEnumeratedGraph& g = res.graphs[i];
        std::cout << g.edges.size();
        for (size_t j = 0; j < g.edges.size(); ++j) {
            std::cout << " " << g.edges[j].first << " " << g.edges[j].second;
        }
        std::cout << "\n";
    }
    return 0;
}
