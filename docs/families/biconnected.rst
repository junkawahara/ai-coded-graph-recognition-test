2-連結グラフ
========================

グラフが 2-連結 (biconnected) であるかどうかを判定する。
2-連結グラフとは、頂点数が 3 以上で切断点を持たない連結グラフである。
頂点数が 3 未満のグラフは 2-連結でないと判定する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_771.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::BiconnectedAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_biconnected
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::BiconnectedLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconnected_labeled_graphs
   :project: graph_recognition

両列挙器の既定は ``EAR_AUGMENTATION`` です。Stolee の正準 ear 探索に基づき、
サイクル ``C_3, ..., C_n`` から既存の異なる2頂点間に道を追加します。
内部頂点は新しく作り、合計頂点数を ``n`` 以下に制限します。内部頂点数0の ear
は非辺の追加です。途中状態も常に2頂点連結であり、ちょうど ``n`` 頂点のグラフ
だけを出力します。``n`` 頂点に達した後も辺追加を続けます。

削除後も2頂点連結である最短の ear を正準ラベルの端点順で選び、親を定めます。
親の正準形の一致と兄弟の正準形集合で同型重複を除きます。ラベル付き版は各代表を
``n! / |Aut(G)|`` 個の異なるラベル付きグラフに展開します。EarSearch の nauty・
頂点対軌道による候補削減は使わず、既存の最悪階乗時間の正準化を利用するため、
多項式遅延は主張しません。探索経路・兄弟集合・全出力を保持し、ラベル展開では
現在の代表の訪問済み辺集合も保持します。

従来の頂点追加方式はラベル付き版の ``REVERSE_SEARCH``、ラベルなし版の
``CANONICAL_AUGMENTATION`` で選択でき、出力順序・代表元を維持します。
こちらは終端で2頂点連結性を認識します。両方式に辺数上限 ``max_edges``
（C++ の第3引数、``-1`` は無制限）を指定できます。ear 探索では途中状態が
``k < n`` 頂点、``m`` 辺のとき、``m + n - k + 1 > max_edges`` なら打ち切ります。

CLI は ``--algo ear_augmentation|reverse_search``（ラベル付き）、
``--algo ear_augmentation|canonical_augmentation``（ラベルなし）、
``--max-edges M``（非負の辺数上限）を受け付けます。Python は
``enumerate_biconnected_labeled_graphs(n, algorithm=None, max_edges=None)`` と
``enumerate_biconnected_unlabeled_graphs(n, algorithm=None, max_edges=None)`` です。
``None`` は ear 追加と辺数無制限を指定します。Python の上限はそれぞれ6頂点・
8頂点です。C++・CLI の上限は63頂点で、従来のラベル付き ``REVERSE_SEARCH``
は64頂点を維持します。``n < 3`` は空出力です（Python は正の ``n`` のみ）。
2頂点連結グラフは定義から連結なので ``connected_only`` はありません。

非同型グラフ数は ``n = 3, ..., 8`` で ``1, 3, 10, 56, 468, 7123``
（`OEIS A002218 <https://oeis.org/A002218>`_）です。新旧方式の同型類を ``n = 7``
まで、ラベル付き辺集合を ``n = 6`` まで比較し、辺数制限も無制限列挙を
フィルタした結果と照合します。

.. doxygenenum:: graph_recognition::BiconnectedUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BiconnectedUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconnected_unlabeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/biconnected.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_biconnected(g);

       std::cout << std::boolalpha << result.is_biconnected << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/biconnected_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconnected_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/biconnected_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_biconnected_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 56 = A002218(6)
       return 0;
   }


参考文献
----------------

* R. Tarjan. "Depth-first search and linear graph algorithms."
  *SIAM Journal on Computing*, 1(2):146--160, 1972.
  `DOI:10.1137/0201010 <https://doi.org/10.1137/0201010>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* D. Stolee. "Isomorph-free generation of 2-connected graphs with applications."
  2011. `arXiv:1104.5261 <https://arxiv.org/abs/1104.5261>`_
