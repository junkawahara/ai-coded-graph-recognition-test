CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra
CXXFLAGS += -Iinclude

TARGETS = interval chordal chordal_enum permutation bipartite chordal_bipartite threshold split cograph block distance_hereditary ptolemaic proper_interval trivially_perfect comparability at_free co_comparability chain cochain co_interval co_chordal unit_interval quasi_threshold strongly_chordal weakly_chordal bipartite_permutation circular_arc planar outer_planar cactus series_parallel

all: $(TARGETS)

interval: src/interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal: src/chordal_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal_enum: src/chordal_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

permutation: src/permutation_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

bipartite: src/bipartite_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal_bipartite: src/chordal_bipartite_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

threshold: src/threshold_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

split: src/split_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cograph: src/cograph_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

block: src/block_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

distance_hereditary: src/distance_hereditary_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

ptolemaic: src/ptolemaic_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

proper_interval: src/proper_interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

trivially_perfect: src/trivially_perfect_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

comparability: src/comparability_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

at_free: src/at_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_comparability: src/co_comparability_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chain: src/chain_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cochain: src/cochain_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_interval: src/co_interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_chordal: src/co_chordal_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

unit_interval: src/unit_interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

quasi_threshold: src/quasi_threshold_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

strongly_chordal: src/strongly_chordal_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

weakly_chordal: src/weakly_chordal_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

bipartite_permutation: src/bipartite_permutation_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

circular_arc: src/circular_arc_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

planar: src/planar_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

outer_planar: src/outer_planar_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cactus: src/cactus_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

series_parallel: src/series_parallel_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

COMPARE_TARGETS = compare_cograph compare_threshold compare_split \
    compare_series_parallel compare_chain compare_chordal compare_cochain \
    compare_distance_hereditary compare_proper_interval \
    compare_strongly_chordal compare_chordal_bipartite compare_weakly_chordal

compare_%: tests/compare_%.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) $(COMPARE_TARGETS)

.PHONY: all clean
