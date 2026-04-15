#include "caterpillar.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);

    graph_recognition::CaterpillarResult res = graph_recognition::check_caterpillar(g);
    if (!res.is_caterpillar) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
