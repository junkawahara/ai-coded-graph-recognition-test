// CLI-level regression test for the shared exception boundary
// (include/util/cli_main.h).
//
// Several recognizers use Theta(n^2) memory even on sparse inputs, so a large
// path can exhaust memory. That used to end in std::terminate ("terminate
// called after throwing an instance of 'std::bad_alloc'", SIGABRT, core
// dump); now every CLI reports the failure on stderr and exits with status 2,
// printing nothing that could be read as YES/NO. The binaries are built by
// "make", not by "make gtest_all", so the test skips when its binary is
// absent.

#include "test_helpers.h"
#include <gtest/gtest.h>

#include <cstdio>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

using graph_recognition::gtest_utils::test_path;

namespace {

struct CliResult {
    int status;
    std::string out;  // stdout and stderr together
};

/** @brief Runs a shell command, capturing stdout and stderr */
CliResult run_shell(const std::string& command) {
    CliResult res;
    res.status = -1;
    FILE* pipe = popen((command + " 2>&1").c_str(), "r");
    if (pipe == 0) return res;
    char buffer[4096];
    while (std::fgets(buffer, sizeof(buffer), pipe) != 0) res.out += buffer;
    const int closed = pclose(pipe);
    res.status = WIFEXITED(closed) ? WEXITSTATUS(closed) : -1;
    return res;
}

}  // namespace

// permutation builds two (n+1) x (n+1) byte matrices: 3.2 GB for a
// 40000-vertex path, far above the 256 MB address-space cap, so the
// allocation throws std::bad_alloc inside the recognizer.
TEST(CliExceptionTest, AllocationFailureEndsWithADiagnosticAndStatus2) {
    const std::string bin = test_path("../bin/permutation");
    if (access(bin.c_str(), X_OK) != 0) GTEST_SKIP() << "bin/permutation not built";
    const std::string cmd =
        "sh -c 'ulimit -v 262144 && awk \"BEGIN { n = 40000; print n, n - 1;"
        " for (i = 1; i < n; ++i) print i, i + 1 }\" | " + bin + "'";
    const CliResult r = run_shell(cmd);
    EXPECT_EQ(r.status, 2) << r.out;
    EXPECT_NE(r.out.find("std::bad_alloc"), std::string::npos) << r.out;
    EXPECT_EQ(r.out.find("YES"), std::string::npos) << r.out;
    EXPECT_EQ(r.out.find("NO"), std::string::npos) << r.out;
}

// A recursion-depth refusal (std::runtime_error, review_all_1.md #3-#6) goes
// through the same boundary: status 2, the reason on stderr, no YES/NO.
TEST(CliExceptionTest, RecursionLimitRefusalEndsWithADiagnosticAndStatus2) {
    const std::string bin = test_path("../bin/meyniel");
    if (access(bin.c_str(), X_OK) != 0) GTEST_SKIP() << "bin/meyniel not built";
    const std::string cmd =
        "awk \"BEGIN { n = 17000; print n, n - 1; for (i = 1; i < n; ++i) print i, i + 1 }\" | " + bin;
    const CliResult r = run_shell(cmd);
    EXPECT_EQ(r.status, 2) << r.out;
    EXPECT_NE(r.out.find("recursion limit"), std::string::npos) << r.out;
    EXPECT_NE(r.out.find("result unknown"), std::string::npos) << r.out;
    EXPECT_EQ(r.out.find("YES"), std::string::npos) << r.out;
    EXPECT_EQ(r.out.find("NO"), std::string::npos) << r.out;
}

// The boundary must not change the answer or the status of a normal run.
TEST(CliExceptionTest, NormalRunIsUnaffected) {
    const std::string bin = test_path("../bin/permutation");
    if (access(bin.c_str(), X_OK) != 0) GTEST_SKIP() << "bin/permutation not built";
    const CliResult r = run_shell("printf '4 4\\n1 2\\n2 3\\n3 4\\n4 1\\n' | " + bin);
    EXPECT_EQ(r.status, 0);
    EXPECT_EQ(r.out, "YES\n");
}

// The directed CLIs (digraph / poset / tournament) read their arcs through
// read_directed() rather than Graph::read(); review_all_1.md #21 found that
// an empty, unparsable, negative or truncated input passed as an (empty) YES
// with status 0. They now follow the same contract as the undirected CLIs.
TEST(CliExceptionTest, DirectedClisRefuseMalformedInput) {
    const char* names[3] = {"digraph", "poset", "tournament"};
    const char* inputs[5] = {"", "abc\\n", "-1 0\\n", "3 -1\\n", "3 2\\n1 2\\n"};
    int checked = 0;
    for (int i = 0; i < 3; ++i) {
        const std::string bin = test_path(std::string("../bin/") + names[i]);
        if (access(bin.c_str(), X_OK) != 0) continue;
        ++checked;
        for (int j = 0; j < 5; ++j) {
            const CliResult r =
                run_shell(std::string("printf -- '") + inputs[j] + "' | " + bin);
            EXPECT_EQ(r.status, 1) << names[i] << " on input " << j;
            EXPECT_NE(r.out.find("malformed graph input"), std::string::npos)
                << names[i] << " on input " << j << ": " << r.out;
            EXPECT_EQ(r.out.find("YES"), std::string::npos) << names[i] << " on input " << j;
        }
        // A well-formed input is still answered (a tournament on 3 vertices
        // needs all 3 arcs; the 2-arc chain is a Hasse diagram and a digraph).
        const char* good = i == 2 ? "printf '3 3\\n1 2\\n2 3\\n1 3\\n' | "
                                  : "printf '3 2\\n1 2\\n2 3\\n' | ";
        const CliResult ok = run_shell(good + bin);
        EXPECT_EQ(ok.status, 0) << names[i];
        EXPECT_EQ(ok.out, "YES\n") << names[i];
    }
    if (checked == 0) GTEST_SKIP() << "no directed CLI built";
}
