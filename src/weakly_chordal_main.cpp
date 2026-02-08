#include "weakly_chordal.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);
    if (g.n == 0) return 0;

    graph_recognition::WeaklyChordalResult res =
        graph_recognition::check_weakly_chordal(g);
    if (!res.is_weakly_chordal) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
