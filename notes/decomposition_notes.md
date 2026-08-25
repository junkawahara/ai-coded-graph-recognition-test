# Exposing decomposition structures (work of 2026-08-25)

Decomposition structures that used to be buried inside recognizers were turned
into public components. Only the pitfalls for reimplementation/modification are
recorded here (see the utilities pages in docs for the catalog and exposition;
moved out of CLAUDE.md).

- **Simple-vertex elimination ≠ strong elimination ordering**: the three
  strongly chordal variants eliminate an arbitrary simple vertex. That is enough
  for recognition but does not yield an SEO. When an SEO is needed, use the
  Farber partial-order construction in `elimination_orderings.h`
  (`StronglyChordalAlgorithm::FARBER_SEO`). SEO verification is "the closed
  neighborhood matrix contains no Γ (`11` / `10`) pattern": for each pair of
  rows, check `min(columns in both) < max(columns in exactly one)`, which is
  cheap with bitsets.
- **Modular decomposition (Gallai recursion)**: the children of a PRIME node are
  the equivalence classes of "min_module({x,y}) is not all of V". The key point
  is to unite the **entire** module returned by the closure at once (uniting
  only pairs makes the number of closure calls blow up).
- **Completeness of the split search**: the seed is three vertices — a crossing
  edge (a,b) plus a second vertex a2 on a's side. With only a and b the closure
  never fires and splits with |A| >= 2 are missed. Exhaustive verification over
  all connected graphs with n=7 (a property test) guards this completeness.
- **SPQR verification pitfalls**: (1) In the polygon 1-4-3-2 the pair {1,3} is a
  separation pair with no virtual edge (a polygon represents all of its own
  split pairs), so verify via "both vertices appear in the same skeleton".
  (2) The converse fails for R nodes (every pair appears in the same skeleton
  but does not separate), so soundness is checked at the poles of virtual
  edges. (3) Degenerate bags themselves can have splits (K5, K_{1,4}, ...), so
  "bags have no split" must not be used as an invariant.
- **DMP planar embedding**: maintain **faces**, not a rotation system, and
  recover the rotation system at the end from face corners (p, v, q → q follows
  p in the rotation at v). This is less error-prone than updating rotations
  directly during face splits. "Prefer a fragment with only one admissible
  face" is required for correctness, not a heuristic. Faces of general graphs
  are not simple cycles (they traverse both sides of a bridge), so
  `embedding_is_valid` cannot be used.
- **1-indexed boundaries**: the leaf-power family searches over 0-indexed clique
  indices, so shift by ±1 at the boundary with the 1-indexed quotient of
  `twins.h`.
