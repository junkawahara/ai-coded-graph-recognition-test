プロパーインターバルグラフ
================================

グラフがプロパーインターバルグラフかどうかを判定する。
真の包含関係を持たない区間の族で表現できるインターバルグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_298.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ProperIntervalAlgorithm``
     - 説明
   * - ``TRIPLE_LOOP_CLAW_CHECK``
     - インターバルグラフ判定 + 三重ループによる claw (K_{1,3}) 検出。計算量: O(n * Delta^3)。
   * - ``FAST_CLAW_CHECK`` **(既定)**
     - インターバルグラフ判定 + 辺カウントによる claw 検出。
       N(c) の辺数が d(d-1)/2 未満の場合のみ詳細な探索を行う。
       claw 検出のフィルタは O(m * Delta) だが詳細探索を含む最悪は O(n * Delta^3) で、
       インターバル判定の O(n^3) と合わせて全体では O(n^3 + n * Delta^3)。

.. doxygenenum:: graph_recognition::ProperIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalResult
   :project: graph_recognition
   :members:

``ProperIntervalResult`` は indifference ordering (各閉近傍が連続する頂点順序)
も返す。区間認識が構築する区間モデルから (左端, 右端) でソートするだけで
得られる。一般の区間グラフではこの方法は成り立たないが、claw-free な区間
グラフではどのクリーク区間も他の内側に両側から真に含まれることがない
(そうなると外側頂点の非隣接な 2 頂点と内側頂点で claw ができる) ため成立する。
返す前に順序を直接検証している。

.. doxygenfunction:: graph_recognition::check_proper_interval
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ProperIntervalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_interval_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "proper_interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_interval(g);

       std::cout << std::boolalpha << result.is_proper_interval << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "proper_interval_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_interval_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
--------------

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
