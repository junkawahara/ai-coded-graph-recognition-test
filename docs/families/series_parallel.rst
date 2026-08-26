直並列グラフ
====================

K\ :sub:`4` マイナーを含まないグラフを **直並列グラフ** という。
直並列グラフはすべて 2-退化であるが、逆は成り立たない
(例: K\ :sub:`4` の細分は 2-退化だが K\ :sub:`4` マイナーを含む)。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_275.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SeriesParallelAlgorithm``
     - 説明
   * - ``MINOR_CHECK``
     - 全走査による直並列簡約 (ペンダント / 直列 / 並列簡約)。
   * - ``QUEUE_REDUCTION`` **(既定)**
     - キューを用いた直並列簡約。

.. doxygenenum:: graph_recognition::SeriesParallelAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SeriesParallelResult
   :project: graph_recognition
   :members:

``SeriesParallelResult::reductions`` は簡約列そのものを返す
(``SPReduction::kind`` が 0 = 孤立, 1 = 端点, 2 = 直列 (辺 u-w を追加),
3 = 並列 (辺 u-w が既存))。この列を再生すると全頂点が消えるため、
そのまま証明書として使える。

.. doxygenfunction:: graph_recognition::check_series_parallel
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::SeriesParallelLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SeriesParallelLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_series_parallel_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "series_parallel.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_series_parallel(g);

       std::cout << std::boolalpha << result.is_series_parallel << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "series_parallel_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_series_parallel_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* R. J. Duffin. "Topology of series-parallel networks."
  *Journal of Mathematical Analysis and Applications*, 10(2):303--318, 1965.
  `DOI:10.1016/0022-247X(65)90125-3 <https://doi.org/10.1016/0022-247X(65)90125-3>`_

* J. Valdes, R. E. Tarjan, E. L. Lawler. "The recognition of series parallel digraphs."
  *SIAM Journal on Computing*, 11(2):298--313, 1982.
  `DOI:10.1137/0211023 <https://doi.org/10.1137/0211023>`_
