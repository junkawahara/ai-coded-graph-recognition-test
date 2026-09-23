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
- **The forcing closure must tell A1 from A2** (bug found 2026-09-02, fixed
  2026-09-17): with b fixed in B1, the part of a's side that sees b's side is
  exactly `committed ∩ N(b)` (A1), and the rest (A2) sees nothing across. A
  side-1 vertex sees all of A1 or nothing of A, and never a vertex of A2. The
  closure used to compare a vertex's committed neighbours against the *whole*
  committed set, so as soon as an A2 component was committed, the B1 vertices
  (all of A1 seen, none of A2) looked like "some but not all" and were pulled
  to side 0, collapsing the closure. The smallest witness is two C5 prime bags
  glued along a marker pair (n = 8: 1-2-3-4 and 5-6-7-8 with 1,4 joined to 5,8),
  which the n <= 7 exhaustive sweeps cannot contain; the fix is exercised by
  that graph plus random n = 8..9 graphs against the brute-force oracle.
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
- **Tutte embedding in double precision is a false-NO generator on nested
  inputs.** The barycentric coordinates of a vertex nested d faces deep shrink
  by a constant factor per level (about one bit per level on the Apollonian
  network that keeps stacking onto the face of a fixed edge and the previous
  vertex): the smallest neighbour distance is 248 at d = 5, 1e-8 at d = 40 and
  exactly 0 at d = 59 with outer coordinates of size 1e3. Then `atan2` returns
  garbage, every peripheral candidate fails validation and the result is
  `success = false` on a genuine 3-connected planar graph -- and no choice of
  outer face helps, because the nest shrinks whichever face is outside. Rescaling
  or `long double` only moves the failing depth by a few levels. The fix
  (review_all_1.md 62) is exact rational arithmetic: outer cycle on the integer
  parabola points (i, i^2) (strictly convex in cycle order, no irrational
  cos/sin), the barycentric system solved fraction-free (Bareiss) so that every
  intermediate is an integer minor, back substitution dividing only by exact
  divisors (det * x is an integer vector by Cramer), all coordinates scaled by
  det (a negative det is a point reflection, which preserves orientation), and
  neighbours sorted by half-plane then cross-product sign instead of `atan2`.
  Under Tutte's theorem the drawing is strictly convex, so the first peripheral
  candidate succeeds and the rotation equals the double-precision one up to a
  cyclic shift of each list (the lists start at the angle -pi, whose neighbour
  depends on the coordinates). Exposed as three `PlanarEmbeddingAlgorithm`
  values: `_ADAPTIVE` (double, then exact for the same candidate; the C++
  default), `_EXACT` (rational throughout) and `_DMP_FALLBACK` (double Tutte,
  then DMP). Cost of the exact path on the nested example: 2 ms at n = 62,
  0.5 s at n = 403; the numbers stay far below the Hadamard bound there.
  Pitfall for ADAPTIVE on inputs outside the precondition: every candidate
  fails in double, so the exact solver runs once per candidate -- the same
  asymptotics as EXACT, just slower.
- **1-indexed boundaries**: the leaf-power family searches over 0-indexed clique
  indices, so shift by ±1 at the boundary with the 1-indexed quotient of
  `twins.h`.
