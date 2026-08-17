インターバルグラフ
========================

グラフがインターバルグラフかどうかを判定する。
各頂点は実数直線上の区間に対応し、区間が重なる頂点間に辺を持つ。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_234.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``IntervalAlgorithm``
     - 説明
   * - ``BACKTRACKING``
     - 弦グラフ性を確認した後、クリーク木上でクリークパスをバックトラッキングで
       探索する (各頂点のクリークは連結な部分木をなす必要がある)。
   * - ``AT_FREE`` **(既定)**
     - Lekkerkerker--Boland の定理に基づき、グラフが弦グラフかつ
       AT-free (asteroidal triple を持たない) であることを検証する。

.. doxygenenum:: graph_recognition::IntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_interval
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::IntervalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_interval_graphs_reverse_search
   :project: graph_recognition

OEIS カウント検証
----------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付きインターバルグラフの個数が
`OEIS A005215 <https://oeis.org/A005215>`_ の ``1, 2, 8, 61, 822`` と一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_interval(g);

       std::cout << std::boolalpha << result.is_interval << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "interval_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_interval_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
--------------

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_

* D. Corneil, S. Olariu, L. Stewart. "The LBFS structure and recognition of interval graphs."
  *SIAM Journal on Discrete Mathematics*, 23(4):1905--1953, 2009.
  `DOI:10.1137/S0895480100373455 <https://doi.org/10.1137/S0895480100373455>`_
