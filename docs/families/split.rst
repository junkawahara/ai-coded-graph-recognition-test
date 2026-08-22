スプリットグラフ
================================

グラフが **スプリットグラフ** であるとは、頂点集合をクリークと独立集合に
分割できることをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_39.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SplitAlgorithm``
     - 説明
   * - ``DEGREE_SEQUENCE``
     - グラフとその補グラフの弦グラフ性検査 (enum 名は歴史的経緯による
       もので、次数列に基づく手法は ``HAMMER_SIMEONE``)、O(n^2)
   * - ``HAMMER_SIMEONE`` **(既定)**
     - Hammer-Simeone の次数列条件。次数列は隣接リストの長さから得られ、
       計数ソートで整列するので O(n)

.. doxygenenum:: graph_recognition::SplitAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_split
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::SplitEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_split_graphs_reverse_search
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` について、列挙されたラベル付きスプリットグラフの個数が
`OEIS A179534 <https://oeis.org/A179534>`_ の値
``2, 8, 58, 632, 9654`` と一致することを検証済み。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "split.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_split(g);

       std::cout << std::boolalpha << result.is_split << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "split_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_split_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* P. L. Hammer, B. Simeone. "The splittance of a graph."
  *Combinatorica*, 1(3):275--284, 1981.
  `DOI:10.1007/BF02579333 <https://doi.org/10.1007/BF02579333>`_

* S. Földes, P. L. Hammer. "Split graphs."
  *Congressus Numerantium*, 19:311--315, 1977.
