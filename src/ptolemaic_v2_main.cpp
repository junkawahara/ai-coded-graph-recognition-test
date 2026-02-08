#include "chordal.h"
#include "distance_hereditary_v2.h"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    graph_recognition::Graph g = graph_recognition::Graph::read(std::cin);
    if (g.n == 0) return 0;

    graph_recognition::ChordalResult chordal = graph_recognition::check_chordal(g);
    if (!chordal.is_chordal) {
        std::cout << "NO\n";
        return 0;
    }

    graph_recognition::DistanceHereditaryResult dh =
        graph_recognition::check_distance_hereditary_v2(g);
    if (!dh.is_distance_hereditary) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
