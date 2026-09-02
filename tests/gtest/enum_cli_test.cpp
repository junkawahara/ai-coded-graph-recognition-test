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

}  // namespace
