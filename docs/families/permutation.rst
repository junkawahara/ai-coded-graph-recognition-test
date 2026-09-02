順列グラフ
==============

グラフが順列グラフかどうかを判定する。
順列 pi に対し、頂点 i と j が隣接するのは、pi によって両者の順序が反転する場合に限る。
G と complement(G) がともに比較可能グラフであることと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_23.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PermutationAlgorithm``
     - 説明
   * - ``BACKTRACKING``
     - G と complement(G) の両方の推移的向き付けをバックトラッキングで探索する。
   * - ``CLASS_BASED`` **(既定)**
     - Gamma クラス (辺の同値類) ごとに向き付けの整合性を検査する。
       未向き付けの辺を貪欲に選択し、各クラス内で制約を伝播させる。

.. doxygenenum:: graph_recognition::PermutationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PermutationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_permutation
   :project: graph_recognition

順列図の構成
--------------

``build_permutation_realizer()`` は順列図そのものを構成する
(Pnueli, Lempel & Even 1971; Golumbic, *Algorithmic Graph Theory and Perfect
Graphs*, Ch. 7)。G の推移的向き付け F1 と補グラフの向き付け F2 に対し、
F1 ∪ F2 と F1 の逆向き ∪ F2 はいずれも推移的トーナメントになる。各頂点の
順位をその 2 つのトーナメントから読み取ると 2 本の線上の位置が定まる。

順位は比較関数によるソートではなく入次数として求める。n 頂点の推移的
トーナメントの入次数はちょうど 0..n-1 なので入次数そのものが位置であり、
定理の適用が正しいことに依存する比較関数を使わずに済む。

.. doxygenstruct:: graph_recognition::PermutationRealizerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::build_permutation_realizer
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::PermutationLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PermutationLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_permutation_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction
path 法 (Johnston (2020) がこのクラスに適用した canonical deletion) です。
順列グラフは遺伝的なので、頂点を 1 つずつ追加してグラフを成長させてよいことが
保証されます。ただし二部グラフや三角形なしグラフの列挙と違い、頂点を追加した
グラフがクラスに留まるかを判定する軽い漸進的テストがないため、枝刈りは候補と
なる子グラフごとの認識器の呼び出しになります。これを同型除去より先に行うので、
より高価な正準化は順列グラフに対してしか実行されません。子グラフは、追加した
頂点がその子の正準ラベリングで最後に置かれる頂点の自己同型軌道に属するときに
限り採用されます。個数は OEIS A123448(n)
(1, 2, 4, 11, 33, 142, 776, 5699, 50723, ...)、``connected_only`` を指定した
場合はそのうち連結なもの (1, 1, 2, 6, 20, 99, 600, 4753, 44068, ...) です。

.. doxygenenum:: graph_recognition::PermutationUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PermutationUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PermutationUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_permutation_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

非同型列挙については ``n = 9`` まで
`OEIS A123448 <https://oeis.org/A123448>`_ の
``1, 2, 4, 11, 33, 142, 776, 5699, 50723`` と一致することを、列挙器自身とは
独立に検証した (``n`` 頂点の非同型グラフをすべて生成して ``check_permutation``
で絞り込んでも同じ個数が得られる)。静的テストケースは ``n = 7`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/permutation.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_permutation(g);

       std::cout << std::boolalpha << result.is_permutation << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/permutation_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_permutation_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/permutation_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_permutation_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 142 = A123448(6)
       return 0;
   }


参考文献
--------------

* A. Pnueli, A. Lempel, S. Even. "Transitive orientation of graphs and identification of permutation graphs."
  *Canadian Journal of Mathematics*, 23(1):160--175, 1971.
  `DOI:10.4153/CJM-1971-016-5 <https://doi.org/10.4153/CJM-1971-016-5>`_

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* Johnston. Canonical-deletion enumeration of permutation graphs, 2020.
