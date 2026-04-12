CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra
CXXFLAGS += -Iinclude

TARGETS = interval interval_enum chordal chordal_enum ptolemaic_enum split_enum cograph_enum proper_interval_enum bipartite_permutation_enum convex_bipartite_enum biconvex_bipartite_enum permutation_enum trivially_perfect_enum distance_hereditary_enum block_enum series_parallel_enum cactus_enum outer_planar_enum planar_enum diamond_free_enum claw_free_enum bipartite_enum comparability_enum co_comparability_enum permutation bipartite chordal_bipartite threshold threshold_enum split cograph block distance_hereditary ptolemaic proper_interval trivially_perfect comparability at_free co_comparability chain chain_enum cochain cochain_enum co_interval co_chordal unit_interval quasi_threshold strongly_chordal weakly_chordal weakly_chordal_enum bipartite_permutation circular_arc planar outer_planar cactus series_parallel trapezoid perfect claw_free diamond_free line_graph line_graph_enum convex_bipartite biconvex_bipartite three_leaf_power three_leaf_power_enum four_leaf_power four_leaf_power_enum five_leaf_power five_leaf_power_enum chordal_bipartite_enum caterpillar_enum tree_enum forest_enum unicyclic_enum ktree_enum triangle_free triangle_free_enum halin_enum self_complementary_enum maximal_planar_enum kregular_enum cubic_enum circle circle_enum eulerian_enum strongly_chordal_enum parity parity_enum meyniel_enum at_free_enum trapezoid_enum circular_arc_enum perfect_enum strongly_regular_enum bull_free bull_free_enum apex apex_enum biconnected biconnected_enum odd_hole_free odd_hole_free_enum even_hole_free even_hole_free_enum cluster cluster_enum gem_free gem_free_enum p5_free p5_free_enum co_chordal_enum co_interval_enum proper_circular_arc proper_circular_arc_enum

all: $(TARGETS)

interval: src/interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

interval_enum: src/interval_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal: src/chordal_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal_enum: src/chordal_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

ptolemaic_enum: src/ptolemaic_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

split_enum: src/split_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cograph_enum: src/cograph_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

proper_interval_enum: src/proper_interval_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

bipartite_permutation_enum: src/bipartite_permutation_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

convex_bipartite_enum: src/convex_bipartite_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

biconvex_bipartite_enum: src/biconvex_bipartite_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

permutation_enum: src/permutation_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

permutation: src/permutation_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

bipartite: src/bipartite_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

bipartite_enum: src/bipartite_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal_bipartite: src/chordal_bipartite_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chordal_bipartite_enum: src/chordal_bipartite_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

threshold: src/threshold_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

threshold_enum: src/threshold_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

split: src/split_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cograph: src/cograph_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

block: src/block_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

block_enum: src/block_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

distance_hereditary: src/distance_hereditary_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

ptolemaic: src/ptolemaic_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

proper_interval: src/proper_interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

trivially_perfect: src/trivially_perfect_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

trivially_perfect_enum: src/trivially_perfect_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

distance_hereditary_enum: src/distance_hereditary_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

comparability: src/comparability_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

comparability_enum: src/comparability_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

at_free: src/at_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

at_free_enum: src/at_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_comparability: src/co_comparability_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_comparability_enum: src/co_comparability_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chain: src/chain_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

chain_enum: src/chain_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cochain: src/cochain_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cochain_enum: src/cochain_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_interval: src/co_interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_chordal: src/co_chordal_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_chordal_enum: src/co_chordal_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

co_interval_enum: src/co_interval_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

unit_interval: src/unit_interval_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

quasi_threshold: src/quasi_threshold_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

strongly_chordal: src/strongly_chordal_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

strongly_chordal_enum: src/strongly_chordal_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

parity: src/parity_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

parity_enum: src/parity_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

meyniel_enum: src/meyniel_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

weakly_chordal: src/weakly_chordal_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

weakly_chordal_enum: src/weakly_chordal_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

bipartite_permutation: src/bipartite_permutation_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

circular_arc: src/circular_arc_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

circular_arc_enum: src/circular_arc_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

planar: src/planar_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

outer_planar: src/outer_planar_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

outer_planar_enum: src/outer_planar_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

planar_enum: src/planar_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

maximal_planar_enum: src/maximal_planar_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

kregular_enum: src/kregular_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cubic_enum: src/cubic_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cactus: src/cactus_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cactus_enum: src/cactus_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

series_parallel: src/series_parallel_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

series_parallel_enum: src/series_parallel_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

trapezoid: src/trapezoid_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

trapezoid_enum: src/trapezoid_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

perfect: src/perfect_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

perfect_enum: src/perfect_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

claw_free: src/claw_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

claw_free_enum: src/claw_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

diamond_free: src/diamond_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

diamond_free_enum: src/diamond_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

triangle_free: src/triangle_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

triangle_free_enum: src/triangle_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

line_graph: src/line_graph_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

line_graph_enum: src/line_graph_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

convex_bipartite: src/convex_bipartite_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

biconvex_bipartite: src/biconvex_bipartite_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

three_leaf_power: src/three_leaf_power_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

three_leaf_power_enum: src/three_leaf_power_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

four_leaf_power: src/four_leaf_power_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

four_leaf_power_enum: src/four_leaf_power_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

five_leaf_power: src/five_leaf_power_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

five_leaf_power_enum: src/five_leaf_power_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

circle: src/circle_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

circle_enum: src/circle_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

eulerian_enum: src/eulerian_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

COMPARE_TARGETS = compare_cograph compare_threshold compare_split \
    compare_series_parallel compare_chain compare_chordal compare_cochain \
    compare_distance_hereditary compare_proper_interval \
    compare_strongly_chordal compare_chordal_bipartite compare_weakly_chordal \
    compare_interval_enum compare_interval_enum_full compare_block \
    compare_claw_free compare_diamond_free compare_diamond_free_enum \
    compare_claw_free_enum \
    compare_line_graph \
    compare_line_graph_enum \
    compare_convex_bipartite \
    compare_biconvex_bipartite \
    compare_block_enum \
    compare_ptolemaic_enum \
    compare_split_enum \
    compare_cograph_enum \
    compare_proper_interval_enum \
    compare_bipartite_permutation_enum \
    compare_convex_bipartite_enum \
    compare_biconvex_bipartite_enum \
    compare_permutation_enum \
    compare_trivially_perfect_enum \
    compare_distance_hereditary_enum \
    compare_series_parallel_enum \
    compare_cactus_enum \
    compare_outer_planar_enum \
    compare_planar_enum \
    compare_comparability_enum \
    compare_co_comparability_enum

caterpillar_enum: src/caterpillar_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

tree_enum: src/tree_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

forest_enum: src/forest_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

unicyclic_enum: src/unicyclic_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

ktree_enum: src/ktree_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

halin_enum: src/halin_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

self_complementary_enum: src/self_complementary_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

strongly_regular_enum: src/strongly_regular_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

bull_free: src/bull_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

bull_free_enum: src/bull_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

apex: src/apex_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

apex_enum: src/apex_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

biconnected: src/biconnected_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

biconnected_enum: src/biconnected_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

odd_hole_free: src/odd_hole_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

odd_hole_free_enum: src/odd_hole_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

even_hole_free: src/even_hole_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

even_hole_free_enum: src/even_hole_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cluster: src/cluster_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

cluster_enum: src/cluster_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

gem_free: src/gem_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

gem_free_enum: src/gem_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

p5_free: src/p5_free_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

p5_free_enum: src/p5_free_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

proper_circular_arc: src/proper_circular_arc_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

proper_circular_arc_enum: src/proper_circular_arc_enum_main.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

compare_%: tests/compare_%.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) $(COMPARE_TARGETS)

.PHONY: all clean
