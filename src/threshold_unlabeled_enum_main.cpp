#include "enumerators/threshold_unlabeled_enum.h"
#include <iostream>

namespace {

/* Each of the 2^(n-1) graphs comes from one binary string of length n-1, so
   the bitmask stops at n = 63. Above that the enumerator returns an empty
   result, which printed as a count would read as "no threshold graph has 64
   vertices" -- false for every n, so refuse instead. */
const int MAX_N = 63;

struct PrintAndCount {
    unsigned long long count;
    void operator()(const graph_recognition::ThresholdUnlabeledEnumeratedGraph& g) {
        ++count;
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
    if (!(std::cin >> n)) {
        std::cerr << "threshold_unlabeled_enum: expected a vertex count on stdin\n";
        return 1;
    }
    if (n < 0 || n > MAX_N) {
        std::cerr << "threshold_unlabeled_enum: n = " << n
                  << " is outside the supported range 0.." << MAX_N << "\n";
        return 1;
    }

    /* The count is 2^(n-1) by construction, so the established "count first"
       format costs neither a counting pass nor holding the output: the graphs
       are streamed straight to stdout. */
    const unsigned long long expected = (n == 0) ? 1ULL : (1ULL << (n - 1));
    std::cout << expected << "\n";

    PrintAndCount printer;
    printer.count = 0;
    graph_recognition::enumerate_threshold_unlabeled_graphs_cb(n, printer);

    if (printer.count != expected) {
        std::cerr << "threshold_unlabeled_enum: enumeration produced "
                  << printer.count << " graphs but " << expected
                  << " were announced -- enumerator bug\n";
        return 1;
    }
    return 0;
}
