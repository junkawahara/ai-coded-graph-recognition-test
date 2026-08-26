#include "enumerators/proper_chordal_labeled_enum.h"
#include <iostream>

namespace {

/* n=7 already emits roughly six hundred thousand graphs.  Refuse larger
   instances in the CLI while leaving the header API unrestricted. */
const int MAX_N = 7;

/* Labeled proper chordal graph counts for n=0..7.  The fixed output format
   puts the count first, so this table permits a single streaming pass. */
const unsigned long long PROPER_CHORDAL_COUNTS[MAX_N + 1] = {
    1ULL, 1ULL, 2ULL, 8ULL, 61ULL, 822ULL, 18034ULL, 595415ULL};

struct PrintAndCount {
    unsigned long long count;

    void operator()(const graph_recognition::EnumeratedGraph& graph) {
        ++count;
        std::cout << graph.edges.size();
        for (std::size_t i = 0; i < graph.edges.size(); ++i) {
            std::cout << " " << graph.edges[i].first
                      << " " << graph.edges[i].second;
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
        std::cerr << "proper_chordal_labeled_enum: n = " << n
                  << " exceeds the supported maximum " << MAX_N
                  << " (the output would be too large)\n";
        return 1;
    }
    if (n < 0) {
        std::cout << 0 << "\n";
        return 0;
    }

    std::cout << PROPER_CHORDAL_COUNTS[n] << "\n";
    PrintAndCount printer;
    printer.count = 0;
    graph_recognition::enumerate_proper_chordal_labeled_graphs_reverse_search_cb(
        n, printer);

    if (printer.count != PROPER_CHORDAL_COUNTS[n]) {
        std::cerr << "proper_chordal_labeled_enum: enumeration produced "
                  << printer.count << " graphs but "
                  << PROPER_CHORDAL_COUNTS[n]
                  << " were announced -- enumerator bug\n";
        return 1;
    }
    return 0;
}
