強弦グラフ
====================

グラフが **強弦グラフ** (strongly chordal graph) であるとは、弦グラフであり、
かつ長さ 6 以上のすべての偶サイクルが奇弦 (サイクルに沿って奇数距離にある
2 頂点を結ぶ弦) を持つことをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_125.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``StronglyChordalAlgorithm``
     - 説明
   * - ``STRONG_ELIMINATION``
     - 強消去順序 (strong elimination ordering) の検査、O(n^4)
   * - ``PEO_MATRIX``
     - 全走査による simple vertex 消去 (名前は歴史的経緯によるもので、
       行列は構築しない)、最悪 O(n m Delta)
   * - ``MCS_SEO`` **(既定)**
     - 次数順ソートした包含判定を用いる simple vertex 消去、
       最悪 O(n m Delta)

.. doxygenenum:: graph_recognition::StronglyChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_chordal_elimination
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal_peo_matrix
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal_mcs_seo
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::StronglyChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_chordal_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "strongly_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_strongly_chordal(g);

       std::cout << std::boolalpha << result.is_strongly_chordal << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "strongly_chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* M. Farber. "Characterizations of strongly chordal graphs."
  *Discrete Mathematics*, 43(2--3):173--189, 1983.
  `DOI:10.1016/0012-365X(83)90154-1 <https://doi.org/10.1016/0012-365X(83)90154-1>`_

* E. Dahlhaus, P. Duchet. "On strongly chordal graphs."
  *Ars Combinatoria*, 24B:23--30, 1987.
