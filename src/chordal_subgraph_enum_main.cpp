#include "enumerators/chordal_subgraph_enum.h"
#include <iostream>

namespace {

/* The output is driven by the edge count, not the vertex count: every
   subgraph of a forest is chordal, so a host with m edges can have up to
   2^m chordal subgraphs. 2^25 is about 3.4e7 outputs, the same order as
   the n = 8 limit of chordal_labeled_enum; beyond that, refuse instead of
   silently starting an unbounded computation. */
const int MAX_M = 25;

struct Count {
    unsigned long long count;
    void operator()(const graph_recognition::EnumeratedGraph&) { ++count; }
};

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

int edge_count(const graph_recognition::Graph& g) {
    int m = 0;
    for (int v = 1; v <= g.n; ++v) m += static_cast<int>(g.adj[v].size());
    return m / 2;
}

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

    const int m = edge_count(g);
    if (m > MAX_M) {
        std::cerr << "chordal_subgraph_enum: m = " << m
                  << " exceeds the supported maximum " << MAX_M
                  << " (the output would be astronomically large)\n";
        return 1;
    }

    /* The count is not known in advance and the established output format
       prints it first, so enumerate twice rather than buffer: at m = 25 the
       materialized output would not fit in memory. */
    Count counter;
    counter.count = 0;
    graph_recognition::enumerate_chordal_subgraphs_cb(g, counter);

    std::cout << counter.count << "\n";

    PrintAndCount printer;
    printer.count = 0;
    graph_recognition::enumerate_chordal_subgraphs_cb(g, printer);

    if (printer.count != counter.count) {
        std::cerr << "chordal_subgraph_enum: enumeration produced "
                  << printer.count << " subgraphs but " << counter.count
                  << " were announced -- enumerator bug\n";
        return 1;
    }
    return 0;
}
