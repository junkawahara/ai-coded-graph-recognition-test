#include "recognizers/tournament.h"
#include <iostream>
#include <utility>
#include <vector>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    int n;
    std::vector<std::pair<int, int>> arcs;
    graph_recognition::read_directed(std::cin, n, arcs);

    graph_recognition::TournamentResult res = graph_recognition::check_tournament(n, arcs);
    if (!res.is_tournament) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
