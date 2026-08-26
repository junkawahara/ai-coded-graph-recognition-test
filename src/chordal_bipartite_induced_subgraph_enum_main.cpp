#include "chordal_bipartite_induced_subgraph_enum.h"
#include <iostream>
#include <vector>

namespace {

/* The output is driven by the vertex count, not the edge count: the class is
   hereditary, so a chordal bipartite host has all 2^n vertex subsets as
   solutions regardless of how many edges it has. 2^25 is about 3.4e7
   outputs, the same order as the m = 25 limit of chordal_subgraph_enum;
   beyond that, refuse instead of silently starting an unbounded
   computation. */
const int MAX_N = 25;

struct Count {
    unsigned long long count;
    void operator()(const std::vector<int>&) { ++count; }
};

struct PrintAndCount {
    unsigned long long count;
    void operator()(const std::vector<int>& vertices) {
        ++count;
        std::cout << vertices.size();
        for (size_t i = 0; i < vertices.size(); ++i) {
            std::cout << " " << vertices[i];
        }
        std::cout << "\n";
    }
};

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    bool input_ok = false;
    graph_recognition::Graph g =
        graph_recognition::Graph::read(std::cin, input_ok);
    if (!input_ok) {
        std::cerr << "error: malformed graph input\n";
        return 1;
    }

    if (g.n > MAX_N) {
        std::cerr << "chordal_bipartite_induced_subgraph_enum: n = " << g.n
                  << " exceeds the supported maximum " << MAX_N
                  << " (the output would be astronomically large)\n";
        return 1;
    }

    /* The count is not known in advance and the established output format
       prints it first, so enumerate twice rather than buffer: at n = 25 the
       materialized output would not fit in memory. */
    Count counter;
    counter.count = 0;
    graph_recognition::enumerate_chordal_bipartite_induced_subgraphs_cb(g, counter);

    std::cout << counter.count << "\n";

    PrintAndCount printer;
    printer.count = 0;
    graph_recognition::enumerate_chordal_bipartite_induced_subgraphs_cb(g, printer);

    if (printer.count != counter.count) {
        std::cerr << "chordal_bipartite_induced_subgraph_enum: enumeration produced "
                  << printer.count << " subgraphs but " << counter.count
                  << " were announced -- enumerator bug\n";
        return 1;
    }
    return 0;
}
