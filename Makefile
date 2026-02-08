CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra
CXXFLAGS += -Iinclude

TARGETS = interval chordal chordal_enum permutation bipartite chordal_bipartite threshold split cograph block distance_hereditary ptolemaic
V2_TARGETS = interval_v2 permutation_v2 chordal_bipartite_v2 distance_hereditary_v2 ptolemaic_v2

all: $(TARGETS) $(V2_TARGETS)

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

interval_v2: src/interval_v2_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

permutation_v2: src/permutation_v2_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal_bipartite_v2: src/chordal_bipartite_v2_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

distance_hereditary_v2: src/distance_hereditary_v2_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

ptolemaic_v2: src/ptolemaic_v2_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) $(V2_TARGETS)

.PHONY: all clean
