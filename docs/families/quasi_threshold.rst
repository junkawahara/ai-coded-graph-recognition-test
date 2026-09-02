準閾値グラフ
========================================

**quasi-threshold graph** は trivially perfect graph の別名である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_781.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``QuasiThresholdAlgorithm``
     - 説明
   * - ``DFS`` **(既定)**
     - DFS に基づく認識

.. doxygenenum:: graph_recognition::QuasiThresholdAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::QuasiThresholdResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_quasi_threshold
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/quasi_threshold.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_quasi_threshold(g);

       std::cout << std::boolalpha << result.is_quasi_threshold << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

このクラスには現在専用の列挙関数が用意されていない。ごく小さい ``n`` については、候補となるラベル付きグラフを列挙し、認識でフィルタリングする:

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "util/graph.h"
   #include "recognizers/quasi_threshold.h"

   int main() {
       using namespace graph_recognition;

       const int n = 4;
       std::vector<std::pair<int, int>> all_edges;
       for (int u = 1; u <= n; ++u) {
           for (int v = u + 1; v <= n; ++v) {
               all_edges.push_back(std::make_pair(u, v));
           }
       }

       int count = 0;
       const int total_masks = 1 << static_cast<int>(all_edges.size());
       for (int mask = 0; mask < total_masks; ++mask) {
           std::vector<std::pair<int, int>> edges;
           for (int i = 0; i < static_cast<int>(all_edges.size()); ++i) {
               if (mask & (1 << i)) {
                   edges.push_back(all_edges[i]);
               }
           }

           Graph g(n, edges);
           if (check_quasi_threshold(g).is_quasi_threshold) {
               ++count;
           }
       }

       std::cout << count << '\n';
       return 0;
   }


参考文献
--------------

* E. S. Wolk. "The comparability graph of a tree."
  *Proceedings of the American Mathematical Society*, 13(5):789--795, 1962.
  `DOI:10.1090/S0002-9939-1962-0172273-0 <https://doi.org/10.1090/S0002-9939-1962-0172273-0>`_

* J.-H. Yan, J.-J. Chen, G. J. Chang. "Quasi-threshold graphs."
  *Discrete Applied Mathematics*, 69(3):247--255, 1996.
  `DOI:10.1016/0166-218X(96)00094-7 <https://doi.org/10.1016/0166-218X(96)00094-7>`_
