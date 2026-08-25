ユーティリティ
==============

forbidden_subgraph.h -- NO 側の証明書
--------------------------------------

認識器が NO と答えるとき、そのグラフをクラスから除外している構造そのもの
(ホール、アステロイダル 3 点組、禁止誘導部分グラフ、マイナーのモデル) を
返せる。全認識器がこの 1 つの型を共有するので、合成クラスは部分証明書を
そのまま伝播できる。interval は chordal のホールを、ptolemaic は gem を、
co_* 系は主グラフ側のパターンに ``in_complement`` を立てて再利用する。

``Obstruction`` は対応する ``is_<type>`` が false のときのみ有効。次数列だけで
判定する variant は指し示す頂点を持たないので ``kind`` は ``NONE`` のままに
なる (どの variant がそうかはクラスごとのドキュメントに記載)。抽出が認識より
高コストな場合は ``build_<type>_obstruction()`` ビルダーが担当し、認識自体の
コストは据え置かれる。

``vertices`` は頂点添字ベクトルではなく頂点リストなので、``side`` や ``parent``
に適用される n+1 の 0 埋め規約は適用されない。並び順は kind ごとに固定されて
おり、enum の各項目に記載されている。

.. doxygenenum:: graph_recognition::ObstructionKind
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::Obstruction
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::make_obstruction
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::obstruction_kind_name
   :project: graph_recognition

obstruction_extract.h -- 証明書の抽出
---------------------------------------

違反を検出して問題の頂点を握っている認識器が呼ぶ共有ルーチン群。いずれも
NO パスでのみ実行されるので、コストを払うのはクラス外のグラフだけである。

要となるのは「誘導部分グラフの最短路はそれ自体が誘導パスである」という点。
BFS を閉近傍の補集合に制限すれば、弦なしサイクルがそのまま得られ、別途の
短絡処理は要らない。

.. doxygenfunction:: graph_recognition::detail_obstruction::shortest_path_in_allowed
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::hole_through_center
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::find_hole
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::hole_from_failed_peo
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::hole_from_bfs_path
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::odd_cycle_from_conflict
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::find_induced_p4
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::find_claw
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::find_diamond
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::find_bull
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::find_gem
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::find_p5
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::find_forcing_cycle
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::pattern_from_non_nested
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::split_obstruction_from_hole
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::detail_obstruction::tp_obstruction_from_hole
   :project: graph_recognition

dsu.h -- Union-Find
-------------------

.. doxygenstruct:: graph_recognition::DSU
   :project: graph_recognition
   :members:

mcs.h -- Maximum Cardinality Search
------------------------------------

.. doxygenenum:: graph_recognition::MCSAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MCSResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::mcs
   :project: graph_recognition

lexbfs.h -- Lexicographic Breadth-First Search
-----------------------------------------------

LexBFS 順序の計算アルゴリズムを選択できます。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``LexBFSAlgorithm``
     - 説明
   * - ``SIMPLE_LEXBFS``
     - ラベルリスト (整数列) の辞書式比較で最大ラベル頂点を選択する単純実装。計算量: O(n² + nm)。
   * - ``PARTITION_LEXBFS`` **(既定)**
     - Habib, McConnell, Paul, Viennot (2000) の分割細分化手法。頂点クラスの順序リストを管理し、各ステップで隣接頂点を所属クラスの前方に分離する。計算量: O(n + m)。

.. doxygenenum:: graph_recognition::LexBFSAlgorithm
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::lexbfs
   :project: graph_recognition

clique.h -- 極大クリーク列挙・クリーク木
-----------------------------------------

クリーク木の構築アルゴリズムを選択できます。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CliqueTreeAlgorithm``
     - 説明
   * - ``KRUSKAL``
     - 最大重みスパニング木 (Kruskal 法) によるクリーク木構築。極大クリーク間の共通頂点数を重みとして最大全域木を構築する。
   * - ``INCREMENTAL`` **(既定)**
     - PEO (Perfect Elimination Ordering) 順にインクリメンタルにクリーク木を構築する。PEO の各頂点を順に処理し、新しい極大クリークを検出したら木に追加する。

.. doxygenenum:: graph_recognition::CliqueTreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalCliques
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CliqueTreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_cliques
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::build_clique_tree
   :project: graph_recognition

graph_utils.h -- 基本的なグラフ変換
------------------------------------

補グラフ・隣接行列・誘導部分グラフの共有実装。以前は各認識器が個別の複製を
持っていた。

.. doxygenfunction:: graph_recognition::build_complement
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::build_adj_matrix
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::build_complement_matrix
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::induced_subgraph
   :project: graph_recognition

components.h -- 連結成分・補グラフ連結成分
--------------------------------------------

グラフ全体に加え、頂点部分集合に制限した版も提供する。分解アルゴリズムは
誘導部分グラフの成分を繰り返し必要とするため、部分集合版は元の頂点番号を
そのまま受け取り・返す。

補グラフ版は補グラフを構築しない。未割当頂点のリストを保持し、各ステップで
現在の頂点の非隣接頂点をまとめて移動するため、1 回の呼び出しは O(n + m)。

.. doxygenstruct:: graph_recognition::ComponentsResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::connected_components
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::co_components
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::induced_components
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::induced_co_components
   :project: graph_recognition

twins.h -- Twin クラス・Twin 商グラフ
--------------------------------------

相異なる 2 頂点は N[u] = N[v] のとき true twin (このとき隣接)、
N(u) = N(v) のとき false twin (このとき非隣接)。いずれかの twin である関係は
同値関係になる (u,v が true twin かつ v,w が false twin は矛盾する) ため、
各クラスはクリークか独立集合であり、商グラフが well-defined。

true twin クラスは leaf power 認識器が使う **critical clique**、
true/false 両方の不動点縮約は circle グラフの Naji 系を縮小する前処理。

片方の種類だけなら 1 ラウンドで不動点だが、両方を混ぜる場合は反復が必要
(C4 の false twin を縮約すると K2 になり、その 2 頂点は true twin)。

.. doxygenenum:: graph_recognition::TwinKind
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TwinQuotientResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::contract_twins
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::critical_clique_quotient
   :project: graph_recognition

block_cut_tree.h -- 二重連結成分・ブロックカット木
------------------------------------------------------

辺スタック DFS (Hopcroft & Tarjan 1973) でブロック分解する。

カット頂点と橋は別途検出しない。頂点がカット頂点であるのは 2 個以上の
ブロックに属するときちょうどであり、橋は辺 1 本からなるブロックちょうど
である。ブロックから導出することで、頂点ベースの関節点判定が必要とする
「DFS 木の根の特別扱い」を回避している。

孤立頂点は単独ブロック (K1) として報告するため、全頂点がいずれかのブロックに
属する。非連結グラフでは森になる。

.. doxygenstruct:: graph_recognition::BlockCutTreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_block_cut_tree
   :project: graph_recognition

pq_tree.h -- PQ-tree と連続 1 性 (C1P)
----------------------------------------

Booth & Lueker (1976) の PQ-tree と、その上に構築した連続 1 性判定。
入口は ``consecutive_ones()``。行を逐次的に簡約したい呼び出し側のために
``PQTree`` 自体も公開している。

実装は正しいが線形時間ではない。原論文の BUBBLE パスを実装していないため、
各簡約は pertinent な葉それぞれから根まで遡る。

列番号はライブラリの 1-indexed 規約に合わせて 1 .. num_columns
(内部の ``detail::check_c1p_pq_tree`` は 0 始まり)。

.. doxygenstruct:: graph_recognition::ConsecutiveOnesResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::consecutive_ones
   :project: graph_recognition

.. doxygenenum:: graph_recognition::PQNodeType
   :project: graph_recognition

.. doxygenenum:: graph_recognition::PQLabel
   :project: graph_recognition

md_tree.h -- 分解木 (modular decomposition / cotree 共通)
----------------------------------------------------------

木のノードはモジュールを表す。葉は単一頂点、内部ノードは子同士の関係で
ラベル付けされる: どの 2 子も結合されないなら PARALLEL、すべて結合される
なら SERIES、それ以外は PRIME。cotree は PRIME を持たないこの木そのもの
なので、cograph.h と modular_decomposition.h は同じ構造を作り、直接比較
できる。

構築側はノードを作り (親は必ず子より小さい添字)、葉の頂点を記録するだけで
よい。``md_finalize()`` が各ノードの頂点集合を導出し、子を正準順に並べ、
商グラフを構築する。

.. doxygenenum:: graph_recognition::MDNodeKind
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MDNode
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::MDTree
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::md_finalize
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::md_rebuild_graph
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::md_is_cotree
   :project: graph_recognition

transitive_orientation.h -- 推移的向き付け
--------------------------------------------

グラフが comparability グラフであるのは、辺を向き付けて推移的にできる
ちょうどそのとき、すなわち半順序の comparability グラフであるとき。
comparability.h / co_comparability.h / permutation.h /
bipartite_permutation.h / trapezoid.h はこのヘッダの上に構築されている。

公開エントリポイントは、返す直前に向き付けの推移性を検証する。行 bitset で
O(n^3 / 64) と探索に比べて無視できるコストであり、伝播のバグを「静かな誤答」
ではなく明示的な失敗に変える。

.. doxygenenum:: graph_recognition::TransitiveOrientationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TransitiveOrientationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::transitive_orientation
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::transitive_orientation_matrix
   :project: graph_recognition

elimination_orderings.h -- Strong elimination ordering
--------------------------------------------------------

頂点が *simple* であるとは、その近傍の閉近傍が包含で線形順序をなすこと。
グラフが strongly chordal であるのは simple 頂点を繰り返し削除して空に
できるちょうどそのとき。したがって任意の simple 頂点消去で**認識**はできるが、
それだけでは strong elimination ordering にはならない (より強い条件)。

そこで Farber の構成を用いる: 各段階で残りグラフの狭義閉近傍包含を部分順序へ
累積し、その中で極小な simple 頂点を削除する (同率は最小ラベル)。
得られた順序は返す前に定義通り検証される。

順序 v1, ..., vn が strong elimination ordering であるのは、すべての i < j,
k < l について「vk, vl ∈ N[vi] かつ vk ∈ N[vj] ならば vl ∈ N[vj]」が成り立つ
ちょうどそのとき。すなわち閉近傍行列がこの順序で

  1 1
  1 0

というパターン (Γ) を含まないこと。perfect elimination はここから従う。

.. doxygenstruct:: graph_recognition::StrongEliminationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_strong_elimination_ordering
   :project: graph_recognition

tree_layout.h -- Indifference tree-layout とそのブロック木
------------------------------------------------------------

グラフの indifference tree-layout とは、各頂点の近傍が根からその頂点への
パスを下方に延長した区間になるような、頂点上の根付き木のこと
(proper interval グラフを定義する頂点順序を木へ一般化したもの)。弦グラフが
これを持つのは proper chordal であるちょうどそのとき
(Paul & Protopapas, STACS 2024)。

構成は、ある頂点を根とする layout の **ブロック木** (論文 Algorithm 1) を経由し、
各ブロックの nested-convex 条件 (Algorithm 2) を検査する。

計算量: 論文のアルゴリズムは O(n^4) だが、ここでの nested-convex 検査は
ブロックの全頂点順序を列挙するため、最悪はブロックサイズの階乗。

.. doxygenstruct:: graph_recognition::LayoutBlockTree
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TreeLayoutResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_layout_block_tree
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::find_indifference_tree_layout
   :project: graph_recognition

tree_decomposition.h -- 木分解 (弦グラフ)
------------------------------------------

木分解とは、木の各節点に頂点の集合 (bag) を割り当てたもので、全頂点が
いずれかの bag に入り、全辺がある bag の内側に収まり、ある 1 頂点を含む
bag 全体が連結な部分木をなすもの。幅は最大 bag サイズ - 1 であり、
グラフの treewidth は全木分解にわたる幅の最小値。

弦グラフではクリーク木がそのまま最適な木分解になる: bag は極大クリークで、
running intersection property が部分木条件そのものであり、どの分解も最大
クリークより小さい bag にはできない。したがって本ヘッダは探索ではなく
``clique.h`` の読み替えである。一般のグラフの treewidth 計算は NP 困難なので、
対象は弦グラフに限られる。

非連結グラフではクリーク木が森になるが、異なる成分の bag は頂点を共有しない
ため、任意に連結して木にしても性質は保たれる。

.. doxygenstruct:: graph_recognition::TreeDecompositionResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::tree_decomposition_from_clique_tree
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::tree_decomposition_chordal
   :project: graph_recognition

modular_decomposition.h -- Modular decomposition 木
-----------------------------------------------------

*モジュール* とは、外から見て区別できない頂点集合のこと: 集合外の各頂点は、
その集合の全体に隣接するか、全く隣接しないかのいずれか。modular decomposition
木は、他のどのモジュールとも交差しない **strong module** を、葉が頂点である
木として記録する。

構成は Gallai の再帰に従う。頂点集合 V に対し:

- 1 頂点 → 葉
- G[V] が非連結 → PARALLEL ノード、子は連結成分
- 補が非連結 → SERIES ノード、子は補連結成分
- 両方連結 → PRIME ノード、子は極大真モジュール
  (Gallai の定理よりこれらは互いに素で V を覆う)

唯一自明でない部品は「与えられた 2 頂点を含む最小モジュール」であり、閉包で
求める: 2 頂点から始め、現在の集合の一部にだけ隣接する頂点 (*splitter*) を
繰り返し吸収する。その 2 頂点を含むどのモジュールも吸収された頂点を含まねば
ならないため、不動点が最小モジュールになる。2 頂点が同じ極大真モジュールに
属するのは閉包が V 全体にならないちょうどそのときで、prime グラフの極大真
モジュールは互いに素なので union-find で復元できる。

計算量: 最悪 O(n^4) (prime ノードごとに O(n^2) 回の閉包、各 O(n^2))。
線形時間法 (Tedder ら) は実装していない。

.. doxygenfunction:: graph_recognition::modular_decomposition
   :project: graph_recognition

split_decomposition.h -- Split と Cunningham の split decomposition
--------------------------------------------------------------------

連結グラフの *split* とは、両側に 2 頂点以上ある二分割 (A, B) で、両側を
またぐ辺がちょうど A1 × B1 の全ペアになるもの (A1 は B を見る A の部分、
B1 は A を見る B の部分)。split に沿って分割すると、相手側を表す **marker**
頂点を各々が持つ 2 つの小さいグラフに置き換えられる。どのバッグにも split が
なくなるまで再帰し、split が不必要に分けてしまった degenerate バッグの対を
併合し直すと、Cunningham の正準分解が得られる。

グラフが距離遺伝的であるのは、その分解が prime バッグを持たない
(全バッグがクリークか星) ちょうどそのとき
(``DistanceHereditaryAlgorithm::SPLIT_DECOMPOSITION``)。

**split 探索の完全性**: どの split にもまたぐ辺 (a, b) と a 側の第 2 の頂点
a2 が存在するので、この 3 点で閉包を seed し全通り試せば必ず見つかる。閉包は
「そのような split すべてが a 側に置く頂点」だけを確定するので、不動点は真の
a 側に含まれる。確定された成分は反対側を一切見なくなるため、不動点自身が
split になる。いずれにせよ候補は定義通り検査される。

クリークと星は split を持つが分割しない: 分割して併合し直すと同じバッグに
戻るため、その往復を省いているだけ。

.. doxygenenum:: graph_recognition::SplitNodeKind
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitBag
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SplitTreeEdge
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SplitDecompositionResult
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SplitResultPair
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::find_split
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::split_decomposition
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::is_totally_decomposable
   :project: graph_recognition

planar_embedding.h -- 平面埋め込み
------------------------------------

アルゴリズム:

- ``TUTTE_3CONNECTED`` **(既定)**: Tutte の重心写像で座標を求め、そこから
  回転系と面を読み取る。3-連結平面グラフ限定だが座標も得られる。
- ``DMP_GENERAL``: Demoucron, Malgrange & Pertuiset (1964)。任意の平面グラフに
  適用できる。サイクルを埋め込み、以降は fragment の経路を 1 本ずつ、それを
  収容できる面に埋め込んで面を 2 つに分割していく。

DMP はグラフを二重連結ブロックに分け、各ブロックを個別に埋め込んでから
カット頂点で回転を接合する (2 つのブロックはその頂点しか共有しないので、
回転の断片をどう交互配置しても正しい埋め込みになる)。橋と孤立頂点は
ブロック分解から K2 / K1 ブロックとして出てくるので特別扱いは要らない。

検証は 2 つのバリアントで異なる。3-連結平面グラフの面は長さ 3 以上の単純
サイクルだが、一般のグラフではそうならない (面は橋の両側を通り、カット頂点を
複数回訪れる)。そのため一般版の検証は成分ごとのオイラーの公式と
「各 half-edge がちょうど 1 つの面に乗る」ことだけを要求する。

.. doxygenenum:: graph_recognition::PlanarEmbeddingAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarEmbeddingResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_planar_embedding
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::compute_planar_embedding_dmp
   :project: graph_recognition

spqr_tree.h -- Tutte の 3-連結成分分解 (SPQR 木)
--------------------------------------------------

二重連結グラフは、**多角形** (サイクル)、**ボンド** (2 頂点を結ぶ複数の平行辺)、
単純 3-連結グラフへ一意に分解でき、それらは **仮想辺** の対で貼り合わされる。
SPQR の用語ではそれぞれ S / P / R ノード。Q ノード (単一辺) は現代的な慣習に
従い使わない。

構成は **分割ペア** {x, y} での分割を繰り返す: 辺は分離クラス (x と y を
取り除いたとき連結を保つ塊、および x-y 辺 1 本ごとに 1 クラス) に分かれ、
それらを各々 2 辺以上の 2 グループに分ける任意の分け方が split になる。
両側は相手を表す仮想 x-y 辺を持つ。三角形や K4 はこの意味で分割できず、
それが再帰の停止条件になる。

分割だけでは正準分解にならない (長いサイクルを 2 つに切り、大きなボンドを
2 つに割ってしまう)。マージパスがそれを戻す: 仮想辺対を共有する 2 つの多角形、
または 2 つのボンドは 1 つになる。Tutte の一意性定理により、結果はどの split を
取ったかに依存しない。

Hopcroft & Tarjan の線形時間法は意図的に実装していない: 発表後に訂正を要した
教科書的事例であり (Gutwenger & Mutzel 2001)、本ライブラリに不足しているのは
速度ではない。ここでの二次の分割ペア探索は、全数の分離ペアと照合して検証する。

.. doxygenenum:: graph_recognition::SPQRNodeKind
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SPQRNode
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SPQRTreeEdge
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SPQRTreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_spqr_tree
   :project: graph_recognition

minor.h -- マイナーチェック
---------------------------

固定禁止マイナー検出の内部ユーティリティ。全シンボルは
``graph_recognition::detail_minor`` に属しており、公開 API の一部ではありません。
