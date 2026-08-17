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

.. doxygenstruct:: graph_recognition::ClusterEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ClusterEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cluster_graphs
   :project: graph_recognition

OEIS カウント検証
------------------------------

``n = 1, 2, 3, 4, 5, 6, 7, 8`` について、列挙されたラベル付き
クラスターグラフの個数が `OEIS A000110
<https://oeis.org/A000110>`_ のベル数 ``1, 2, 5, 15, 52, 203, 877, 4140`` と
一致することを検証した。


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cluster.h"

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
   #include "cluster_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cluster_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
