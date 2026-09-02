単位インターバルグラフ
============================

グラフが単位インターバルグラフかどうかを判定する。
すべての区間が同じ長さを持つインターバルグラフである。
Roberts の定理により、プロパーインターバルグラフと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_299.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``UnitIntervalAlgorithm``
     - 説明
   * - ``PROPER_INTERVAL`` **(既定)**
     - プロパーインターバルグラフの認識に帰着する (Roberts の定理)。

.. doxygenenum:: graph_recognition::UnitIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::UnitIntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_unit_interval
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/unit_interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_unit_interval(g);

       std::cout << std::boolalpha << result.is_unit_interval << '\n';
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
   #include "recognizers/unit_interval.h"

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
           if (check_unit_interval(g).is_unit_interval) {
               ++count;
           }
       }

       std::cout << count << '\n';
       return 0;
   }


参考文献
--------------

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.
