弦二部グラフ
========================

グラフが **弦二部グラフ** (chordal bipartite graph) であるとは、二部グラフで
あり、かつ長さ 6 以上の誘導サイクルを含まないことをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_79.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - 説明
   * - ``CYCLE_CHECK``
     - 誘導偶サイクルの総当たり探索
   * - ``BISIMPLICIAL``
     - Bisimplicial 辺消去 (総当たり)、O(m n^4)
   * - ``FAST_BISIMPLICIAL`` **(既定)**
     - 高速な bisimplicial 辺消去、O(m^2 Delta^2)

.. doxygenenum:: graph_recognition::ChordalBipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chordal_bipartite
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ChordalBipartiteLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_bipartite_labeled_graphs_reverse_search
   :project: graph_recognition


誘導部分グラフ列挙
--------------------------------

``chordal_bipartite_induced_subgraph_enum.h`` は、与えられたホストグラフの
弦二部**誘導**部分グラフ、すなわち ``G[X]`` が弦二部となる頂点部分集合
``X ⊆ V(G)`` をすべて列挙する。誘導部分グラフは頂点集合で定まるため、出力は
辺リストではなく頂点集合の族であり、空集合が常に最初の解となる。

探索は論文の特徴づけに基づく逆探索である。グラフが弦二部であることは、
*weak-simplicial* 頂点 (近傍が独立集合であり、かつ包含関係で全順序をなす頂点)
を繰り返し取り除いて空にできることと同値である。非空な解の親はその最大の
weak-simplicial 頂点を取り除くので、``X ∪ {v}`` が ``X`` の子であるのは、
``v`` が拡大後の部分グラフで weak-simplicial かつそれが最大のときに限る。
子の判定に認識器の呼び出しは不要である。

出力サイズは辺数ではなく頂点数で決まる。このクラスは遺伝的なので、ホスト自身が
弦二部ならば、どれだけ疎であっても ``2^n`` 個の頂点部分集合がすべて解になる。
大きなホストにはストリーミング API を使うこと。

本実装は weak-simplicial 頂点を差分更新せず候補ごとに再計算するため、論文の
償却 ``O(k t Δ^2)`` の界は当てはまらない。異なるのは遅延だけで、列挙される族は
同じである。

.. doxygenenum:: graph_recognition::ChordalBipartiteInducedSubgraphEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteInducedSubgraphEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_bipartite_induced_subgraphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "chordal_bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chordal_bipartite(g);

       std::cout << std::boolalpha << result.is_chordal_bipartite << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chordal_bipartite_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_bipartite_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

誘導部分グラフ列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chordal_bipartite_induced_subgraph_enum.h"

   int main() {
       using namespace graph_recognition;

       // C6: 全体以外のすべての頂点部分集合が弦二部になる
       Graph g(6, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}});
       auto result = enumerate_chordal_bipartite_induced_subgraphs(g);

       std::cout << result.vertex_sets.size() << '\n';  // 63
       return 0;
   }


参考文献
----------------

* M. C. Golumbic, C. F. Goss. "Perfect elimination and chordal bipartite graphs."
  *Journal of Graph Theory*, 2(2):155--163, 1978.
  `DOI:10.1002/jgt.3190020209 <https://doi.org/10.1002/jgt.3190020209>`_

* A. Lubiw. "Doubly lexical orderings of matrices."
  *SIAM Journal on Computing*, 16(5):854--879, 1987.
  `DOI:10.1137/0216057 <https://doi.org/10.1137/0216057>`_

* K. Kurita, K. Wasa, H. Arimura, T. Uno. "An Efficient Algorithm for
  Enumerating Chordal Bipartite Induced Subgraphs in Sparse Graphs."
  *COCOON 2019*, LNCS 11653, 339--351.
  `DOI:10.1007/978-3-030-26176-4_28 <https://doi.org/10.1007/978-3-030-26176-4_28>`_
