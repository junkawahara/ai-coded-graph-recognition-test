#include "caterpillar_unlabeled_enum.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    int n;
    if (!(std::cin >> n)) return 0;

    graph_recognition::CaterpillarUnlabeledEnumerationResult res =
        graph_recognition::enumerate_caterpillar_unlabeled_graphs(n);

    std::cout << res.graphs.size() << "\n";
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        const graph_recognition::CaterpillarUnlabeledEnumeratedGraph& g = res.graphs[i];
        std::cout << g.edges.size();
        for (size_t j = 0; j < g.edges.size(); ++j) {
            std::cout << " " << g.edges[j].first << " " << g.edges[j].second;
        }
        std::cout << "\n";
    }
    return 0;
}
