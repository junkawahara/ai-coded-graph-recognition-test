クラスターグラフ
========================

**クラスターグラフ** は完全グラフの互いに素な和 (disjoint union) である。
同値に、P3-free (3 頂点の誘導パスを含まない) グラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1237.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ClusterAlgorithm``
     - 説明
   * - ``COMPONENT_CLIQUE`` **(既定)**
     - 各連結成分がクリークであることを確認する。

.. doxygenenum:: graph_recognition::ClusterAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ClusterResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cluster
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::ClusterLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ClusterLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cluster_labeled_graphs
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。クラスターグラフのクリークは連結成分そのもの
なので、クリークのサイズの多重集合が完全な同型不変量になります。すなわち
n 頂点の同型類は n の整数分割と 1 対 1 に対応し、分割を非増加順に走査して
各パートに 1 つずつクリークを配置すれば、同型判定なしで各同型類がちょうど
1 回ずつ現れます。個数は OEIS A000041(n) (1, 2, 3, 5, 7, 11, 15, 22, ...)
です。``connected_only`` を指定した場合は、n 頂点で連結なクラスターグラフ
は完全グラフ K_n のみなので、出力はその 1 個だけになります。

.. doxygenenum:: graph_recognition::ClusterUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ClusterUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ClusterUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cluster_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
------------------------------

``n = 1, 2, 3, 4, 5, 6, 7, 8`` について、列挙されたラベル付き
クラスターグラフの個数が `OEIS A000110
<https://oeis.org/A000110>`_ のベル数 ``1, 2, 5, 15, 52, 203, 877, 4140`` と
一致することを検証した。非同型列挙については ``n = 20`` まで
`OEIS A000041 <https://oeis.org/A000041>`_ の分割数 (``1, 2, 3, 5, 7, 11,
15, 22, 30, 42, 56, 77, 101, 135, 176, 231, 297, 385, 490, 627``) と
一致することを検証済み。


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/cluster.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {3, 4}});
       auto result = check_cluster(g);

       std::cout << std::boolalpha << result.is_cluster << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/cluster_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cluster_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/cluster_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cluster_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';  // 5 = 4 の分割数
       return 0;
   }


参考文献
------------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
