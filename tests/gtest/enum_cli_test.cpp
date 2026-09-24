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

TEST(EnumCliTest, BiconnectedAlgorithmSelectionEdgeBoundsAndValidation) {
    for (const std::string kind : {"labeled", "unlabeled"}) {
        const std::string name = "biconnected_" + kind + "_enum";
        const std::string legacy = kind == "labeled" ? "reverse_search" : "canonical_augmentation";
        if (skip_if_missing(name)) GTEST_SKIP() << name << " not built";
        const auto defaults = run_cli(name, "", "5\n");
        const auto dedicated = run_cli(name, "--algo ear_augmentation", "5\n");
        ASSERT_EQ(defaults.status, 0);
        ASSERT_EQ(dedicated.status, 0);
        EXPECT_EQ(defaults.out, dedicated.out);
        const std::string count = kind == "labeled" ? "238" : "10";
        for (const auto& algo : {std::string("ear_augmentation"), legacy}) {
            const auto full = run_cli(name, "--algo " + algo, "5\n");
            EXPECT_EQ(full.status, 0);
            EXPECT_EQ(full.out.substr(0, full.out.find('\n')), count);
            EXPECT_EQ(line_count(full.out), std::stoul(count) + 1);
            const auto cycles = run_cli(name, "--max-edges 5 --algo " + algo, "5\n");
            EXPECT_EQ(cycles.status, 0);
            EXPECT_EQ(cycles.out.substr(0, cycles.out.find('\n')), kind == "labeled" ? "12" : "1");
            EXPECT_EQ(run_cli(name, "--algo " + algo + " --max-edges 4", "5\n").out, "0\n");
            for (const auto& input : {"0\n", "1\n", "2\n"}) {
                const auto empty = run_cli(name, "--algo " + algo, input);
                EXPECT_EQ(empty.status, 0);
                EXPECT_EQ(empty.out, "0\n");
            }
            const std::string upper = kind == "labeled" && algo == legacy ? "65\n" : "64\n";
            for (const auto& input : {std::string("-1\n"), std::string("garbage\n"), upper}) {
                const auto invalid = run_cli(name, "--algo " + algo, input);
                EXPECT_EQ(invalid.status, 1);
                EXPECT_TRUE(invalid.out.empty());
            }
        }
        for (const std::string args : {"--algo", "--algo unknown", "--algo ''", "--connected",
                 "--max-edges", "--max-edges -1", "--max-edges 2.5", "--max-edges nope",
                 "--max-edges 99999999999999999999", "--algo ear_augmentation extra"}) {
            const auto invalid = run_cli(name, args, "3\n");
            EXPECT_EQ(invalid.status, 1) << name << " " << args;
            EXPECT_TRUE(invalid.out.empty());
        }
    }
    EXPECT_EQ(run_cli("biconnected_unlabeled_enum", "--algo canonical_augmentation", "4\n").out,
        "3\n4 1 3 1 4 2 3 2 4\n5 1 3 1 4 2 3 2 4 3 4\n6 1 2 1 3 1 4 2 3 2 4 3 4\n");
    EXPECT_EQ(run_cli("biconnected_labeled_enum", "--algo reverse_search --max-edges 0", "64\n").status, 0);
}

TEST(EnumCliTest, TriangleFreeSupportsBothAlgorithmsAndConnectedFiltering) {
    struct Case {
        std::string name;
        std::string legacy;
        bool unlabeled;
        bool connected;
        std::size_t count;
    };
    const Case cases[] = {
        {"triangle_free_labeled_enum", "reverse_search", false, false, 388},
        {"triangle_free_unlabeled_enum", "canonical_augmentation", true, false, 14},
        {"triangle_free_unlabeled_enum", "canonical_augmentation", true, true, 6}};
    for (const auto& c : cases) {
        if (skip_if_missing(c.name)) GTEST_SKIP() << c.name << " not built";
        SCOPED_TRACE(c.name + (c.connected ? " connected" : ""));
        const std::string connected = c.connected ? "--connected" : "";
        const auto defaults = run_cli(c.name, connected, "5\n");
        const auto dedicated = run_cli(c.name, connected + " --algo independent_sets", "5\n");
        const auto legacy = run_cli(c.name, "--algo " + c.legacy + " " + connected, "5\n");
        ASSERT_EQ(defaults.status, 0);
        ASSERT_EQ(dedicated.status, 0);
        ASSERT_EQ(legacy.status, 0);
        EXPECT_EQ(defaults.out, dedicated.out);
        EXPECT_EQ(dedicated.out, legacy.out);
        EXPECT_EQ(dedicated.out.substr(0, dedicated.out.find('\n')), std::to_string(c.count));
        EXPECT_EQ(line_count(dedicated.out), c.count + 1);
        for (const auto& algo : {std::string("independent_sets"), c.legacy}) {
            const std::string args = "--algo " + algo + " " + connected;
            const auto empty = run_cli(c.name, args, "0\n");
            EXPECT_EQ(empty.status, 0);
            EXPECT_EQ(empty.out, "1\n0\n");
            for (const auto& input : {"-1\n", "garbage\n", c.unlabeled ? "64\n" : "65\n"}) {
                const auto invalid = run_cli(c.name, args, input);
                EXPECT_EQ(invalid.status, 1);
                EXPECT_TRUE(invalid.out.empty());
            }
        }
    }
}

TEST(EnumCliTest, TriangleFreeRejectsInvalidAlgorithmOptions) {
    for (const std::string name : {"triangle_free_labeled_enum", "triangle_free_unlabeled_enum"}) {
        if (skip_if_missing(name)) GTEST_SKIP() << name << " not built";
        for (const auto& args : {"--algo", "--algo unknown", "--alg independent_sets",
                                 "--algo independent_sets extra", "--conected"}) {
            const auto result = run_cli(name, args, "3\n");
            EXPECT_EQ(result.status, 1) << name << " " << args;
            EXPECT_TRUE(result.out.empty()) << name << " " << args;
        }
        const auto wrong = run_cli(name, name == "triangle_free_labeled_enum"
            ? "--algo canonical_augmentation" : "--algo reverse_search", "3\n");
        EXPECT_EQ(wrong.status, 1);
        EXPECT_TRUE(wrong.out.empty());
    }
}

TEST(EnumCliTest, BipartiteSupportsBothAlgorithmsAndConnectedFiltering) {
    struct Case {
        std::string name;
        std::string legacy;
        bool unlabeled;
        bool connected;
        std::size_t count;
    };
    const Case cases[] = {
        {"bipartite_labeled_enum", "reverse_search", false, false, 376},
        {"bipartite_unlabeled_enum", "canonical_augmentation", true, false, 13},
        {"bipartite_unlabeled_enum", "canonical_augmentation", true, true, 5}};
    for (const auto& c : cases) {
        if (skip_if_missing(c.name)) GTEST_SKIP() << c.name << " not built";
        SCOPED_TRACE(c.name + (c.connected ? " connected" : ""));
        const std::string connected = c.connected ? "--connected" : "";
        const auto defaults = run_cli(c.name, connected, "5\n");
        const auto dedicated = run_cli(c.name, connected + " --algo bipartite_coloring", "5\n");
        const auto legacy = run_cli(c.name, "--algo " + c.legacy + " " + connected, "5\n");
        ASSERT_EQ(defaults.status, 0);
        ASSERT_EQ(dedicated.status, 0);
        ASSERT_EQ(legacy.status, 0);
        EXPECT_EQ(defaults.out, dedicated.out);
        std::set<std::vector<std::pair<int, int>>> actual, expected;
        for (const auto* result : {&dedicated, &legacy}) {
            std::istringstream input(result->out);
            std::size_t count;
            ASSERT_TRUE(static_cast<bool>(input >> count));
            EXPECT_EQ(count, c.count);
            auto& graphs = result == &dedicated ? actual : expected;
            for (std::size_t i = 0; i < count; ++i) {
                int m;
                ASSERT_TRUE(static_cast<bool>(input >> m));
                ASSERT_GE(m, 0);
                std::vector<std::pair<int, int>> edges;
                for (int j = 0; j < m; ++j) {
                    int u, v;
                    ASSERT_TRUE(static_cast<bool>(input >> u >> v));
                    edges.emplace_back(u, v);
                }
                if (c.unlabeled)
                    edges = graph_recognition::gtest_utils::canonical_edge_list(5, edges);
                EXPECT_TRUE(graphs.insert(edges).second);
            }
            std::string extra;
            EXPECT_FALSE(static_cast<bool>(input >> extra));
            EXPECT_EQ(graphs.size(), c.count);
        }
        EXPECT_EQ(actual, expected);
        for (const auto& algo : {std::string("bipartite_coloring"), c.legacy}) {
            const std::string args = "--algo " + algo + " " + connected;
            const auto empty = run_cli(c.name, args, "0\n");
            EXPECT_EQ(empty.status, 0);
            EXPECT_EQ(empty.out, "1\n0\n");
            const auto oversized = run_cli(c.name, args, c.unlabeled ? "64\n" : "65\n");
            EXPECT_EQ(oversized.status, 1);
            EXPECT_TRUE(oversized.out.empty());
        }
    }
}

TEST(EnumCliTest, BipartiteRejectsInvalidAlgorithmOptions) {
    for (const std::string name : {"bipartite_labeled_enum", "bipartite_unlabeled_enum"}) {
        if (skip_if_missing(name)) GTEST_SKIP() << name << " not built";
        for (const auto& args : {"--algo", "--algo unknown", "--alg bipartite_coloring",
                                 "--algo bipartite_coloring extra", "--conected"}) {
            const auto result = run_cli(name, args, "3\n");
            EXPECT_EQ(result.status, 1) << name << " " << args;
            EXPECT_TRUE(result.out.empty()) << name << " " << args;
        }
        const auto wrong = run_cli(name, name == "bipartite_labeled_enum"
            ? "--algo canonical_augmentation" : "--algo reverse_search", "3\n");
        EXPECT_EQ(wrong.status, 1);
        EXPECT_TRUE(wrong.out.empty());
    }
    EXPECT_EQ(run_cli("bipartite_labeled_enum", "--connected", "3\n").status, 1);
}

TEST(EnumCliTest, BipartitePermutationSupportsBothAlgorithmsAndConnectedFiltering) {
    struct Case {
        std::string name;
        std::string legacy;
        bool unlabeled;
        bool connected;
        std::size_t count;
    };
    const Case cases[] = {
        {"bipartite_permutation_labeled_enum", "reverse_search", false, false, 376},
        {"bipartite_permutation_unlabeled_enum", "canonical_augmentation", true, false, 13},
        {"bipartite_permutation_unlabeled_enum", "canonical_augmentation", true, true, 5}};
    for (const auto& c : cases) {
        if (skip_if_missing(c.name)) GTEST_SKIP() << c.name << " not built";
        SCOPED_TRACE(c.name + (c.connected ? " connected" : ""));
        const std::string connected = c.connected ? "--connected" : "";
        const auto defaults = run_cli(c.name, connected, "5\n");
        const auto dedicated = run_cli(c.name, connected + " --algo string_representation", "5\n");
        const auto legacy = run_cli(c.name, "--algo " + c.legacy + " " + connected, "5\n");
        ASSERT_EQ(defaults.status, 0);
        ASSERT_EQ(dedicated.status, 0);
        ASSERT_EQ(legacy.status, 0);
        EXPECT_EQ(defaults.out, dedicated.out);
        std::set<std::vector<std::pair<int, int>>> actual, expected;
        for (const auto* result : {&dedicated, &legacy}) {
            std::istringstream input(result->out);
            std::size_t count;
            ASSERT_TRUE(static_cast<bool>(input >> count));
            EXPECT_EQ(count, c.count);
            auto& graphs = result == &dedicated ? actual : expected;
            for (std::size_t i = 0; i < count; ++i) {
                int m;
                ASSERT_TRUE(static_cast<bool>(input >> m));
                ASSERT_GE(m, 0);
                std::vector<std::pair<int, int>> edges;
                for (int j = 0; j < m; ++j) {
                    int u, v;
                    ASSERT_TRUE(static_cast<bool>(input >> u >> v));
                    edges.emplace_back(u, v);
                }
                if (c.unlabeled)
                    edges = graph_recognition::gtest_utils::canonical_edge_list(5, edges);
                EXPECT_TRUE(graphs.insert(edges).second);
            }
            std::string extra;
            EXPECT_FALSE(static_cast<bool>(input >> extra));
            EXPECT_EQ(graphs.size(), c.count);
        }
        EXPECT_EQ(actual, expected);
        for (const auto& algo : {std::string("string_representation"), c.legacy}) {
            const std::string args = "--algo " + algo + " " + connected;
            const auto empty = run_cli(c.name, args, "0\n");
            EXPECT_EQ(empty.status, 0);
            EXPECT_EQ(empty.out, "1\n0\n");
            const auto oversized = run_cli(c.name, args, c.unlabeled ? "64\n" : "65\n");
            EXPECT_EQ(oversized.status, 1);
            EXPECT_TRUE(oversized.out.empty());
        }
    }
}

TEST(EnumCliTest, BipartitePermutationRejectsInvalidAlgorithmOptions) {
    for (const std::string name : {"bipartite_permutation_labeled_enum", "bipartite_permutation_unlabeled_enum"}) {
        if (skip_if_missing(name)) GTEST_SKIP() << name << " not built";
        for (const auto& args : {"--algo", "--algo unknown", "--alg string_representation",
                                 "--algo string_representation extra", "--conected"}) {
            const auto result = run_cli(name, args, "3\n");
            EXPECT_EQ(result.status, 1) << name << " " << args;
            EXPECT_TRUE(result.out.empty()) << name << " " << args;
        }
        const auto wrong = run_cli(name, name == "bipartite_permutation_labeled_enum"
            ? "--algo canonical_augmentation" : "--algo reverse_search", "3\n");
        EXPECT_EQ(wrong.status, 1);
        EXPECT_TRUE(wrong.out.empty());
    }
    EXPECT_EQ(run_cli("bipartite_permutation_labeled_enum", "--connected", "3\n").status, 1);
}

TEST(EnumCliTest, PermutationSupportsBothAlgorithmsAndConnectedFiltering) {
    struct Case {
        std::string name;
        std::string legacy;
        bool unlabeled;
        bool connected;
        std::size_t count;
    };
    const Case cases[] = {
        {"permutation_labeled_enum", "reverse_search", false, false, 1012},
        {"permutation_unlabeled_enum", "canonical_augmentation", true, false, 33},
        {"permutation_unlabeled_enum", "canonical_augmentation", true, true, 20}};
    for (const auto& c : cases) {
        if (skip_if_missing(c.name)) GTEST_SKIP() << c.name << " not built";
        SCOPED_TRACE(c.name + (c.connected ? " connected" : ""));
        const std::string connected = c.connected ? "--connected" : "";
        const auto defaults = run_cli(c.name, connected, "5\n");
        const auto dedicated = run_cli(c.name, connected + " --algo modular_decomposition", "5\n");
        const auto legacy = run_cli(c.name, "--algo " + c.legacy + " " + connected, "5\n");
        ASSERT_EQ(defaults.status, 0);
        ASSERT_EQ(dedicated.status, 0);
        ASSERT_EQ(legacy.status, 0);
        EXPECT_EQ(defaults.out, dedicated.out);
        std::set<std::vector<std::pair<int, int>>> actual, expected;
        for (const auto* result : {&dedicated, &legacy}) {
            std::istringstream input(result->out);
            std::size_t count;
            ASSERT_TRUE(static_cast<bool>(input >> count));
            EXPECT_EQ(count, c.count);
            auto& graphs = result == &dedicated ? actual : expected;
            for (std::size_t i = 0; i < count; ++i) {
                int m;
                ASSERT_TRUE(static_cast<bool>(input >> m));
                ASSERT_GE(m, 0);
                std::vector<std::pair<int, int>> edges;
                for (int j = 0; j < m; ++j) {
                    int u, v;
                    ASSERT_TRUE(static_cast<bool>(input >> u >> v));
                    edges.emplace_back(u, v);
                }
                if (c.unlabeled)
                    edges = graph_recognition::gtest_utils::canonical_edge_list(5, edges);
                EXPECT_TRUE(graphs.insert(edges).second);
            }
            std::string extra;
            EXPECT_FALSE(static_cast<bool>(input >> extra));
            EXPECT_EQ(graphs.size(), c.count);
        }
        EXPECT_EQ(actual, expected);
        for (const auto& algo : {std::string("modular_decomposition"), c.legacy}) {
            const std::string args = "--algo " + algo + " " + connected;
            const auto empty = run_cli(c.name, args, "0\n");
            EXPECT_EQ(empty.status, 0);
            EXPECT_EQ(empty.out, "1\n0\n");
            const auto oversized = run_cli(c.name, args, c.unlabeled ? "64\n" : "65\n");
            EXPECT_EQ(oversized.status, 1);
            EXPECT_TRUE(oversized.out.empty());
        }
    }
}

TEST(EnumCliTest, PermutationRejectsInvalidAlgorithmOptions) {
    for (const std::string name : {"permutation_labeled_enum", "permutation_unlabeled_enum"}) {
        if (skip_if_missing(name)) GTEST_SKIP() << name << " not built";
        for (const auto& args : {"--algo", "--algo unknown", "--alg modular_decomposition",
                                 "--algo modular_decomposition extra", "--conected"}) {
            const auto result = run_cli(name, args, "3\n");
            EXPECT_EQ(result.status, 1) << name << " " << args;
            EXPECT_TRUE(result.out.empty()) << name << " " << args;
        }
        const auto wrong = run_cli(name, name == "permutation_labeled_enum"
            ? "--algo canonical_augmentation" : "--algo reverse_search", "3\n");
        EXPECT_EQ(wrong.status, 1);
        EXPECT_TRUE(wrong.out.empty());
    }
    EXPECT_EQ(run_cli("permutation_labeled_enum", "--connected", "3\n").status, 1);
}

TEST(EnumCliTest, IntervalUnlabeledSupportsBothAlgorithmsAndConnectedFiltering) {
    const std::string name = "interval_unlabeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << name << " not built";
    for (bool connected : {false, true}) {
        const std::string flag = connected ? "--connected" : "";
        const auto defaults = run_cli(name, flag, "5\n");
        const auto dedicated = run_cli(name, flag + " --algo mpq_tree", "5\n");
        const auto legacy = run_cli(name, "--algo canonical_augmentation " + flag, "5\n");
        ASSERT_EQ(defaults.status, 0);
        ASSERT_EQ(dedicated.status, 0);
        ASSERT_EQ(legacy.status, 0);
        EXPECT_EQ(defaults.out, dedicated.out);
        std::set<std::vector<std::pair<int, int>>> actual, expected;
        for (const auto* result : {&dedicated, &legacy}) {
            std::istringstream input(result->out);
            int count;
            ASSERT_TRUE(static_cast<bool>(input >> count));
            EXPECT_EQ(count, connected ? 15 : 27);
            auto& graphs = result == &dedicated ? actual : expected;
            for (int i = 0; i < count; ++i) {
                int m;
                ASSERT_TRUE(static_cast<bool>(input >> m));
                ASSERT_GE(m, 0);
                std::vector<std::pair<int, int>> edges;
                for (int j = 0; j < m; ++j) {
                    int u, v;
                    ASSERT_TRUE(static_cast<bool>(input >> u >> v));
                    edges.emplace_back(u, v);
                }
                EXPECT_TRUE(graphs.insert(graph_recognition::gtest_utils::canonical_edge_list(5, edges)).second);
            }
            std::string extra;
            EXPECT_FALSE(static_cast<bool>(input >> extra));
        }
        EXPECT_EQ(actual, expected);
        for (const std::string algorithm : {"mpq_tree", "canonical_augmentation"}) {
            const auto empty = run_cli(name, flag + " --algo " + algorithm, "0\n");
            EXPECT_EQ(empty.status, 0);
            EXPECT_EQ(empty.out, "1\n0\n");
            for (const std::string input : {"64\n", "-1\n", "garbage\n"}) {
                const auto invalid = run_cli(name, flag + " --algo " + algorithm, input);
                EXPECT_EQ(invalid.status, 1);
                EXPECT_TRUE(invalid.out.empty());
            }
        }
    }
    for (const std::string args : {"--algo", "--algo unknown", "--algo mpq_tree extra",
                                  "--alg mpq_tree", "--conected"}) {
        const auto invalid = run_cli(name, args, "3\n");
        EXPECT_EQ(invalid.status, 1);
        EXPECT_TRUE(invalid.out.empty());
    }
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

TEST(EnumCliTest, CoIntervalSupportsBothAlgorithms) {
    const std::string name = "co_interval_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    const auto default_result = run_cli(name, "", "4\n");
    const auto dedicated = run_cli(name, "--algo kiyomi_kijima_uno", "4\n");
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
    EXPECT_EQ(run_cli(name, "", "3\n").out.substr(0, 10), "8\n0\n1 1 2\n");
    EXPECT_EQ(run_cli(name, "--algo reverse_search", "3\n").out.substr(0, 10),
              "8\n0\n1 1 3\n");
    for (const auto& args : {"--algo kiyomi_kijima_uno", "--algo reverse_search"}) {
        const auto empty = run_cli(name, args, "0\n");
        EXPECT_EQ(empty.status, 0);
        EXPECT_EQ(empty.out, "1\n0\n");
        for (const auto& input : {"65\n", "-1\n", "garbage\n", ""}) {
            const auto invalid = run_cli(name, args, input);
            EXPECT_EQ(invalid.status, 1) << args << " input=" << input;
            EXPECT_TRUE(invalid.out.empty());
        }
    }
}

TEST(EnumCliTest, CoIntervalRejectsInvalidAlgorithmOptions) {
    const std::string name = "co_interval_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    for (const auto& args : {"--algo", "--algo unknown", "--alg kiyomi_kijima_uno",
                             "--algo kiyomi_kijima_uno extra"}) {
        const auto result = run_cli(name, args, "3\n");
        EXPECT_EQ(result.status, 1) << args;
        EXPECT_TRUE(result.out.empty()) << args;
    }
}

TEST(EnumCliTest, DistanceHereditarySupportsBothAlgorithms) {
    const std::string name = "distance_hereditary_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    const auto default_result = run_cli(name, "", "5\n");
    const auto dedicated = run_cli(name, "--algo vertex_incremental", "5\n");
    const auto legacy = run_cli(name, "--algo reverse_search", "5\n");
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
        EXPECT_EQ(line, "892");
        auto& graphs = result == &dedicated ? dedicated_set : legacy_set;
        while (std::getline(lines, line)) EXPECT_TRUE(graphs.insert(line).second);
        EXPECT_EQ(graphs.size(), 892u);
    }
    EXPECT_EQ(dedicated_set, legacy_set);
    EXPECT_EQ(run_cli(name, "", "2\n").out, "2\n1 1 2\n0\n");
    EXPECT_EQ(run_cli(name, "--algo reverse_search", "2\n").out, "2\n0\n1 1 2\n");
    for (const auto& args : {"", "--algo vertex_incremental", "--algo reverse_search"}) {
        const auto empty = run_cli(name, args, "0\n");
        EXPECT_EQ(empty.status, 0);
        EXPECT_EQ(empty.out, "1\n0\n");
        for (const auto& input : {"65\n", "-1\n", "garbage\n", ""}) {
            const auto invalid = run_cli(name, args, input);
            EXPECT_EQ(invalid.status, 1) << args << " input=" << input;
            EXPECT_TRUE(invalid.out.empty());
        }
    }
    for (const auto& args : {"", "--algo vertex_incremental"}) {
        const auto invalid = run_cli(name, args, "64\n");
        EXPECT_EQ(invalid.status, 1);
        EXPECT_TRUE(invalid.out.empty());
    }
}

TEST(EnumCliTest, DistanceHereditaryRejectsInvalidAlgorithmOptions) {
    const std::string name = "distance_hereditary_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    for (const auto& args : {"--algo", "--algo unknown", "--alg vertex_incremental",
                             "--algo vertex_incremental extra", "--connected"}) {
        const auto result = run_cli(name, args, "3\n");
        EXPECT_EQ(result.status, 1) << args;
        EXPECT_TRUE(result.out.empty()) << args;
    }
}

TEST(EnumCliTest, PtolemaicSupportsBothAlgorithms) {
    const std::string name = "ptolemaic_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    const auto default_result = run_cli(name, "", "5\n");
    const auto dedicated = run_cli(name, "--algo vertex_incremental", "5\n");
    const auto legacy = run_cli(name, "--algo reverse_search", "5\n");
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
        EXPECT_EQ(line, "762");
        auto& graphs = result == &dedicated ? dedicated_set : legacy_set;
        while (std::getline(lines, line)) EXPECT_TRUE(graphs.insert(line).second);
        EXPECT_EQ(graphs.size(), 762u);
    }
    EXPECT_EQ(dedicated_set, legacy_set);
    EXPECT_EQ(run_cli(name, "", "2\n").out, "2\n1 1 2\n0\n");
    EXPECT_EQ(run_cli(name, "--algo reverse_search", "2\n").out, "2\n0\n1 1 2\n");
    for (const auto& args : {"", "--algo vertex_incremental", "--algo reverse_search"}) {
        const auto empty = run_cli(name, args, "0\n");
        EXPECT_EQ(empty.status, 0);
        EXPECT_EQ(empty.out, "1\n0\n");
        for (const auto& input : {"2147483647\n", "-1\n", "garbage\n", ""}) {
            const auto invalid = run_cli(name, args, input);
            EXPECT_EQ(invalid.status, 1) << args << " input=" << input;
            EXPECT_TRUE(invalid.out.empty());
        }
    }
    for (const auto& args : {"", "--algo vertex_incremental"}) {
        const auto invalid = run_cli(name, args, "64\n");
        EXPECT_EQ(invalid.status, 1);
        EXPECT_TRUE(invalid.out.empty());
    }
}

TEST(EnumCliTest, PtolemaicRejectsInvalidAlgorithmOptions) {
    const std::string name = "ptolemaic_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    for (const auto& args : {"--algo", "--algo unknown", "--alg vertex_incremental",
                             "--algo vertex_incremental extra", "--connected"}) {
        const auto result = run_cli(name, args, "3\n");
        EXPECT_EQ(result.status, 1) << args;
        EXPECT_TRUE(result.out.empty()) << args;
    }
}

TEST(EnumCliTest, ThreeLeafPowerSupportsBothAlgorithms) {
    const std::string name = "three_leaf_power_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    const auto default_result = run_cli(name, "", "5\n");
    const auto dedicated = run_cli(name, "--algo vertex_incremental", "5\n");
    const auto legacy = run_cli(name, "--algo reverse_search", "5\n");
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
        EXPECT_EQ(line, "642");
        auto& graphs = result == &dedicated ? dedicated_set : legacy_set;
        while (std::getline(lines, line)) EXPECT_TRUE(graphs.insert(line).second);
        EXPECT_EQ(graphs.size(), 642u);
    }
    EXPECT_EQ(dedicated_set, legacy_set);
    EXPECT_EQ(run_cli(name, "", "2\n").out, "2\n1 1 2\n0\n");
    EXPECT_EQ(run_cli(name, "--algo reverse_search", "2\n").out, "2\n0\n1 1 2\n");
    for (const auto& args : {"", "--algo vertex_incremental", "--algo reverse_search"}) {
        const auto empty = run_cli(name, args, "0\n");
        EXPECT_EQ(empty.status, 0);
        EXPECT_EQ(empty.out, "1\n0\n");
        for (const auto& input : {"2147483647\n", "-1\n", "garbage\n", ""}) {
            const auto invalid = run_cli(name, args, input);
            EXPECT_EQ(invalid.status, 1) << args << " input=" << input;
            EXPECT_TRUE(invalid.out.empty());
        }
    }
    for (const auto& args : {"", "--algo vertex_incremental"}) {
        const auto invalid = run_cli(name, args, "64\n");
        EXPECT_EQ(invalid.status, 1);
        EXPECT_TRUE(invalid.out.empty());
    }
}

TEST(EnumCliTest, ThreeLeafPowerRejectsInvalidAlgorithmOptions) {
    const std::string name = "three_leaf_power_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    for (const auto& args : {"--algo", "--algo unknown", "--alg vertex_incremental",
                             "--algo vertex_incremental extra", "--connected"}) {
        const auto result = run_cli(name, args, "3\n");
        EXPECT_EQ(result.status, 1) << args;
        EXPECT_TRUE(result.out.empty()) << args;
    }
}

TEST(EnumCliTest, ProperIntervalSupportsBothAlgorithms) {
    const std::string name = "proper_interval_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    const auto default_result = run_cli(name, "", "4\n");
    const auto dedicated = run_cli(name, "--algo string_representation", "4\n");
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
        EXPECT_EQ(line, "57");
        auto& graphs = result == &dedicated ? dedicated_set : legacy_set;
        while (std::getline(lines, line)) EXPECT_TRUE(graphs.insert(line).second);
        EXPECT_EQ(graphs.size(), 57u);
    }
    EXPECT_EQ(dedicated_set, legacy_set);
    EXPECT_EQ(run_cli(name, "", "2\n").out, "2\n1 1 2\n0\n");
    EXPECT_EQ(run_cli(name, "--algo reverse_search", "2\n").out, "2\n0\n1 1 2\n");
    for (const auto& args : {"--algo string_representation", "--algo reverse_search"}) {
        const auto empty = run_cli(name, args, "0\n");
        EXPECT_EQ(empty.status, 0);
        EXPECT_EQ(empty.out, "1\n0\n");
        for (const auto& input : {"2147483647\n", "-1\n", "garbage\n", ""}) {
            const auto invalid = run_cli(name, args, input);
            EXPECT_EQ(invalid.status, 1) << args << " input=" << input;
            EXPECT_TRUE(invalid.out.empty());
        }
    }
}

TEST(EnumCliTest, BlockSupportsBothAlgorithmsAndUnlabeledComponents) {
    const std::string labeled = "block_labeled_enum", unlabeled = "block_unlabeled_enum";
    if (skip_if_missing(labeled) || skip_if_missing(unlabeled)) GTEST_SKIP() << "not built";
    const auto default_result = run_cli(labeled, "", "5\n");
    const auto dedicated = run_cli(labeled, "--algo block_graph_gen", "5\n");
    const auto legacy = run_cli(labeled, "--algo reverse_search", "5\n");
    ASSERT_EQ(default_result.status, 0);
    ASSERT_EQ(dedicated.status, 0);
    ASSERT_EQ(legacy.status, 0);
    EXPECT_EQ(default_result.out, dedicated.out);
    std::set<std::string> dedicated_set, legacy_set;
    for (const auto* result : {&dedicated, &legacy}) {
        std::istringstream lines(result->out);
        std::string line;
        ASSERT_TRUE(static_cast<bool>(std::getline(lines, line)));
        EXPECT_EQ(line, "562");
        auto& graphs = result == &dedicated ? dedicated_set : legacy_set;
        while (std::getline(lines, line)) EXPECT_TRUE(graphs.insert(line).second);
        EXPECT_EQ(graphs.size(), 562u);
    }
    EXPECT_EQ(dedicated_set, legacy_set);
    EXPECT_EQ(run_cli(labeled, "--algo reverse_search", "2\n").out, "2\n0\n1 1 2\n");
    for (const auto& args : {"", "--algo block_graph_gen"}) {
        const auto all = run_cli(unlabeled, args, "8\n");
        const auto connected = run_cli(unlabeled, std::string(args) + " --connected", "8\n");
        ASSERT_EQ(all.status, 0);
        ASSERT_EQ(connected.status, 0);
        EXPECT_EQ(all.out.substr(0, all.out.find('\n')), "351");
        EXPECT_EQ(connected.out.substr(0, connected.out.find('\n')), "165");
        EXPECT_EQ(line_count(all.out), 352u);
        EXPECT_EQ(line_count(connected.out), 166u);
        EXPECT_EQ(run_cli(unlabeled, args, "0\n").out, "1\n0\n");
        EXPECT_EQ(run_cli(unlabeled, std::string(args) + " --connected", "0\n").out, "0\n");
    }
    for (const auto& name : {labeled, unlabeled}) {
        for (const auto& args : {"--algo", "--algo unknown", "--algo ''",
                                 "--alg block_graph_gen", "--algo block_graph_gen extra"}) {
            const auto invalid = run_cli(name, args, "3\n");
            EXPECT_EQ(invalid.status, 1);
            EXPECT_TRUE(invalid.out.empty());
        }
        for (const auto& input : {"-1\n", "2147483647\n", "garbage\n", ""}) {
            const auto invalid = run_cli(name, "--algo block_graph_gen", input);
            EXPECT_EQ(invalid.status, 1);
            EXPECT_TRUE(invalid.out.empty());
        }
    }
    EXPECT_EQ(run_cli(labeled, "--connected", "3\n").status, 1);
    EXPECT_EQ(run_cli(unlabeled, "--algo reverse_search", "3\n").status, 1);
}

TEST(EnumCliTest, LamanSupportsHennebergAndLegacyAlgorithms) {
    for (const std::string name : {"laman_labeled_enum", "laman_unlabeled_enum"}) {
        if (skip_if_missing(name)) GTEST_SKIP() << name << " not built";
        const bool labeled = name == "laman_labeled_enum";
        const std::string legacy_arg = labeled ? "--algo reverse_search"
                                                : "--algo canonical_augmentation";
        const auto default_result = run_cli(name, "", "6\n");
        const auto dedicated = run_cli(name, "--algo henneberg", "6\n");
        const auto legacy = run_cli(name, legacy_arg, "6\n");
        ASSERT_EQ(default_result.status, 0);
        ASSERT_EQ(dedicated.status, 0);
        ASSERT_EQ(legacy.status, 0);
        EXPECT_EQ(default_result.out, dedicated.out);
        std::set<std::string> dedicated_set, legacy_set;
        for (const auto* result : {&dedicated, &legacy}) {
            std::istringstream lines(result->out);
            std::string line;
            ASSERT_TRUE(static_cast<bool>(std::getline(lines, line)));
            EXPECT_EQ(line, labeled ? "3355" : "13");
            auto& graphs = result == &dedicated ? dedicated_set : legacy_set;
            while (std::getline(lines, line)) EXPECT_TRUE(graphs.insert(line).second);
            EXPECT_EQ(graphs.size(), labeled ? 3355u : 13u);
        }
        if (labeled) {
            EXPECT_EQ(dedicated_set, legacy_set);
        }
        for (const std::string& args : {std::string(), std::string("--algo henneberg"), legacy_arg}) {
            EXPECT_EQ(run_cli(name, args, "0\n").out, "0\n");
            EXPECT_EQ(run_cli(name, args, "1\n").out, "1\n0\n");
            EXPECT_EQ(run_cli(name, args, "2\n").out, "1\n1 1 2\n");
            for (const auto& input : {"-1\n", "65\n", "2147483647\n", "garbage\n", ""}) {
                const auto invalid = run_cli(name, args, input);
                EXPECT_EQ(invalid.status, 1) << name << " " << args;
                EXPECT_TRUE(invalid.out.empty());
            }
        }
        for (const auto& args : {"", "--algo henneberg"}) {
            const auto invalid = run_cli(name, args, "64\n");
            EXPECT_EQ(invalid.status, 1);
            EXPECT_TRUE(invalid.out.empty());
        }
    }
}

TEST(EnumCliTest, LamanRejectsInvalidAlgorithmOptions) {
    for (const std::string name : {"laman_labeled_enum", "laman_unlabeled_enum"}) {
        if (skip_if_missing(name)) GTEST_SKIP() << name << " not built";
        for (const auto& args : {"--algo", "--algo unknown", "--algo ''",
                                 "--alg henneberg", "--algo henneberg extra", "--connected"}) {
            const auto invalid = run_cli(name, args, "3\n");
            EXPECT_EQ(invalid.status, 1) << name << " " << args;
            EXPECT_TRUE(invalid.out.empty());
        }
        const auto wrong_mode = run_cli(name, name == "laman_labeled_enum"
            ? "--algo canonical_augmentation" : "--algo reverse_search", "3\n");
        EXPECT_EQ(wrong_mode.status, 1);
        EXPECT_TRUE(wrong_mode.out.empty());
    }
}

TEST(EnumCliTest, ProperIntervalRejectsInvalidAlgorithmOptions) {
    const std::string name = "proper_interval_labeled_enum";
    if (skip_if_missing(name)) GTEST_SKIP() << "not built";
    for (const auto& args : {"--algo", "--algo unknown", "--alg string_representation",
                             "--algo string_representation extra", "--connected"}) {
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
    "bipartite_unlabeled_enum", "block_labeled_enum", "block_unlabeled_enum", "bull_free_labeled_enum",
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
    "ptolemaic_labeled_enum", "ptolemaic_unlabeled_enum", "self_complementary_unlabeled_enum",
    "series_parallel_unlabeled_enum", "snark_unlabeled_enum",
    "split_unlabeled_enum", "strongly_regular_unlabeled_enum",
    "three_leaf_power_labeled_enum", "three_leaf_power_unlabeled_enum", "threshold_unlabeled_enum",
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
