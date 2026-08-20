#include "chordal_enum.h"
#include <iostream>

namespace {

/* The number of labeled chordal graphs explodes super-exponentially
   (n = 7: 617675, n = 8: about 3.1e7, n = 9: about 2.2e9). Beyond n = 8 the
   enumeration would effectively never terminate, so refuse instead of
   silently starting an unbounded computation. */
const int MAX_N = 8;

struct CountGraphs {
    unsigned long long count;
    void operator()(const graph_recognition::EnumeratedGraph&) { ++count; }
};

struct PrintGraph {
    void operator()(const graph_recognition::EnumeratedGraph& g) {
        std::cout << g.edges.size();
        for (size_t j = 0; j < g.edges.size(); ++j) {
            std::cout << " " << g.edges[j].first << " " << g.edges[j].second;
        }
        std::cout << "\n";
    }
};

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    int n;
    if (!(std::cin >> n)) return 0;
    if (n > MAX_N) {
        std::cerr << "chordal_enum: n = " << n << " exceeds the supported maximum "
                  << MAX_N << " (the output would be astronomically large)\n";
        return 1;
    }

    /* Two streaming passes (count, then graphs) keep memory at O(n^2)
       instead of materializing all graphs. The count is printed first to
       preserve the established output format. */
    CountGraphs counter;
    counter.count = 0;
    graph_recognition::enumerate_chordal_graphs_reverse_search_cb(n, counter);
    std::cout << counter.count << "\n";

    PrintGraph printer;
    graph_recognition::enumerate_chordal_graphs_reverse_search_cb(n, printer);
    return 0;
}
