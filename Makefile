CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra
CXXFLAGS += -Iinclude

# CLI targets are derived from src/*_main.cpp; binaries are emitted into bin/.
NAMES   := $(patsubst src/%_main.cpp,%,$(wildcard src/*_main.cpp))
TARGETS := $(addprefix bin/,$(NAMES))

all: $(TARGETS)

bin/%: src/%_main.cpp $(wildcard include/*/*.h)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $<

# Convenience: "make <name>" builds bin/<name>.
.PHONY: $(NAMES)
$(NAMES): %: bin/%

# ---- Google Test ----
GTEST_DIR     := third_party/googletest/googletest
GTEST_INC     := -I$(GTEST_DIR)/include -I$(GTEST_DIR)
GTEST_OBJ_DIR := build/gtest
GTEST_LIB     := $(GTEST_OBJ_DIR)/libgtest.a

# Google Test requires C++17; library headers stay C++11 compatible.
TEST_STD      := -std=c++17

$(GTEST_OBJ_DIR)/gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc
	@mkdir -p $(GTEST_OBJ_DIR)
	$(CXX) $(TEST_STD) -O2 -pthread $(GTEST_INC) -c $< -o $@

$(GTEST_LIB): $(GTEST_OBJ_DIR)/gtest-all.o
	ar rcs $@ $^

# ---- gtest_all test binary ----
TEST_DIR      := tests/gtest
TEST_SRCS     := $(shell find $(TEST_DIR) -name '*.cpp' 2>/dev/null)
TEST_OBJ_DIR  := build/test_obj
TEST_OBJS     := $(patsubst $(TEST_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_SRCS))
TEST_CXXFLAGS := $(TEST_STD) -O2 -Wall -Wextra -Iinclude \
                 -I$(TEST_DIR)/helpers $(GTEST_INC) \
                 -DTESTS_DATA_DIR=\"$(abspath tests)\" -pthread -MMD -MP

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

-include $(TEST_OBJS:.o=.d)

gtest_all: $(TEST_OBJS) $(GTEST_LIB)
	$(CXX) $(TEST_CXXFLAGS) $^ -o $@ -pthread

# Tests excluded from the default "make test" run (keep this list in sync with
# TEST_DEFAULT_FILTER below and with the "テスト" section of CLAUDE.md):
#   *FullereneUnlabeledEnum*                  — enumerator n>=20 takes hours
#   *CubicPlanarLabeledEnum*case6           — n=10 (5826240 graphs) takes ~280 s
#   */CircularArcLabeledEnumTest.*case6     — n=6 (28081 graphs) reverse search takes ~250 s on an
#                                      idle machine, ~520 s under parallel load; the rest of
#                                      "make test" runs in ~6 s, so this one case dominated it.
#                                      (the leading "/" keeps ProperCircularArcLabeledEnumTest in)
#   *Property*                       — randomized property tests (use test-quick/test-all)
TEST_SLOW_ENUM_FILTER := *FullereneUnlabeledEnum*:*CubicPlanarLabeledEnum*case6:*/CircularArcLabeledEnumTest.*case6
TEST_DEFAULT_FILTER := -$(TEST_SLOW_ENUM_FILTER):*Property*

test: gtest_all
	./gtest_all "--gtest_filter=$(TEST_DEFAULT_FILTER)"

# Default set plus the property tests; only the multi-minute enum cases stay out.
test-quick: gtest_all
	./gtest_all "--gtest_filter=-$(TEST_SLOW_ENUM_FILTER)"

test-all: gtest_all
	./gtest_all

clean-test:
	rm -rf build gtest_all

clean: clean-test
	rm -rf bin
	@# Transitional: builds made before the binaries moved into bin/ left
	@# the executables at the repository root under the same names; remove
	@# them too so stale pre-move binaries cannot shadow bin/ or end up in
	@# a commit via "git add -A".
	rm -f $(NAMES)

.PHONY: all clean clean-test test test-quick test-all
