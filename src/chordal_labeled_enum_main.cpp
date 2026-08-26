#include "chordal_labeled_enum.h"
#include <iostream>

namespace {

/* The number of labeled chordal graphs explodes super-exponentially
   (n = 7: 617675, n = 8: about 3.1e7, n = 9: about 2.2e9). Beyond n = 8 the
   enumeration would effectively never terminate, so refuse instead of
   silently starting an unbounded computation. */
const int MAX_N = 8;

/* Labeled chordal graph counts for n = 0..MAX_N (OEIS A058862). The
   established output format prints the count before the graphs; taking it
   from this table keeps that format with a single streaming enumeration
   pass (the output at n = 8 is ~1.6 GB, so buffering it is not an option,
   and a separate counting pass doubled the runtime). The streamed count
   is verified against the table after the pass. */
const unsigned long long CHORDAL_COUNTS[MAX_N + 1] = {
    1ULL, 1ULL, 2ULL, 8ULL, 61ULL, 822ULL, 18154ULL, 617675ULL, 30888596ULL};

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
    if (!(std::cin >> n)) return 0;
    if (n > MAX_N) {
        std::cerr << "chordal_labeled_enum: n = " << n << " exceeds the supported maximum "
                  << MAX_N << " (the output would be astronomically large)\n";
        return 1;
    }
    if (n < 0) {
        std::cout << 0 << "\n";
        return 0;
    }

    std::cout << CHORDAL_COUNTS[n] << "\n";

    PrintAndCount printer;
    printer.count = 0;
    graph_recognition::enumerate_chordal_labeled_graphs_reverse_search_cb(n, printer);

    if (printer.count != CHORDAL_COUNTS[n]) {
        std::cerr << "chordal_labeled_enum: enumeration produced " << printer.count
                  << " graphs but " << CHORDAL_COUNTS[n]
                  << " were announced -- enumerator bug\n";
        return 1;
    }
    return 0;
}
