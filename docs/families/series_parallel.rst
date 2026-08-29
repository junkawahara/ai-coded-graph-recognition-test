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

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは順列グラフ・サークルグラフ・
弦グラフ・外平面グラフの列挙器と同じ McKay の canonical construction path
法です。直並列グラフはマイナー閉、したがって遺伝的なので、頂点を 1 つずつ
追加してグラフを成長させてよいことが保証されます。枝刈りは候補となる
子グラフごとの ``check_series_parallel`` 呼び出し (キューを用いた直並列簡約)
で、これを同型除去より先に行うので、より高価な正準化は直並列グラフに対して
しか実行されません。Kawano と Nakano の専用列挙器は根付き連結直並列グラフを
1 グラフあたり償却定数時間で正準化なしに生成しますが、本実装は共有の
canonical-augmentation 機構を再利用する方式で、``n = 10`` 程度まで実用的
です。個数は 1, 2, 4, 10, 27, 92, 360, 1715, 9356, ... (n = 1, 2, ...)、
``connected_only`` を指定した場合はそのうち連結なもの
(1, 1, 2, 5, 15, 56, 241, 1245, 7182, ...) です。どちらの数列も
2026 年時点で OEIS に登録されていません。

.. doxygenenum:: graph_recognition::SeriesParallelUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SeriesParallelUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SeriesParallelUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_series_parallel_unlabeled_graphs
   :project: graph_recognition

カウント検証
--------------------------------

非同型列挙については ``n = 8`` まで、列挙器自身とは独立に個数を検証した
(枝刈りなしの canonical augmentation で **すべての** 非同型グラフを列挙し、
``util/minor.h`` の K\ :sub:`4` マイナー直接判定で絞り込んでも同じ個数
``1, 2, 4, 10, 27, 92, 360, 1715`` (連結では
``1, 1, 2, 5, 15, 56, 241, 1245``) が得られる)。どちらの数列も OEIS に
登録されていない (2026-08 確認)。静的テストケースは ``n = 8`` までである。


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

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "series_parallel_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_series_parallel_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 92
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

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* S. Kawano, S.-i. Nakano. "Constant time generation of series-parallel graphs."
  *IEICE Transactions on Fundamentals of Electronics, Communications and Computer Sciences*, E88-A(5):1129--1135, 2005.
  `DOI:10.1093/ietfec/e88-a.5.1129 <https://doi.org/10.1093/ietfec/e88-a.5.1129>`_
