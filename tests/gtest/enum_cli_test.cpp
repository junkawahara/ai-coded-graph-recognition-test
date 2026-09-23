// CLI-level regression tests for the enumeration binaries.
//
// The guards these check live in src/*_main.cpp, so no library test can see
// them: an unsupported n printed as a count, a mistyped option enumerating
// something else, or an enumeration materialized before the first byte of
// output. The binaries are built by "make", not by "make gtest_all", so each
// test skips when its binary is absent.

#include "test_helpers.h"
#include <gtest/gtest.h>

#include <cstdio>
#include <set>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

using graph_recognition::gtest_utils::test_path;

namespace {

std::string bin_path(const std::string& name) {
    return test_path("../bin/" + name);
}

struct CliResult {
    int status;
    std::string out;
};

/** @brief Runs bin/<name> <args> with @p input on stdin, capturing stdout */
CliResult run_cli(const std::string& name,
                  const std::string& args,
                  const std::string& input) {
    const std::string command = "printf '%s' " + std::string("'") + input +
                                "' | " + bin_path(name) + " " + args +
                                " 2>/dev/null";
    CliResult res;
    res.status = -1;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == 0) return res;
    char buffer[4096];
    while (std::fgets(buffer, sizeof(buffer), pipe) != 0) res.out += buffer;
    const int closed = pclose(pipe);
    res.status = WIFEXITED(closed) ? WEXITSTATUS(closed) : -1;
    return res;
}

bool skip_if_missing(const std::string& name) {
    return access(bin_path(name).c_str(), X_OK) != 0;
}

std::size_t line_count(const std::string& s) {
    std::size_t lines = 0;
    for (std::size_t i = 0; i < s.size(); ++i)
        if (s[i] == '\n') ++lines;
    return lines;
}

// n >= 64 is outside the enumerator's bitmask range, where it returns an
// empty result. Printing that as a count claims the class has no member on
// 64 vertices, which is false for every n.
TEST(EnumCliTest, UnlabeledEnumeratorsRefuseAnUnsupportedVertexCount) {
    const char* names[3] = {"chordal_unlabeled_enum", "split_unlabeled_enum",
                            "threshold_unlabeled_enum"};
    for (int i = 0; i < 3; ++i) {
        if (skip_if_missing(names[i])) GTEST_SKIP() << names[i] << " not built";
        CliResult r = run_cli(names[i], "", "64\n");
        EXPECT_NE(r.status, 0) << names[i];
        EXPECT_EQ(r.out, "") << names[i];
    }
}

TEST(EnumCliTest, UnlabeledEnumeratorsRefuseMalformedInput) {
    const char* names[3] = {"chordal_unlabeled_enum", "split_unlabeled_enum",
                            "threshold_unlabeled_enum"};
    for (int i = 0; i < 3; ++i) {
        if (skip_if_missing(names[i])) GTEST_SKIP() << names[i] << " not built";
        EXPECT_NE(run_cli(names[i], "", "garbage\n").status, 0) << names[i];
        EXPECT_NE(run_cli(names[i], "", "-1\n").status, 0) << names[i];
    }
}

// A mistyped --connected used to be ignored, so the CLI answered a different
// question (557 graphs instead of 393) with exit status 0.
TEST(EnumCliTest, UnknownOptionIsRejected) {
    if (skip_if_missing("split_unlabeled_enum")) GTEST_SKIP() << "not built";
    EXPECT_NE(run_cli("split_unlabeled_enum", "--conected", "8\n").status, 0);
    CliResult ok = run_cli("split_unlabeled_enum", "--connected", "8\n");
    EXPECT_EQ(ok.status, 0);
    EXPECT_EQ(ok.out.substr(0, ok.out.find('\n')), "393");
}

TEST(EnumCliTest, LabeledEnumeratorsRefuseAnUnsupportedVertexCount) {
    if (!skip_if_missing("split_labeled_enum")) {
        EXPECT_NE(run_cli("split_labeled_enum", "", "9\n").status, 0);
    }
    if (!skip_if_missing("strongly_chordal_labeled_enum")) {
        EXPECT_NE(run_cli("strongly_chordal_labeled_enum", "", "8\n").status, 0);
    }
}

// The announced count is the enumerated count: the CLIs take it from a table
// (or the 2^(n-1) formula) so that they can stream, and check it afterwards.
TEST(EnumCliTest, AnnouncedCountMatchesTheStreamedOutput) {
    struct Case {
        const char* name;
        const char* input;
        const char* expected;
    };
    const Case cases[3] = {{"split_labeled_enum", "4\n", "58"},
                           {"strongly_chordal_labeled_enum", "4\n", "61"},
                           {"threshold_unlabeled_enum", "4\n", "8"}};
    for (int i = 0; i < 3; ++i) {
        if (skip_if_missing(cases[i].name)) GTEST_SKIP() << cases[i].name
                                                         << " not built";
        CliResult r = run_cli(cases[i].name, "", cases[i].input);
        EXPECT_EQ(r.status, 0) << cases[i].name;
        const std::string first = r.out.substr(0, r.out.find('\n'));
        EXPECT_EQ(first, cases[i].expected) << cases[i].name;
        EXPECT_EQ(line_count(r.out), std::stoul(cases[i].expected) + 1)
            << cases[i].name;
    }
}

// A host of isolated vertices has exactly one chordal subgraph, and the
// enumeration must not size itself by the vertex count to find that out.
TEST(EnumCliTest, ChordalSubgraphEnumHandlesAVerySparseHost) {
    if (skip_if_missing("chordal_subgraph_enum")) GTEST_SKIP() << "not built";
    CliResult r = run_cli("chordal_subgraph_enum", "", "10000 0\n");
    EXPECT_EQ(r.status, 0);
    EXPECT_EQ(r.out, "1\n0\n");
}

TEST(EnumCliTest, CoChordalSupportsBothAlgorithms) {
    const std::string name = "co_chordal_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    const auto default_result = run_cli(name, "", "4\n");
    const auto dedicated = run_cli(name, "--algo kiyomi_uno", "4\n");
    const auto legacy = run_cli(name, "--algo reverse_search", "4\n");
    ASSERT_EQ(default_result.status, 0);
    ASSERT_EQ(dedicated.status, 0);
    ASSERT_EQ(legacy.status, 0);
    EXPECT_EQ(default_result.out, dedicated.out);
    EXPECT_NE(dedicated.out, legacy.out);
    std::set<std::string> dedicated_set, legacy_set;
    for (const auto* result : {&dedicated, &legacy}) {
        std::istringstream lines(result->out);
        std::string line;
        ASSERT_TRUE(static_cast<bool>(std::getline(lines, line)));
        EXPECT_EQ(line, "61");
        auto& graphs = result == &dedicated ? dedicated_set : legacy_set;
        while (std::getline(lines, line)) EXPECT_TRUE(graphs.insert(line).second);
        EXPECT_EQ(graphs.size(), 61u);
    }
    EXPECT_EQ(dedicated_set, legacy_set);
    EXPECT_EQ(run_cli(name, "", "2\n").out, "2\n1 1 2\n0\n");
    EXPECT_EQ(run_cli(name, "--algo reverse_search", "2\n").out, "2\n0\n1 1 2\n");
    for (const auto& args : {"--algo kiyomi_uno", "--algo reverse_search"}) {
        const auto empty = run_cli(name, args, "0\n");
        EXPECT_EQ(empty.status, 0);
        EXPECT_EQ(empty.out, "1\n0\n");
        const auto oversized = run_cli(name, args, "65\n");
        EXPECT_EQ(oversized.status, 1);
        EXPECT_TRUE(oversized.out.empty());
    }
}

TEST(EnumCliTest, CoChordalRejectsInvalidAlgorithmOptions) {
    const std::string name = "co_chordal_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    for (const auto& args : {"--algo", "--algo unknown", "--alg kiyomi_uno",
                             "--algo kiyomi_uno extra"}) {
        const auto result = run_cli(name, args, "3\n");
        EXPECT_EQ(result.status, 1) << args;
        EXPECT_TRUE(result.out.empty()) << args;
    }
}

}  // namespace

// ---- the shared input contract of every enumeration CLI --------------------
//
// review_all_1.md #22: about 110 enumeration CLIs used to print "0" for a
// negative n, nothing at all for an empty stdin, "0" for an n beyond the
// enumerator's bitmask range, and silently ignored unknown options, all with
// exit status 0. They now share cli_parse_enum_options() /
// cli_read_vertex_count() (util/cli_main.h), and the enumerators themselves
// throw std::invalid_argument for an unrepresentable n (util/vertex_count.h),
// which run_cli_main() maps to status 1.

namespace {

// Every n-taking enumeration CLI. The subgraph enumerators read a graph
// through Graph::read and are covered by its own tests.
const char* const kEnumClis[] = {
    "apex_labeled_enum", "apollonian_unlabeled_enum", "at_free_labeled_enum",
    "biconnected_labeled_enum", "biconnected_unlabeled_enum",
    "biconvex_bipartite_labeled_enum", "bipartite_labeled_enum",
    "bipartite_permutation_labeled_enum", "bipartite_permutation_unlabeled_enum",
    "bipartite_unlabeled_enum", "block_labeled_enum", "bull_free_labeled_enum",
    "cactus_labeled_enum", "cactus_unlabeled_enum", "cage_unlabeled_enum",
    "caterpillar_unlabeled_enum", "chain_unlabeled_enum",
    "chordal_bipartite_labeled_enum", "chordal_labeled_enum",
    "chordal_unlabeled_enum", "circle_labeled_enum", "circle_unlabeled_enum",
    "circular_arc_labeled_enum", "claw_free_labeled_enum", "cluster_labeled_enum",
    "cluster_unlabeled_enum", "co_chordal_labeled_enum", "co_chordal_unlabeled_enum",
    "co_comparability_labeled_enum", "co_comparability_unlabeled_enum",
    "co_interval_labeled_enum", "co_interval_unlabeled_enum",
    "cochain_unlabeled_enum", "cograph_labeled_enum", "cograph_unlabeled_enum",
    "comparability_labeled_enum", "comparability_unlabeled_enum",
    "convex_bipartite_labeled_enum", "cubic_labeled_enum",
    "cubic_planar_labeled_enum", "cubic_planar_unlabeled_enum",
    "cubic_unlabeled_enum", "degenerate_unlabeled_enum",
    "diamond_free_labeled_enum", "digraph_labeled_enum", "digraph_unlabeled_enum",
    "distance_hereditary_labeled_enum", "distance_hereditary_unlabeled_enum",
    "eulerian_labeled_enum", "eulerian_unlabeled_enum",
    "even_hole_free_labeled_enum", "five_leaf_power_labeled_enum",
    "forest_unlabeled_enum", "four_leaf_power_labeled_enum",
    "fullerene_unlabeled_enum", "gem_free_labeled_enum", "halin_unlabeled_enum",
    "interval_labeled_enum", "interval_unlabeled_enum", "kregular_labeled_enum",
    "kregular_unlabeled_enum", "ktree_labeled_enum", "laman_labeled_enum",
    "laman_unlabeled_enum", "line_graph_labeled_enum",
    "maximal_outer_planar_unlabeled_enum", "maximal_planar_labeled_enum",
    "maximal_planar_unlabeled_enum", "meyniel_labeled_enum",
    "odd_hole_free_labeled_enum", "outer_planar_labeled_enum",
    "outer_planar_unlabeled_enum", "p5_free_labeled_enum", "parity_labeled_enum",
    "partial_ktree_unlabeled_enum", "perfect_labeled_enum",
    "permutation_labeled_enum", "permutation_unlabeled_enum",
    "planar_labeled_enum", "planar_unlabeled_enum", "polyhedral_labeled_enum",
    "polyhedral_unlabeled_enum", "poset_labeled_enum", "poset_unlabeled_enum",
    "proper_chordal_labeled_enum", "proper_circular_arc_labeled_enum",
    "proper_interval_labeled_enum", "proper_interval_unlabeled_enum",
    "ptolemaic_labeled_enum", "ptolemaic_unlabeled_enum",
    "self_complementary_labeled_enum", "self_complementary_unlabeled_enum",
    "series_parallel_labeled_enum", "series_parallel_unlabeled_enum",
    "simple_quadrangulation_unlabeled_enum", "snark_labeled_enum",
    "snark_unlabeled_enum", "split_labeled_enum", "split_unlabeled_enum",
    "strongly_chordal_labeled_enum", "strongly_regular_labeled_enum",
    "strongly_regular_unlabeled_enum", "three_leaf_power_labeled_enum",
    "three_leaf_power_unlabeled_enum", "threshold_unlabeled_enum",
    "tournament_labeled_enum", "tournament_unlabeled_enum",
    "trapezoid_labeled_enum", "tree_unlabeled_enum", "triangle_free_labeled_enum",
    "triangle_free_unlabeled_enum", "trivially_perfect_labeled_enum",
    "trivially_perfect_unlabeled_enum", "unicyclic_unlabeled_enum",
    "weakly_chordal_labeled_enum"};
const int kEnumCliCount = sizeof(kEnumClis) / sizeof(kEnumClis[0]);

// CLIs whose enumerator keeps a vertex's neighbourhood in one 64-bit word
// (n <= 63) ...
const char* const kMax63[] = {
    "apollonian_unlabeled_enum", "biconnected_unlabeled_enum",
    "bipartite_permutation_unlabeled_enum", "bipartite_unlabeled_enum",
    "cactus_unlabeled_enum", "cage_unlabeled_enum", "chordal_unlabeled_enum",
    "circle_unlabeled_enum", "comparability_unlabeled_enum",
    "cubic_planar_unlabeled_enum", "cubic_unlabeled_enum",
    "degenerate_unlabeled_enum", "distance_hereditary_unlabeled_enum",
    "eulerian_unlabeled_enum", "interval_unlabeled_enum",
    "kregular_unlabeled_enum", "laman_unlabeled_enum",
    "maximal_planar_unlabeled_enum", "outer_planar_unlabeled_enum",
    "partial_ktree_unlabeled_enum", "permutation_unlabeled_enum",
    "planar_unlabeled_enum", "polyhedral_unlabeled_enum",
    "ptolemaic_unlabeled_enum", "self_complementary_unlabeled_enum",
    "series_parallel_unlabeled_enum", "snark_unlabeled_enum",
    "split_unlabeled_enum", "strongly_regular_unlabeled_enum",
    "three_leaf_power_unlabeled_enum", "threshold_unlabeled_enum",
    "tournament_unlabeled_enum", "triangle_free_unlabeled_enum",
    "trivially_perfect_labeled_enum"};
// ... or the candidate neighbourhoods of a new vertex (n <= 64) ...
const char* const kMax64[] = {
    "apex_labeled_enum", "at_free_labeled_enum", "biconnected_labeled_enum",
    "biconvex_bipartite_labeled_enum", "bipartite_labeled_enum",
    "bipartite_permutation_labeled_enum", "bull_free_labeled_enum",
    "cactus_labeled_enum", "chordal_bipartite_labeled_enum",
    "circle_labeled_enum", "circular_arc_labeled_enum", "claw_free_labeled_enum",
    "co_chordal_labeled_enum", "co_comparability_labeled_enum",
    "co_interval_labeled_enum", "comparability_labeled_enum",
    "convex_bipartite_labeled_enum", "diamond_free_labeled_enum",
    "distance_hereditary_labeled_enum", "even_hole_free_labeled_enum",
    "gem_free_labeled_enum", "laman_labeled_enum", "line_graph_labeled_enum",
    "maximal_planar_labeled_enum", "meyniel_labeled_enum",
    "odd_hole_free_labeled_enum", "outer_planar_labeled_enum",
    "p5_free_labeled_enum", "parity_labeled_enum", "perfect_labeled_enum",
    "permutation_labeled_enum", "planar_labeled_enum",
    "polyhedral_labeled_enum", "proper_circular_arc_labeled_enum",
    "series_parallel_labeled_enum", "trapezoid_labeled_enum",
    "triangle_free_labeled_enum", "weakly_chordal_labeled_enum"};
// ... or both arc directions of the directed canonical form (n <= 33).
const char* const kMax33[] = {"digraph_unlabeled_enum", "poset_unlabeled_enum"};

}  // namespace

// Empty stdin, a negative n and an unknown option are refused by every
// enumeration CLI with status 1 and nothing on stdout (they used to print
// nothing, "0", or the answer to a different question, all with status 0).
TEST(EnumCliTest, EveryEnumerationCliRefusesMalformedInput) {
    int checked = 0;
    for (int i = 0; i < kEnumCliCount; ++i) {
        const std::string name = kEnumClis[i];
        if (skip_if_missing(name)) continue;
        ++checked;
        CliResult empty = run_cli(name, "", "");
        EXPECT_EQ(empty.status, 1) << name << " on empty stdin";
        EXPECT_EQ(empty.out, "") << name << " on empty stdin";
        CliResult negative = run_cli(name, "", "-1\n");
        EXPECT_EQ(negative.status, 1) << name << " on n = -1";
        EXPECT_EQ(negative.out, "") << name << " on n = -1";
        CliResult option = run_cli(name, "--no-such-option", "3\n");
        EXPECT_EQ(option.status, 1) << name << " with an unknown option";
        EXPECT_EQ(option.out, "") << name << " with an unknown option";
    }
    if (checked == 0) GTEST_SKIP() << "no enumeration CLI built";
}

// An n one past the enumerator's representable range is refused, never
// reported as "0 graphs" (every class has a member on every n).
TEST(EnumCliTest, EveryBitmaskEnumerationCliRefusesAnUnrepresentableVertexCount) {
    struct Group {
        const char* const* names;
        int count;
        const char* input;
    };
    const Group groups[3] = {
        {kMax63, (int)(sizeof(kMax63) / sizeof(kMax63[0])), "64\n"},
        {kMax64, (int)(sizeof(kMax64) / sizeof(kMax64[0])), "65\n"},
        {kMax33, (int)(sizeof(kMax33) / sizeof(kMax33[0])), "34\n"}};
    int checked = 0;
    for (int gi = 0; gi < 3; ++gi) {
        for (int i = 0; i < groups[gi].count; ++i) {
            const std::string name = groups[gi].names[i];
            if (skip_if_missing(name)) continue;
            ++checked;
            CliResult r = run_cli(name, "", groups[gi].input);
            EXPECT_EQ(r.status, 1) << name << " on " << groups[gi].input;
            EXPECT_EQ(r.out, "") << name << " on " << groups[gi].input;
        }
    }
    if (checked == 0) GTEST_SKIP() << "no bitmask enumeration CLI built";
}

// The k-taking CLIs refuse a missing parameter the same way.
TEST(EnumCliTest, ParameterizedEnumerationClisRefuseAMissingParameter) {
    const char* names[5] = {"kregular_labeled_enum", "ktree_labeled_enum",
                            "degenerate_unlabeled_enum",
                            "partial_ktree_unlabeled_enum", "cage_unlabeled_enum"};
    int checked = 0;
    for (int i = 0; i < 5; ++i) {
        if (skip_if_missing(names[i])) continue;
        ++checked;
        CliResult r = run_cli(names[i], "", "4\n");
        EXPECT_EQ(r.status, 1) << names[i] << " without its parameter";
        EXPECT_EQ(r.out, "") << names[i] << " without its parameter";
    }
    if (checked == 0) GTEST_SKIP() << "no parameterized enumeration CLI built";
}
