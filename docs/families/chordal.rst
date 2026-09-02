弦グラフ
================

グラフが **弦グラフ** (chordal graph, 三角化グラフ) であるとは、長さ 4 以上の
誘導サイクルを含まないことをいう。これは完全消去順序 (perfect elimination
ordering, PEO) を持つことと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_32.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalAlgorithm``
     - 説明
   * - ``MCS_PEO``
     - 優先度付きキューによる Maximum Cardinality Search、O(n + m log n)
   * - ``BUCKET_MCS_PEO`` **(既定)**
     - バケットソートによる Maximum Cardinality Search、O(n + m)
   * - ``LEXBFS_PEO``
     - Rose-Tarjan-Lueker 1976 による LexBFS、O(n + m)

.. doxygenenum:: graph_recognition::ChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chordal
   :project: graph_recognition


列挙
------------

既定の ``KIYOMI_UNO`` は、1 辺グラフを根とし、最小次数の simplicial
vertex（同率では最小ラベル）を除去して親を定める専用逆探索である。子は
新しい頂点の近傍を既存グラフのクリークとして追加し、Kiyomi--Uno の
子判定条件で重複を除く。PEO は探索中に維持され、候補クリークの列挙に使われる。

原論文の O(1) 償却時間・O(1) delay は、最適化された差分出力実装での境界である。
この実装は専用探索木を保ちつつ単純な O(n^2) 状態を用い、callback ごとに完全な
辺リストも構築するため、その境界はこの API には適用されない。旧来の最大ラベル頂点除去による探索は
``LEGACY_VERTEX_REVERSE_SEARCH`` として比較・互換性確認用に残している。

.. doxygenenum:: graph_recognition::ChordalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ChordalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは順列グラフ・サークルグラフの
列挙器と同じ McKay の canonical construction path 法です。弦グラフは遺伝的
なので、頂点を 1 つずつ追加してグラフを成長させてよいことが保証されます。
枝刈りは候補となる子グラフごとの ``check_chordal`` 呼び出し (線形時間の
MCS + PEO 検証) で、これを同型除去より先に行うので、より高価な正準化は
弦グラフに対してしか実行されません。近傍がクリークとなる頂点の追加は常に
弦グラフ性を保ちますが、canonical deletion で削除される頂点 (子の正準
ラベリングで最後に置かれる頂点) は simplicial とは限らないため、認識器の
呼び出しを近傍クリーク判定で置き換えることはできません。個数は OEIS
A048193(n) (1, 2, 4, 10, 27, 94, 393, 2119, 14524, ...)、``connected_only``
を指定した場合はそのうち連結なもの (A048192: 1, 1, 2, 5, 15, 58, 272,
1614, 11911, ...) です。

.. doxygenenum:: graph_recognition::ChordalUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ChordalUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_unlabeled_graphs
   :project: graph_recognition


部分グラフ列挙
------------------------

``chordal_subgraph_enum.h`` は、``n`` 頂点の弦グラフ全体ではなく、
**与えられたホストグラフ**\ に含まれる弦部分グラフを列挙する。これが Kiyomi--Uno の原論文が
扱っている問題そのものであり、上記の列挙はその ``G = K_n`` の特殊ケースにあたる。
ここでの弦部分グラフは全域部分グラフ ``(V, E')``（``E'`` はホストの辺集合の部分集合）
であり、出力は弦グラフとなる辺部分集合と 1 対 1 に対応し、空の辺集合も常に含まれる。

アルゴリズムは同じ逆探索で、子の生成をホストの隣接関係で絞り込むだけである。
このフィルタだけで正しいのは、親の定義が辺の削除しか行わないためである。よって
固定したホストの部分グラフ族は親操作について閉じており、制限された探索木は
ホストの各辺を根とする木のままになる。

出力サイズは頂点数ではなく辺数に支配される。森の部分グラフはすべて弦グラフなので、
辺数 ``m`` のホストは最大 ``2^m`` 個の弦部分グラフを持ちうる。大きなホストでは
ストリーミング版の callback API を使うこと。

.. doxygenenum:: graph_recognition::ChordalSubgraphEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalSubgraphEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_subgraphs
   :project: graph_recognition


OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` について、列挙されたラベル付き弦グラフの個数が
`OEIS A058862 <https://oeis.org/A058862>`_ の値
``2, 8, 61, 822, 18154`` と一致することを検証済み。

非同型列挙については ``n = 9`` まで
`OEIS A048193 <https://oeis.org/A048193>`_ の
``1, 2, 4, 10, 27, 94, 393, 2119, 14524`` と一致することを、列挙器自身とは
独立に検証した (``n`` 頂点の非同型グラフをすべて生成して ``check_chordal``
で絞り込んでも同じ個数が得られる)。``connected_only`` を指定した場合の個数は
`OEIS A048192 <https://oeis.org/A048192>`_ の
``1, 1, 2, 5, 15, 58, 272, 1614, 11911`` と一致する。静的テストケースは
``n = 8`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chordal(g);

       std::cout << std::boolalpha << result.is_chordal << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/chordal_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/chordal_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 94 = A048193(6)
       return 0;
   }

部分グラフ列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/chordal_subgraph_enum.h"

   int main() {
       using namespace graph_recognition;

       // C4: 自身以外のすべての辺部分集合が弦グラフなので 15 個
       Graph host(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = enumerate_chordal_subgraphs(host);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs."
  *Pacific Journal of Mathematics*, 15(3):835--855, 1965.
  `DOI:10.2140/pjm.1965.15.835 <https://doi.org/10.2140/pjm.1965.15.835>`_

* D. J. Rose, R. E. Tarjan, G. S. Lueker. "Algorithmic aspects of vertex elimination on graphs."
  *SIAM Journal on Computing*, 5(2):266--283, 1976.
  `DOI:10.1137/0205021 <https://doi.org/10.1137/0205021>`_

* M. Kiyomi, T. Uno. "Generating chordal graphs included in given graphs."
  *IEICE Transactions on Information and Systems*, E89-D(2):763--770, 2006.
  `DOI:10.1093/ietisy/e89-d.2.763 <https://doi.org/10.1093/ietisy/e89-d.2.763>`_

* R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs."
  *SIAM Journal on Computing*, 13(3):566--579, 1984.
  `DOI:10.1137/0213035 <https://doi.org/10.1137/0213035>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
