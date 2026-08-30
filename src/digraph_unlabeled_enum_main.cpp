#include "enumerators/digraph_unlabeled_enum.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    int n;
    if (!(std::cin >> n)) return 0;

    graph_recognition::DigraphUnlabeledEnumerationResult res =
        graph_recognition::enumerate_digraph_unlabeled_graphs(n);

    std::cout << res.graphs.size() << "\n";
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        const graph_recognition::DigraphUnlabeledEnumeratedGraph& g = res.graphs[i];
        std::cout << g.arcs.size();
        for (size_t j = 0; j < g.arcs.size(); ++j) {
            std::cout << " " << g.arcs[j].first << " " << g.arcs[j].second;
        }
        std::cout << "\n";
    }
    return 0;
}
