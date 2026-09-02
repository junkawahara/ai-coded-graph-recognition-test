#include "enumerators/split_unlabeled_enum.h"
#include <cstring>
#include <iostream>

namespace {

/* The enumerator keeps each vertex's neighbourhood in one 64-bit word, so
   n >= 64 is outside its range and it returns an empty result there. Printed
   as a count that would read as "no split graph has 64 vertices", which is
   false for every n (the edgeless graph alone is split), so refuse instead. */
const int MAX_N = 63;

}  // namespace

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    bool connected_only = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--connected") == 0) {
            connected_only = true;
        } else {
            std::cerr << "split_unlabeled_enum: unknown option " << argv[i]
                      << " (only --connected is accepted)\n";
            return 1;
        }
    }

    int n;
    if (!(std::cin >> n)) {
        std::cerr << "split_unlabeled_enum: expected a vertex count on stdin\n";
        return 1;
    }
    if (n < 0 || n > MAX_N) {
        std::cerr << "split_unlabeled_enum: n = " << n
                  << " is outside the supported range 0.." << MAX_N << "\n";
        return 1;
    }

    graph_recognition::SplitUnlabeledEnumerationResult res =
        graph_recognition::enumerate_split_unlabeled_graphs(n, connected_only);

    std::cout << res.graphs.size() << "\n";
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        const graph_recognition::SplitUnlabeledEnumeratedGraph& g = res.graphs[i];
        std::cout << g.edges.size();
        for (size_t j = 0; j < g.edges.size(); ++j) {
            std::cout << " " << g.edges[j].first << " " << g.edges[j].second;
        }
        std::cout << "\n";
    }
    return 0;
}
