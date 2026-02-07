CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra
CXXFLAGS += -Iinclude

TARGETS = interval chordal permutation bipartite chordal_bipartite threshold split cograph block

all: $(TARGETS)

interval: src/interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal: src/chordal_main.cpp $(wildcard include/*.h)
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

clean:
	rm -f $(TARGETS)

.PHONY: all clean
