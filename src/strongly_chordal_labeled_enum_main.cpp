#include "enumerators/strongly_chordal_labeled_enum.h"
#include <iostream>

namespace {

/* The enumeration grows super-exponentially and each candidate edge costs a
   Farber partial-order construction: n = 7 already takes ~14 s for 598,775
   graphs and n = 8 runs for hours. Refuse beyond the range whose counts are
   verified rather than silently starting an unbounded computation. */
const int MAX_N = 7;

/* Labeled strongly chordal graph counts for n = 0..MAX_N. The established
   output format prints the count before the graphs; taking it from this table
   keeps that format with a single streaming pass, so the CLI never holds the
   whole enumeration in memory (n = 7 materialized is ~80 MiB). The streamed
   count is checked against the table afterwards. */
const unsigned long long STRONGLY_CHORDAL_COUNTS[MAX_N + 1] = {
    1ULL, 1ULL, 2ULL, 8ULL, 61ULL, 822ULL, 18034ULL, 598775ULL};

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
        std::cerr << "strongly_chordal_labeled_enum: expected a vertex count on stdin\n";
        return 1;
    }
    if (n < 0 || n > MAX_N) {
        std::cerr << "strongly_chordal_labeled_enum: n = " << n
                  << " is outside the supported range 0.." << MAX_N
                  << " (beyond it the enumeration runs for hours)\n";
        return 1;
    }

    std::cout << STRONGLY_CHORDAL_COUNTS[n] << "\n";

    PrintAndCount printer;
    printer.count = 0;
    graph_recognition::enumerate_strongly_chordal_labeled_graphs_reverse_search_cb(
        n, printer);

    if (printer.count != STRONGLY_CHORDAL_COUNTS[n]) {
        std::cerr << "strongly_chordal_labeled_enum: enumeration produced "
                  << printer.count << " graphs but "
                  << STRONGLY_CHORDAL_COUNTS[n]
                  << " were announced -- enumerator bug\n";
        return 1;
    }
    return 0;
}
