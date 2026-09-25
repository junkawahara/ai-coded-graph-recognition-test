単純四角形分割
========================

**単純四角形分割** は、(外面を含む) すべての面が四角形 (長さ 4 のサイクル) である
3-連結平面グラフである。最小のものは立方体グラフ (n = 8) であり、
C4 や K_{2,3} は 2-連結にとどまるため認識されない。

認識
----------

.. doxygenenum:: graph_recognition::SimpleQuadrangulationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SimpleQuadrangulationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_simple_quadrangulation
   :project: graph_recognition


列挙
----------

既定の ``PLANTRI`` は Brinkmann らの Theorem 3 に従い、全サイズの
pseudo-double wheel から P1・P3 拡張を行う。P1 は対象の3頂点が3-cut
でない場合に限り適用し、3連結性を保つ。正準縮約と兄弟の正準形集合で
反転も含む同型重複を除く。外部プログラムは不要。
従来の4正則グラフ列挙・双対化は ``REVERSE_SEARCH``
（CLI: ``--algo reverse_search``、既定は ``--algo plantri``）で選択できる。
両方式とも全出力を保持し、n < 8 と n = 9 の結果は空集合となる。
n = 11 以降の奇数頂点も対象で、入力範囲 ``0..max_vertex_count`` は変わらないが、
実用的に全列挙できる頂点数はこれより大幅に小さい。

.. doxygenenum:: graph_recognition::SimpleQuadrangulationUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SimpleQuadEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SimpleQuadrangulationUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_simple_quadrangulation_unlabeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/simple_quadrangulation.h"

   int main() {
       using namespace graph_recognition;

       // Cube graph Q_3 (8 vertices, 12 edges)
       Graph g(8, {
           {1, 2}, {2, 3}, {3, 4}, {4, 1},
           {5, 6}, {6, 7}, {7, 8}, {8, 5},
           {1, 5}, {2, 6}, {3, 7}, {4, 8}
       });
       auto result = check_simple_quadrangulation(g);

       std::cout << std::boolalpha << result.is_simple_quadrangulation << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/simple_quadrangulation_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       // 最小の simple quadrangulation は立方体グラフ (n = 8) なので
       // 1 が出力される。
       auto result = enumerate_simple_quadrangulation_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* G. Brinkmann, S. Greenberg, C. Greenhill, B. D. McKay, R. Thomas, P. Wollan.
  "Generation of simple quadrangulations of the sphere."
  *Discrete Mathematics*, 305:33--54, 2005. Theorem 3, Figure 3 and Table 3.
  `DOI:10.1016/j.disc.2005.10.005 <https://doi.org/10.1016/j.disc.2005.10.005>`_
