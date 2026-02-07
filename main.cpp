#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> p, r;
    DSU(int n = 0) { init(n); }
    void init(int n) { p.resize(n); r.assign(n, 0); iota(p.begin(), p.end(), 0); }
    int find(int x) { return p[x] == x ? x : p[x] = find(p[x]); }
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (r[a] < r[b]) swap(a, b);
        p[b] = a;
        if (r[a] == r[b]) r[a]++;
        return true;
    }
};

struct Edge {
    int w, a, b;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> g(n + 1);
    g.assign(n + 1, {});
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        if (u == v) continue;
        g[u].push_back(v);
        g[v].push_back(u);
    }

    // MCS ordering.
    vector<int> label(n + 1, 0), used(n + 1, 0);
    vector<int> order(n + 1, 0), number(n + 1, 0);
    priority_queue<pair<int, int>> pq;
    for (int v = 1; v <= n; ++v) pq.push({0, v});
    for (int i = 1; i <= n; ++i) {
        while (!pq.empty()) {
            int v = pq.top().second;
            int l = pq.top().first;
            if (used[v] || l != label[v]) { pq.pop(); continue; }
            pq.pop();
            used[v] = 1;
            order[i] = v;
            number[v] = i;
            for (int u : g[v]) {
                if (!used[u]) {
                    label[u]++;
                    pq.push({label[u], u});
                }
            }
            break;
        }
    }

    // Adjacency sets for O(1) membership checks.
    vector<unordered_set<int>> adjSet(n + 1);
    adjSet.reserve(n + 1);
    for (int v = 1; v <= n; ++v) {
        adjSet[v].reserve(g[v].size() * 2 + 1);
        for (int u : g[v]) adjSet[v].insert(u);
    }

    // Build later-neighbor lists.
    vector<vector<int>> later(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (int u : g[v]) {
            if (number[v] < number[u]) later[v].push_back(u);
        }
    }

    // Chordal check and parent computation.
    vector<int> parent(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        int best = n + 1;
        for (int u : later[v]) {
            if (number[u] < best) { best = number[u]; parent[v] = u; }
        }
        if (parent[v] == 0) continue;
        for (int u : later[v]) {
            if (u == parent[v]) continue;
            if (adjSet[parent[v]].find(u) == adjSet[parent[v]].end()) {
                cout << "NO\n";
                return 0;
            }
        }
    }

    // Maximal cliques in PEO order.
    vector<vector<int>> cliques;
    vector<int> mark(n + 1, 0);
    int stamp = 1;
    bool hasLast = false;
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        vector<int> cv;
        cv.reserve(later[v].size() + 1);
        cv.push_back(v);
        for (int u : later[v]) cv.push_back(u);
        if (hasLast) {
            bool subset = true;
            for (int x : cv) {
                if (mark[x] != stamp) { subset = false; break; }
            }
            if (subset) continue;
        }
        cliques.push_back(cv);
        stamp++;
        for (int x : cv) mark[x] = stamp;
        hasLast = true;
    }

    int k = (int)cliques.size();
    if (k == 0) {
        cout << "NO\n";
        return 0;
    }

    // Vertex -> clique membership.
    vector<vector<int>> member(n + 1);
    for (int i = 0; i < k; ++i) {
        for (int v : cliques[i]) member[v].push_back(i);
    }

    // Intersection sizes.
    vector<vector<int>> w(k, vector<int>(k, 0));
    for (int v = 1; v <= n; ++v) {
        auto &cl = member[v];
        for (size_t i = 0; i < cl.size(); ++i) {
            for (size_t j = i + 1; j < cl.size(); ++j) {
                int a = cl[i], b = cl[j];
                if (a > b) swap(a, b);
                w[a][b]++;
            }
        }
    }

    // Build maximum spanning forest of clique graph.
    vector<Edge> edges;
    edges.reserve(k * (k - 1) / 2);
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            if (w[i][j] > 0) edges.push_back({w[i][j], i, j});
        }
    }
    sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) {
        return a.w > b.w;
    });
    DSU dsu(k);
    vector<vector<int>> tree(k);
    for (auto &e : edges) {
        if (dsu.unite(e.a, e.b)) {
            tree[e.a].push_back(e.b);
            tree[e.b].push_back(e.a);
        }
    }

    // Check each component is a path and build clique order.
    vector<int> pos(k, -1);
    vector<int> seen(k, 0);
    int offset = 0;
    for (int i = 0; i < k; ++i) {
        if (seen[i]) continue;
        // Collect component.
        vector<int> comp;
        queue<int> q;
        q.push(i);
        seen[i] = 1;
        while (!q.empty()) {
            int v = q.front(); q.pop();
            comp.push_back(v);
            if ((int)tree[v].size() > 2) {
                cout << "NO\n";
                return 0;
            }
            for (int u : tree[v]) {
                if (!seen[u]) { seen[u] = 1; q.push(u); }
            }
        }
        if (comp.size() == 1) {
            pos[comp[0]] = offset + 1;
            offset += 1;
            continue;
        }
        int start = -1;
        for (int v : comp) {
            if ((int)tree[v].size() == 1) { start = v; break; }
        }
        if (start == -1) {
            cout << "NO\n";
            return 0;
        }
        int prev = -1;
        int cur = start;
        int idx = 0;
        while (cur != -1) {
            pos[cur] = offset + 1 + idx;
            idx++;
            int next = -1;
            for (int u : tree[cur]) {
                if (u != prev) { next = u; break; }
            }
            prev = cur;
            cur = next;
        }
        if (idx != (int)comp.size()) {
            cout << "NO\n";
            return 0;
        }
        offset += (int)comp.size();
    }

    // Build interval model.
    vector<int> L(n + 1, INT_MAX), R(n + 1, INT_MIN);
    for (int v = 1; v <= n; ++v) {
        for (int cid : member[v]) {
            L[v] = min(L[v], pos[cid]);
            R[v] = max(R[v], pos[cid]);
        }
        if (L[v] == INT_MAX) { L[v] = R[v] = 1; }
    }

    cout << "YES\n";
    for (int v = 1; v <= n; ++v) {
        cout << L[v] << " " << R[v] << "\n";
    }
    return 0;
}
