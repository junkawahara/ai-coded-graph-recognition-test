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

.. doxygenenum:: graph_recognition::ChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_graphs_reverse_search
   :project: graph_recognition


OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` について、列挙されたラベル付き弦グラフの個数が
`OEIS A058862 <https://oeis.org/A058862>`_ の値
``2, 8, 61, 822, 18154`` と一致することを検証済み。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "chordal.h"

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
   #include "chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_graphs_reverse_search(4);
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
