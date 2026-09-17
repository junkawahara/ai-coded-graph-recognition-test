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

# (minimum length, required parity or None, maximum number of chords or
# None for "any number"). Only ODD_CYCLE tolerates chords -- an odd closed
# walk with distinct vertices is non-bipartite whatever else is present -- and
# ODD_CYCLE_LE1_CHORD (handled apart) tolerates exactly one.
CYCLE_RULES = {
    "hole": (4, None, 0),
    "odd_cycle": (3, 1, None),
    "odd_hole": (5, 1, 0),
    "even_hole": (4, 0, 0),
    "c4": (4, 0, 0),
    "c5": (5, 1, 0),
}

# What each certified class may return, restated from the Doxygen comment on
# its `Obstruction obstruction;` field: kind -> (side, minimum length). side is
# False for a witness in the graph, True for one in the complement, None when
# the class documents both. The minimum length overrides the kind's own bound
# where the class contract is stricter (a chordal bipartite graph may contain
# C4, so its EVEN_HOLE has length >= 6; a weakly chordal one may contain C4
# too, so its HOLE has length >= 5). verify_for() applies this table before the
# kind-level check, so a regression that hands back a shorter cycle, the
# wrong side or a kind the class never promised is caught even though it
# would be a valid instance of its kind.
CLASS_RULES = {
    "at_free": {"asteroidal_triple": (False, 0)},
    "biconnected": {"cut_vertex": (False, 0), "disconnected_pair": (False, 0)},
    "bipartite": {"odd_cycle": (False, 0)},
    "block": {"hole": (False, 0), "diamond": (False, 0)},
    "bull_free": {"bull": (False, 0)},
    "cactus": {"two_cycles_sharing_edge": (False, 0)},
    "chain": {"odd_cycle": (False, 0), "2k2": (False, 0)},
    "chordal": {"hole": (False, 0)},
    "chordal_bipartite": {"odd_cycle": (False, 0), "even_hole": (False, 6)},
    "claw_free": {"claw": (False, 0)},
    "cluster": {"p3": (False, 0)},
    "co_chordal": {"hole": (True, 0)},
    "co_comparability": {"forcing_cycle": (True, 0)},
    "co_interval": {"hole": (True, 0), "asteroidal_triple": (True, 0)},
    "cochain": {"odd_cycle": (True, 0), "2k2": (True, 0)},
    "cograph": {"p4": (False, 0)},
    "comparability": {"forcing_cycle": (False, 0)},
    "diamond_free": {"diamond": (False, 0)},
    "distance_hereditary": {"non_shortest_induced_path": (False, 0)},
    "even_hole_free": {"even_hole": (False, 0)},
    "gem_free": {"gem": (False, 0)},
    "interval": {"hole": (False, 0), "asteroidal_triple": (False, 0)},
    "meyniel": {"odd_cycle_le1_chord": (False, 0)},
    "odd_hole_free": {"odd_hole": (False, 0)},
    "p5_free": {"p5": (False, 0)},
    "parity": {"induced_path_wrong_parity": (False, 0)},
    "perfect": {"odd_hole": (None, 0)},
    "permutation": {"forcing_cycle": (None, 0)},
    "planar": {"k5_minor": (False, 0), "k33_minor": (False, 0)},
    "proper_interval": {
        "claw": (False, 0), "hole": (False, 0), "asteroidal_triple": (False, 0),
    },
    "ptolemaic": {"hole": (False, 0), "gem": (False, 0)},
    "quasi_threshold": {"c4": (False, 0), "p4": (False, 0)},
    # split's witnesses are always read in the graph itself (see
    # notes/obstruction_notes.md): the complement's hole is projected to the
    # C4 / 2K2 of the graph it is.
    "split": {"2k2": (False, 0), "c4": (False, 0), "c5": (False, 0)},
    "threshold": {"2k2": (False, 0), "c4": (False, 0), "p4": (False, 0)},
    "triangle_free": {"triangle": (False, 0)},
    "trivially_perfect": {"c4": (False, 0), "p4": (False, 0)},
    "unit_interval": {
        "claw": (False, 0), "hole": (False, 0), "asteroidal_triple": (False, 0),
    },
    "weakly_chordal": {"hole": (None, 5)},
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


def _check_cycle(adj, cycle, min_len, parity, max_chords):
    """A simple closed cycle in cyclic order; max_chords=None allows any."""
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
    return max_chords is None or chords <= max_chords


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
        min_len, parity, max_chords = CYCLE_RULES[kind]
        if kind in ("c4", "c5") and len(verts) != min_len:
            return False
        return _check_cycle(adj, verts, min_len, parity, max_chords)

    if kind == "odd_cycle_le1_chord":
        return _check_cycle(adj, verts, 5, 1, 1)

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
            if not _check_cycle(adj, cycle, 3, None, None):
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


def verify_for(type_name, n, edges, cert):
    """verify() restricted to what type_name's recognizer promises.

    The certificate must be a kind listed for the class in CLASS_RULES, on
    the documented side of the graph and at least as long as the class
    contract demands; only then is its own definition checked. An unknown
    class fails outright so the table cannot silently lag behind
    CERTIFIED_TYPES.
    """
    rules = CLASS_RULES.get(type_name)
    if rules is None or cert["kind"] not in rules:
        return False
    side, min_len = rules[cert["kind"]]
    if side is not None and cert["in_complement"] != side:
        return False
    if len(cert["vertices"]) < min_len:
        return False
    return verify(n, edges, cert)


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
        assert verify_for(type_name, n, edges, cert), \
            "{} witness {} fails on n={} edges={}".format(type_name, cert, n, edges)
    assert saw_no, "{} accepted every sample graph".format(type_name)


def test_class_rules_cover_exactly_the_certified_types():
    assert sorted(CLASS_RULES) == CERTIFIED_TYPES
    known_kinds = set(PATTERN_EDGES) | set(CYCLE_RULES) | set(MINOR_PAIRS) | {
        "odd_cycle_le1_chord", "asteroidal_triple", "cut_vertex",
        "disconnected_pair", "forcing_cycle", "two_cycles_sharing_edge",
        "induced_path_wrong_parity", "non_shortest_induced_path",
    }
    for type_name, rules in CLASS_RULES.items():
        assert set(rules) <= known_kinds, type_name


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
        assert verify_for(type_name, n, edges, cert), \
            "{}/{} witness {} fails verification".format(type_name, name, cert)
    if checked == 0:
        pytest.skip("no NO fixtures for {}".format(type_name))


# ---- self-tests of the verifier on forged certificates ----------------------
#
# The tests above only ever see genuine recognizer output, so a verifier that
# accepts too much (or too little) never fails there. These feed hand-made
# certificates that are wrong in exactly one way, next to correct ones.


def _cert(kind, vertices, in_complement=False, vertex_sets=()):
    return {
        "kind": kind,
        "vertices": list(vertices),
        "in_complement": in_complement,
        "vertex_sets": [list(s) for s in vertex_sets],
    }


K5 = (5, list(itertools.combinations(range(1, 6), 2)))
K4 = (4, list(itertools.combinations(range(1, 5), 2)))
C4 = (4, [(1, 2), (2, 3), (3, 4), (4, 1)])
C5 = (5, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 1)])
C6 = (6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 1)])
C5_ONE_CHORD = (5, C5[1] + [(1, 3)])
C5_TWO_CHORDS = (5, C5[1] + [(1, 3), (1, 4)])


@pytest.mark.parametrize("graph, cert, expected", [
    # ODD_CYCLE tolerates any number of chords: K5 walked 1..5 is one.
    (K5, _cert("odd_cycle", [1, 2, 3, 4, 5]), True),
    (C5_TWO_CHORDS, _cert("odd_cycle", [1, 2, 3, 4, 5]), True),
    (C5, _cert("odd_cycle", [1, 2, 3, 4, 5]), True),
    # ... but it must still be an odd simple closed walk.
    (K4, _cert("odd_cycle", [1, 2, 3, 4]), False),
    (K5, _cert("odd_cycle", [1, 2, 3, 4, 1]), False),
    (K5, _cert("odd_cycle", [1, 2, 3, 4, 6]), False),
    (C6, _cert("odd_cycle", [1, 2, 3, 4, 5]), False),
    # Holes of every flavour reject chords.
    (K5, _cert("odd_hole", [1, 2, 3, 4, 5]), False),
    (K5, _cert("hole", [1, 2, 3, 4, 5]), False),
    (C5_ONE_CHORD, _cert("hole", [1, 2, 3, 4, 5]), False),
    (C5, _cert("hole", [1, 2, 3, 4, 5]), True),
    (C5, _cert("odd_hole", [1, 2, 3, 4, 5]), True),
    (C6, _cert("even_hole", [1, 2, 3, 4, 5, 6]), True),
    (C6, _cert("odd_hole", [1, 2, 3, 4, 5, 6]), False),
    # ODD_CYCLE_LE1_CHORD: exactly the "at most one" of its name.
    (C5, _cert("odd_cycle_le1_chord", [1, 2, 3, 4, 5]), True),
    (C5_ONE_CHORD, _cert("odd_cycle_le1_chord", [1, 2, 3, 4, 5]), True),
    (C5_TWO_CHORDS, _cert("odd_cycle_le1_chord", [1, 2, 3, 4, 5]), False),
    # Complement side: C5 is self-complementary, K5's complement is edgeless.
    (C5, _cert("hole", [1, 3, 5, 2, 4], in_complement=True), True),
    (K5, _cert("odd_cycle", [1, 2, 3, 4, 5], in_complement=True), False),
])
def test_verifier_on_forged_cycle_certificates(graph, cert, expected):
    n, edges = graph
    assert verify(n, edges, cert) is expected


def test_class_rule_rejects_c4_as_a_chordal_bipartite_witness():
    n, edges = C4
    cert = _cert("even_hole", [1, 2, 3, 4])
    assert verify(n, edges, cert)                       # a valid even hole
    assert not verify_for("chordal_bipartite", n, edges, cert)
    assert verify_for("even_hole_free", n, edges, cert)
    n6, edges6 = C6
    assert verify_for("chordal_bipartite", n6, edges6,
                      _cert("even_hole", [1, 2, 3, 4, 5, 6]))


def test_class_rule_rejects_undocumented_kind_and_side():
    n, edges = C5
    hole = _cert("hole", [1, 2, 3, 4, 5])
    odd = _cert("odd_cycle", [1, 2, 3, 4, 5])
    # The complement of C5 is the 5-cycle 1-3-5-2-4.
    co_hole = _cert("hole", [1, 3, 5, 2, 4], in_complement=True)
    assert verify_for("chordal", n, edges, hole)
    assert not verify_for("chordal", n, edges, odd)
    assert not verify_for("chordal", n, edges, co_hole)
    assert verify_for("bipartite", n, edges, odd)
    assert not verify_for("bipartite", n, edges, hole)
    assert verify_for("co_chordal", n, edges, co_hole)
    assert not verify_for("co_chordal", n, edges, hole)
    assert verify_for("weakly_chordal", n, edges, hole)
    assert verify_for("weakly_chordal", n, edges, co_hole)
    assert not verify_for("no_such_class", n, edges, hole)


def test_class_rule_keeps_the_kind_level_check():
    # A permitted kind on the permitted side is still checked as a pattern.
    n, edges = C5
    assert not verify_for("chordal", n, edges, _cert("hole", [1, 2, 3, 5, 4]))
    assert verify_for("cograph", n, edges, _cert("p4", [1, 2, 3, 4]))  # induced in C5
    assert not verify_for("cograph", n, edges, _cert("p4", [1, 2, 3, 5]))
    assert verify_for("cograph", 4, [(1, 2), (2, 3), (3, 4)],
                      _cert("p4", [1, 2, 3, 4]))
