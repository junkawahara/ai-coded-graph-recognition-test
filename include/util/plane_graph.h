#ifndef GRAPH_RECOGNITION_PLANE_GRAPH_H
#define GRAPH_RECOGNITION_PLANE_GRAPH_H

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace detail {

// A connected simple sphere embedding. Entries are zero-based neighbours
// in cyclic order; reversing EVERY row mirrors it. The canonicalizer below
// requires at least one edge and identifies embeddings, not arbitrary
// abstract planar graphs (Whitney uniqueness applies when 3-connected).
struct PlaneGraph {
    std::vector<std::vector<int> > rotation;
};

inline std::vector<std::pair<int, int> > plane_graph_edges(
    const PlaneGraph& g) {
    std::vector<std::pair<int, int> > edges;
    for (int v = 0; v < static_cast<int>(g.rotation.size()); ++v)
        for (int w : g.rotation[v])
            if (v < w) edges.push_back(std::make_pair(v + 1, w + 1));
    std::sort(edges.begin(), edges.end());
    return edges;
}

struct PlaneGraphCanon {
    PlaneGraph graph;
    std::vector<int> code;
};

// Rooted directed-edge BFS, in both orientations (Brinkmann--McKay 2007,
// section 1.3). Neighbours receive labels on discovery; 0 separates rows.
// The code reconstructs the rotation system. At most 4m rooted scans give
// a canonical embedding, including reflection, in O(n^2) time and
// O(n) temporary space. Whitney uniqueness makes this an abstract-graph
// canonical form for simple 3-connected plane graphs. Minimum degree restricts the
// roots invariantly; prefixes worse than the current best are abandoned.
inline PlaneGraphCanon plane_graph_canonicalize(
    const PlaneGraph& g) {
    const int n = static_cast<int>(g.rotation.size());
    PlaneGraphCanon best;
    std::size_t min_degree = g.rotation[0].size();
    for (const auto& row : g.rotation) min_degree = std::min(min_degree, row.size());
    for (int root = 0; root < n; ++root) {
        if (g.rotation[root].size() != min_degree) continue;
        for (int first = 0; first < static_cast<int>(g.rotation[root].size()); ++first) {
            for (int direction = -1; direction <= 1; direction += 2) {
                std::vector<int> labels(n, -1), anchors(n, 0), order(1, root);
                labels[root] = 0;
                anchors[root] = first;
                std::vector<int> code;
                code.reserve(7 * n);
                bool smaller = best.code.empty(), worse = false;
                for (int i = 0; i < n && !worse; ++i) {
                    const int v = order[i];
                    const int degree = static_cast<int>(g.rotation[v].size());
                    for (int j = 0; j < degree; ++j) {
                        const int pos = (anchors[v] + direction * j + degree) % degree;
                        const int w = g.rotation[v][pos];
                        if (labels[w] < 0) {
                            labels[w] = static_cast<int>(order.size());
                            order.push_back(w);
                            anchors[w] = static_cast<int>(
                                std::find(g.rotation[w].begin(), g.rotation[w].end(), v)
                                - g.rotation[w].begin());
                        }
                        const int entry = labels[w] + 1;
                        if (!smaller) {
                            if (entry > best.code[code.size()]) { worse = true; break; }
                            smaller = entry < best.code[code.size()];
                        }
                        code.push_back(entry);
                    }
                    if (worse) break;
                    if (!smaller) smaller = best.code[code.size()] > 0;
                    code.push_back(0);
                }
                if (!worse && smaller) best.code = std::move(code);
            }
        }
    }
    best.graph.rotation.resize(n);
    int row = 0;
    for (int entry : best.code) {
        if (entry == 0) ++row;
        else best.graph.rotation[row].push_back(entry - 1);
    }
    return best;
}

}  // namespace detail
}  // namespace graph_recognition

#endif
