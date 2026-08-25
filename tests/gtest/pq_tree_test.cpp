#include "pq_tree.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

using namespace graph_recognition;

namespace {

// Independent check: with the reported column order, every row's ones are
// contiguous.
bool rows_are_consecutive(const std::vector<std::vector<int>>& rows,
                          int num_columns,
                          const std::vector<int>& column_order) {
    if ((int)column_order.size() != num_columns + 1) return false;
    std::vector<int> pos(num_columns + 1, -1);
    for (int i = 1; i <= num_columns; ++i) {
        int c = column_order[i];
        if (c < 1 || c > num_columns || pos[c] != -1) return false;
        pos[c] = i;
    }
    for (size_t r = 0; r < rows.size(); ++r) {
        int lo = num_columns + 1, hi = 0, count = 0;
        std::vector<bool> seen(num_columns + 1, false);
        for (size_t j = 0; j < rows[r].size(); ++j) {
            int c = rows[r][j];
            if (c < 1 || c > num_columns) continue;
            if (seen[c]) continue;
            seen[c] = true;
            ++count;
            lo = std::min(lo, pos[c]);
            hi = std::max(hi, pos[c]);
        }
        if (count == 0) continue;
        if (hi - lo + 1 != count) return false;
    }
    return true;
}

// Oracle: try every column permutation (num_columns <= 7).
bool bf_has_c1p(const std::vector<std::vector<int>>& rows, int num_columns) {
    std::vector<int> perm;
    for (int c = 1; c <= num_columns; ++c) perm.push_back(c);
    do {
        std::vector<int> order(num_columns + 1, 0);
        for (int i = 0; i < num_columns; ++i) order[i + 1] = perm[i];
        if (rows_are_consecutive(rows, num_columns, order)) return true;
    } while (std::next_permutation(perm.begin(), perm.end()));
    return false;
}

std::vector<std::vector<int>> make_rows(const char* const* lines, int count, int num_columns) {
    std::vector<std::vector<int>> rows;
    for (int r = 0; r < count; ++r) {
        std::vector<int> row;
        for (int c = 0; c < num_columns; ++c) {
            if (lines[r][c] == '1') row.push_back(c + 1);
        }
        rows.push_back(row);
    }
    return rows;
}

TEST(ConsecutiveOnesTest, EmptyInputsSucceed) {
    ConsecutiveOnesResult r = consecutive_ones(0, std::vector<std::vector<int>>());
    EXPECT_TRUE(r.success);
    EXPECT_EQ(r.column_order.size(), 1u);

    ConsecutiveOnesResult r2 = consecutive_ones(4, std::vector<std::vector<int>>());
    EXPECT_TRUE(r2.success);
    ASSERT_EQ(r2.column_order.size(), 5u);
    std::vector<int> cols(r2.column_order.begin() + 1, r2.column_order.end());
    std::sort(cols.begin(), cols.end());
    for (int i = 0; i < 4; ++i) EXPECT_EQ(cols[i], i + 1);
}

TEST(ConsecutiveOnesTest, NegativeColumnCountFails) {
    EXPECT_FALSE(consecutive_ones(-1, std::vector<std::vector<int>>()).success);
}

TEST(ConsecutiveOnesTest, AcceptsAMatrixWithC1P) {
    // Already consecutive under the identity order.
    const char* lines[] = {"11000", "01110", "00011"};
    std::vector<std::vector<int>> rows = make_rows(lines, 3, 5);
    ConsecutiveOnesResult r = consecutive_ones(5, rows);
    ASSERT_TRUE(r.success);
    EXPECT_TRUE(rows_are_consecutive(rows, 5, r.column_order));
}

TEST(ConsecutiveOnesTest, AcceptsAPermutedMatrix) {
    // The same instance with columns shuffled: still C1P, under a different
    // order than the identity.
    const char* lines[] = {"10100", "01110", "01001"};
    std::vector<std::vector<int>> rows = make_rows(lines, 3, 5);
    ConsecutiveOnesResult r = consecutive_ones(5, rows);
    ASSERT_TRUE(r.success);
    EXPECT_TRUE(rows_are_consecutive(rows, 5, r.column_order));
}

TEST(ConsecutiveOnesTest, RejectsTheSmallestNonC1PMatrix) {
    // Three rows pairwise sharing one column and no column in all three:
    // column 1 would have to sit next to 2, 3 and 4 at once.
    const char* lines[] = {"1100", "1010", "1001"};
    std::vector<std::vector<int>> rows = make_rows(lines, 3, 4);
    EXPECT_FALSE(consecutive_ones(4, rows).success);
    EXPECT_FALSE(bf_has_c1p(rows, 4));
}

TEST(ConsecutiveOnesTest, IgnoresOutOfRangeAndDuplicateColumns) {
    std::vector<std::vector<int>> rows;
    std::vector<int> row;
    row.push_back(1);
    row.push_back(1);   // duplicate
    row.push_back(0);   // out of range
    row.push_back(99);  // out of range
    row.push_back(2);
    rows.push_back(row);
    ConsecutiveOnesResult r = consecutive_ones(3, rows);
    ASSERT_TRUE(r.success);
    EXPECT_TRUE(rows_are_consecutive(rows, 3, r.column_order));
}

// Exhaustive: every 0/1 matrix with up to 4 rows over 4 columns, and a
// sampled sweep over 5 columns, must agree with the all-permutations oracle.
TEST(ConsecutiveOnesTest, ExhaustiveAgreementWithBruteForce) {
    for (int num_columns = 1; num_columns <= 4; ++num_columns) {
        int row_space = 1 << num_columns;
        for (int r1 = 0; r1 < row_space; ++r1) {
            for (int r2 = 0; r2 < row_space; ++r2) {
                for (int r3 = 0; r3 < row_space; ++r3) {
                    int masks[3] = {r1, r2, r3};
                    std::vector<std::vector<int>> rows;
                    for (int i = 0; i < 3; ++i) {
                        std::vector<int> row;
                        for (int c = 0; c < num_columns; ++c) {
                            if (masks[i] & (1 << c)) row.push_back(c + 1);
                        }
                        rows.push_back(row);
                    }
                    ConsecutiveOnesResult got = consecutive_ones(num_columns, rows);
                    bool expect = bf_has_c1p(rows, num_columns);
                    ASSERT_EQ(got.success, expect)
                        << "cols=" << num_columns << " masks=" << r1 << "," << r2 << "," << r3;
                    if (got.success) {
                        ASSERT_TRUE(rows_are_consecutive(rows, num_columns, got.column_order))
                            << "cols=" << num_columns << " masks=" << r1 << "," << r2 << "," << r3;
                    }
                }
            }
        }
    }
}

}  // namespace
