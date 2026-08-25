"""Checks the NO certificates returned by recognize_<type>.

The verification here re-derives each pattern from its definition against the
edge list the caller passed in, sharing no code with the C++ side, so a bug in
the extraction cannot make its own check pass. It is the Python-side mirror of
verify_obstruction() in tests/gtest/helpers/certificates.cpp.
"""

import itertools
import os

import pytest

import graph_recognition as gr
from graph_recognition import _core

CERTIFIED_TYPES = sorted(_core._certified_types())

# Edges the pattern must have, as index pairs into the certificate's vertex
# list; every other pair must be a non-edge. Restated from the definitions
# rather than imported from anywhere.
PATTERN_EDGES = {
    "triangle": [(0, 1), (1, 2), (0, 2)],
    "p3": [(0, 1), (1, 2)],
    "p4": [(0, 1), (1, 2), (2, 3)],
    "p5": [(0, 1), (1, 2), (2, 3), (3, 4)],
    "2k2": [(0, 1), (2, 3)],
    "claw": [(0, 1), (0, 2), (0, 3)],
    "diamond": [(0, 1), (0, 2), (0, 3), (1, 2), (1, 3)],
    "bull": [(0, 1), (1, 2), (0, 2), (0, 3), (1, 4)],
    "gem": [(0, 1), (1, 2), (2, 3), (4, 0), (4, 1), (4, 2), (4, 3)],
}

# (minimum length, required parity or None, chordless, maximum chords)
CYCLE_RULES = {
    "hole": (4, None, True),
    "odd_cycle": (3, 1, False),
    "odd_hole": (5, 1, True),
    "even_hole": (4, 0, True),
    "c4": (4, 0, True),
    "c5": (5, 1, True),
}

MINOR_PAIRS = {
    "k5_minor": (5, list(itertools.combinations(range(5), 2))),
    "k33_minor": (6, [(a, b) for a in range(3) for b in range(3, 6)]),
}


class Adjacency(object):
    """Adjacency lookup that inverts itself for complement certificates."""

    def __init__(self, n, edges, in_complement):
        self.n = n
        self.in_complement = in_complement
        self.edges = set()
        for u, v in edges:
            self.edges.add((min(u, v), max(u, v)))

    def __call__(self, u, v):
        if u == v:
            return False
        present = (min(u, v), max(u, v)) in self.edges
        return not present if self.in_complement else present

    def reachable(self, start, target, blocked):
        if start in blocked or target in blocked:
            return False
        seen = {start}
        stack = [start]
        while stack:
            cur = stack.pop()
            if cur == target:
                return True
            for w in range(1, self.n + 1):
                if w in seen or w in blocked or not self(cur, w):
                    continue
                seen.add(w)
                stack.append(w)
        return target in seen

    def components(self, blocked):
        seen = set()
        count = 0
        for s in range(1, self.n + 1):
            if s in blocked or s in seen:
                continue
            count += 1
            stack = [s]
            seen.add(s)
            while stack:
                cur = stack.pop()
                for w in range(1, self.n + 1):
                    if w in seen or w in blocked or not self(cur, w):
                        continue
                    seen.add(w)
                    stack.append(w)
        return count

    def distance(self, u, v):
        if u == v:
            return 0
        dist = {u: 0}
        queue = [u]
        for cur in queue:
            for w in range(1, self.n + 1):
                if w in dist or not self(cur, w):
                    continue
                dist[w] = dist[cur] + 1
                if w == v:
                    return dist[w]
                queue.append(w)
        return -1


def _distinct_in_range(n, verts):
    return len(set(verts)) == len(verts) and all(1 <= v <= n for v in verts)


def _check_cycle(adj, cycle, min_len, parity, chordless, max_chords=0):
    k = len(cycle)
    if k < max(3, min_len) or not _distinct_in_range(adj.n, cycle):
        return False
    if parity is not None and k % 2 != parity:
        return False
    if any(not adj(cycle[i], cycle[(i + 1) % k]) for i in range(k)):
        return False
    chords = 0
    for i in range(k):
        for j in range(i + 2, k):
            if i == 0 and j == k - 1:
                continue  # the closing edge, not a chord
            if adj(cycle[i], cycle[j]):
                chords += 1
    return chords == 0 if chordless else chords <= max_chords


def _check_induced_path(adj, path):
    if len(path) < 2 or not _distinct_in_range(adj.n, path):
        return False
    if any(not adj(path[i], path[i + 1]) for i in range(len(path) - 1)):
        return False
    return all(
        not adj(path[i], path[j])
        for i in range(len(path))
        for j in range(i + 2, len(path))
    )


def verify(n, edges, cert):
    """Re-derives the certificate's own definition. Returns True if it holds."""
    kind = cert["kind"]
    verts = cert["vertices"]
    sets = cert["vertex_sets"]
    adj = Adjacency(n, edges, cert["in_complement"])

    if kind in PATTERN_EDGES:
        want = PATTERN_EDGES[kind]
        size = max(max(p) for p in want) + 1
        if len(verts) != size or not _distinct_in_range(n, verts):
            return False
        required = {(min(a, b), max(a, b)) for a, b in want}
        for i in range(size):
            for j in range(i + 1, size):
                if adj(verts[i], verts[j]) != ((i, j) in required):
                    return False
        return True

    if kind in CYCLE_RULES:
        min_len, parity, chordless = CYCLE_RULES[kind]
        if kind in ("c4", "c5") and len(verts) != min_len:
            return False
        return _check_cycle(adj, verts, min_len, parity, chordless)

    if kind == "odd_cycle_le1_chord":
        return _check_cycle(adj, verts, 5, 1, False, max_chords=1)

    if kind == "asteroidal_triple":
        if len(verts) != 3 or not _distinct_in_range(n, verts):
            return False
        if any(adj(a, b) for a, b in itertools.combinations(verts, 2)):
            return False
        for k in range(3):
            z, x, y = verts[k], verts[(k + 1) % 3], verts[(k + 2) % 3]
            blocked = {z} | {w for w in range(1, n + 1) if adj(z, w)}
            if not adj.reachable(x, y, blocked):
                return False
        return True

    if kind == "cut_vertex":
        if len(verts) != 1 or not _distinct_in_range(n, verts):
            return False
        return adj.components({verts[0]}) > adj.components(set())

    if kind == "disconnected_pair":
        if len(verts) != 2 or not _distinct_in_range(n, verts):
            return False
        return not adj.reachable(verts[0], verts[1], set())

    if kind == "forcing_cycle":
        if len(verts) < 6 or len(verts) % 2:
            return False
        arcs = [(verts[i], verts[i + 1]) for i in range(0, len(verts), 2)]
        if any(not adj(a, b) for a, b in arcs):
            return False
        for (a, b), (c, d) in zip(arcs, arcs[1:]):
            tail_step = a == c and b != d and not adj(b, d)
            head_step = b == d and a != c and not adj(a, c)
            if not (tail_step or head_step):
                return False
        return arcs[-1] == (arcs[0][1], arcs[0][0])

    if kind in MINOR_PAIRS:
        count, pairs = MINOR_PAIRS[kind]
        if len(sets) != count or any(not s for s in sets):
            return False
        flat = [v for s in sets for v in s]
        if not _distinct_in_range(n, flat):
            return False
        for s in sets:
            if any(not adj.reachable(s[0], v, set(range(1, n + 1)) - set(s))
                   for v in s[1:]):
                return False
        return all(
            any(adj(x, y) for x in sets[a] for y in sets[b]) for a, b in pairs
        )

    if kind == "two_cycles_sharing_edge":
        if len(sets) != 2 or len(verts) != 2 or not adj(verts[0], verts[1]):
            return False
        shared = (min(verts), max(verts))
        edge_sets = []
        for cycle in sets:
            if not _check_cycle(adj, cycle, 3, None, False, max_chords=n * n):
                return False
            es = {
                (min(cycle[i], cycle[(i + 1) % len(cycle)]),
                 max(cycle[i], cycle[(i + 1) % len(cycle)]))
                for i in range(len(cycle))
            }
            if shared not in es:
                return False
            edge_sets.append(es)
        return edge_sets[0] != edge_sets[1]

    if kind == "induced_path_wrong_parity":
        if len(verts) != 2 or len(sets) != 2:
            return False
        u, v = verts
        for path in sets:
            if not _check_induced_path(adj, path) or path[0] != u or path[-1] != v:
                return False
        return (len(sets[0]) - 1) % 2 != (len(sets[1]) - 1) % 2

    if kind == "non_shortest_induced_path":
        if not _check_induced_path(adj, verts):
            return False
        d = adj.distance(verts[0], verts[-1])
        return d >= 0 and len(verts) - 1 > d

    return False


# A spread of small graphs; between them every certified class answers NO on
# at least one, which the coverage test below asserts rather than assumes.
SAMPLE_GRAPHS = [
    (4, [(1, 2), (2, 3), (3, 4)]),                                    # P4
    (4, [(1, 2), (2, 3), (3, 4), (4, 1)]),                            # C4
    (5, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 1)]),                    # C5
    (6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 1)]),            # C6
    (7, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 1)]),    # C7
    (4, [(1, 2), (1, 3), (1, 4)]),                                    # claw
    (5, list(itertools.combinations(range(1, 6), 2))),                # K5
    (6, [(a, b) for a in (1, 2, 3) for b in (4, 5, 6)]),              # K3,3
    (5, [(1, 2), (1, 3), (1, 4), (1, 5), (2, 3), (4, 5)]),            # bowtie
    (6, [(1, 2), (2, 3), (3, 1), (3, 4), (4, 5), (5, 6), (6, 4)]),    # two triangles
    (5, [(1, 2), (2, 3), (3, 4), (4, 1), (1, 3), (2, 5)]),            # diamond + pendant
    (6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6)]),                    # P6
    (7, [(1, 2), (1, 3), (1, 4), (2, 5), (3, 6), (4, 7)]),            # net-like tree
    (5, [(1, 2), (2, 3), (1, 3), (1, 4), (2, 5)]),                    # bull
    (5, [(1, 2), (2, 3), (3, 4), (5, 1), (5, 2), (5, 3), (5, 4)]),    # gem
]


@pytest.mark.parametrize("type_name", CERTIFIED_TYPES)
def test_no_certificates_are_valid(type_name):
    rec_fn = getattr(gr, "recognize_{}".format(type_name))
    saw_no = False
    for n, edges in SAMPLE_GRAPHS:
        member, cert = rec_fn(n, edges)
        if member:
            assert cert is None
            continue
        saw_no = True
        assert cert is not None, "{} gave no witness on n={}".format(type_name, n)
        assert verify(n, edges, cert), \
            "{} witness {} fails on n={} edges={}".format(type_name, cert, n, edges)
    assert saw_no, "{} accepted every sample graph".format(type_name)


@pytest.mark.parametrize("type_name", CERTIFIED_TYPES)
def test_yes_answers_carry_no_certificate(type_name):
    rec_fn = getattr(gr, "recognize_{}".format(type_name))
    for n, edges in SAMPLE_GRAPHS:
        member, cert = rec_fn(n, edges)
        assert (cert is None) == member


def test_certificate_survives_a_networkx_input():
    nx = pytest.importorskip("networkx")
    g = nx.cycle_graph(5)
    g = nx.relabel_nodes(g, {i: i + 1 for i in g.nodes})
    member, cert = gr.recognize_chordal(g)
    assert member is False
    assert cert["kind"] == "hole"
    assert verify(5, list(g.edges), cert)


def test_uncertified_class_reports_nothing():
    # circle has no NO certificate: the recognizer decides by linear algebra
    # over GF(2), which leaves no combinatorial witness behind.
    assert "circle" not in CERTIFIED_TYPES
    member, cert = gr.recognize_circle(6, [(1, 2), (2, 3), (3, 1)])
    assert cert is None
    assert isinstance(member, bool)


REPO_TESTS_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), os.pardir, os.pardir, "tests"
)


@pytest.mark.skipif(
    not os.path.isdir(REPO_TESTS_DIR), reason="repository fixtures unavailable"
)
@pytest.mark.parametrize("type_name", CERTIFIED_TYPES)
def test_certificates_on_repository_fixtures(type_name):
    """Every NO fixture of the C++ suite must yield a verifiable witness."""
    case_dir = os.path.join(REPO_TESTS_DIR, type_name)
    if not os.path.isdir(case_dir):
        pytest.skip("no fixtures for {}".format(type_name))

    rec_fn = getattr(gr, "recognize_{}".format(type_name))
    checked = 0
    for name in sorted(os.listdir(case_dir)):
        if not name.endswith(".in"):
            continue
        with open(os.path.join(case_dir, name)) as fh:
            n, m = (int(x) for x in fh.readline().split())
            edges = [tuple(int(x) for x in fh.readline().split()) for _ in range(m)]
        member, cert = rec_fn(n, edges)
        if member:
            continue
        if type_name == "biconnected" and n < 3:
            # A graph too small to be biconnected fails on size alone; K2 is
            # connected and has no cut vertex, so there is nothing to point at.
            assert cert is None
            continue
        checked += 1
        assert cert is not None, "{}/{} gave no witness".format(type_name, name)
        assert verify(n, edges, cert), \
            "{}/{} witness {} fails verification".format(type_name, name, cert)
    if checked == 0:
        pytest.skip("no NO fixtures for {}".format(type_name))
