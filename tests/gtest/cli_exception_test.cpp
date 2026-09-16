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

// The boundary must not change the answer or the status of a normal run.
TEST(CliExceptionTest, NormalRunIsUnaffected) {
    const std::string bin = test_path("../bin/permutation");
    if (access(bin.c_str(), X_OK) != 0) GTEST_SKIP() << "bin/permutation not built";
    const CliResult r = run_shell("printf '4 4\\n1 2\\n2 3\\n3 4\\n4 1\\n' | " + bin);
    EXPECT_EQ(r.status, 0);
    EXPECT_EQ(r.out, "YES\n");
}
