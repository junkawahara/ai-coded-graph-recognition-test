#include "enumerators/split_labeled_enum.h"
#include <iostream>

namespace {

/* The number of labeled split graphs grows super-exponentially (n = 8:
   5,843,954; n = 9: 233,064,944, whose edge lists alone are gigabytes of
   output). Refuse beyond the range whose counts are verified rather than
   silently starting an unbounded computation. */
const int MAX_N = 8;

/* Labeled split graph counts for n = 0..MAX_N (OEIS A179534 from n = 2). The
   established output format prints the count before the graphs; taking it
   from this table keeps that format with a single streaming pass, so the CLI
   never holds the whole enumeration in memory. The streamed count is checked
   against the table afterwards. */
const unsigned long long SPLIT_COUNTS[MAX_N + 1] = {
    1ULL, 1ULL, 2ULL, 8ULL, 58ULL, 632ULL, 9654ULL, 202484ULL, 5843954ULL};

struct PrintAndCount {
    unsigned long long count;
    void operator()(const graph_recognition::EnumeratedGraph& g) {
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
        std::cerr << "split_labeled_enum: expected a vertex count on stdin\n";
        return 1;
    }
    if (n < 0 || n > MAX_N) {
        std::cerr << "split_labeled_enum: n = " << n
                  << " is outside the supported range 0.." << MAX_N
                  << " (beyond it the output would be astronomically large)\n";
        return 1;
    }

    std::cout << SPLIT_COUNTS[n] << "\n";

    PrintAndCount printer;
    printer.count = 0;
    graph_recognition::enumerate_split_labeled_graphs_reverse_search_cb(n, printer);

    if (printer.count != SPLIT_COUNTS[n]) {
        std::cerr << "split_labeled_enum: enumeration produced " << printer.count
                  << " graphs but " << SPLIT_COUNTS[n]
                  << " were announced -- enumerator bug\n";
        return 1;
    }
    return 0;
}
