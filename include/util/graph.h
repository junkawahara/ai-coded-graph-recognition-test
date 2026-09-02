#ifndef GRAPH_RECOGNITION_GRAPH_H
#define GRAPH_RECOGNITION_GRAPH_H

/**
 * @file graph.h
 * @brief Basic graph data structure
 *
 * Represents a 1-indexed undirected graph using adjacency lists and adjacency sets.
 */

#include <iostream>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 1-indexed undirected graph (adjacency list + adjacency set)
 *
 * Vertex numbers range from 1 to n. Both adjacency list (adj) and adjacency set
 * (adj_set) are maintained for efficient traversal and membership queries.
 */
struct Graph {
    int n; /**< Number of vertices */
    std::vector<std::vector<int>> adj;            /**< Adjacency list (1-indexed) */
    std::vector<std::unordered_set<int>> adj_set; /**< Adjacency set (1-indexed) */

    /** @brief Default constructor (empty graph) */
    Graph() : n(0), adj(1), adj_set(1) {}

    /**
     * @brief Constructs a graph from vertex count and edge list
     * @param num_vertices Number of vertices; a negative count is clamped to 0
     * @param edges Edge list (1-indexed vertex pairs)
     *
     * Self-loops and multi-edges are automatically ignored.
     *
     * A negative vertex count would leave adj/adj_set empty while n stayed
     * negative, and every `for (v = 1; v <= n; ++v)` guard downstream would
     * then pass an empty container to an unguarded `[0]` write; read() already
     * maps a negative header to the empty graph, so the constructor does too.
     */
    Graph(int num_vertices, const std::vector<std::pair<int, int>>& edges)
        : n(num_vertices < 0 ? 0 : num_vertices), adj(n + 1), adj_set(n + 1) {
        for (size_t i = 0; i < edges.size(); ++i) {
            int u = edges[i].first, v = edges[i].second;
            if (u < 1 || u > n || v < 1 || v > n) continue;
            if (u == v) continue;
            if (adj_set[u].count(v)) continue;
            adj_set[u].insert(v);
            adj_set[v].insert(u);
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    /**
     * @brief Checks whether an edge exists
     * @param u Vertex u
     * @param v Vertex v
     * @return true if edge (u, v) exists
     */
    bool has_edge(int u, int v) const {
        if (u < 1 || u > n || v < 1 || v > n) return false;
        return adj_set[u].count(v) > 0;
    }

    /** @brief Largest vertex count read() accepts. A bogus header such as
     *         n = INT_MAX would otherwise allocate gigabytes for adj/adj_set
     *         (or overflow into an uncaught std::length_error) before any
     *         algorithm runs. (enum: usable without an out-of-class
     *         definition under C++11) */
    enum ReadLimit { max_read_vertices = 1000000 };

    /**
     * @brief Reads a graph from an input stream
     * @param in Input stream
     * @return The graph read from the stream
     *
     * Input format: first line contains n m, followed by m lines each with edge u v.
     * Malformed input yields an empty or truncated graph indistinguishable from
     * a valid one; use the two-argument overload to detect input errors.
     */
    static Graph read(std::istream& in) {
        bool ok;
        return read(in, ok);
    }

    /**
     * @brief Reads a graph from an input stream, reporting input validity
     * @param in Input stream
     * @param ok Set to true iff the header is well-formed (0 <= n <=
     *           max_read_vertices, m >= 0), all m edge lines were read, and
     *           every edge is a non-loop pair inside [1, n]. Duplicate edges
     *           are permitted (they describe the same simple graph).
     * @return The graph read from the stream (on failure, the edges read so far)
     */
    static Graph read(std::istream& in, bool& ok) {
        ok = false;
        int n, m;
        if (!(in >> n >> m)) return Graph();
        if (n < 0 || m < 0 || n > max_read_vertices) return Graph();
        std::vector<std::pair<int, int>> edges;
        // m comes from untrusted input: cap the reserve so a bogus header
        // (e.g. m = INT_MAX) cannot request gigabytes up front. Beyond the
        // cap, push_back grows the vector amortized as usual.
        const int reserve_cap = 1 << 20;
        edges.reserve(m < reserve_cap ? m : reserve_cap);
        bool edges_ok = true;
        for (int i = 0; i < m; ++i) {
            int u, v;
            if (!(in >> u >> v)) return Graph(n, edges);
            // The constructor silently drops out-of-range and self-loop
            // edges; report them here so callers can tell "valid input"
            // from "answer about a different graph than the file describes".
            if (u < 1 || u > n || v < 1 || v > n || u == v) edges_ok = false;
            edges.push_back(std::make_pair(u, v));
        }
        ok = edges_ok;
        return Graph(n, edges);
    }
};

} // namespace graph_recognition

#endif
