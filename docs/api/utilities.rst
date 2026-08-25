ユーティリティ
==============

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

minor.h -- マイナーチェック
---------------------------

固定禁止マイナー検出の内部ユーティリティ。全シンボルは
``graph_recognition::detail_minor`` に属しており、公開 API の一部ではありません。
