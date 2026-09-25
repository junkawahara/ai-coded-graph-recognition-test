フラーレングラフ
========================

**フラーレングラフ** は、すべての面が五角形または六角形である
3-連結 3-正則平面グラフである。オイラーの公式により、フラーレングラフは
常にちょうど 12 個の五角形面を持つ。

認識
----------

.. doxygenenum:: graph_recognition::FullereneAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::FullereneResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_fullerene
   :project: graph_recognition


列挙
----------

既定の ``BUCKYGEN`` は、Brinkmann--Goedgebeur--McKay (2012) の
L/B パッチ拡張を双対三角形分割に適用する。C20・C28(Td) に加え、
別途構成する (5,0) nanotube（C30, C40, ...）を種とする。
正準な逆縮約と兄弟の正準形集合で鏡映を含む同型重複を除き、
双対化して固定頂点数の結果を返す。外部 buckygen や平面性認識は使わない。

``REVERSE_SEARCH`` は従来の次数制約付き双対候補列挙と出力順を維持する。
CLI は ``--algo buckygen``（既定）/ ``--algo reverse_search`` で選択する。
両方式とも20頂点未満・奇数・22頂点は空集合を返し、
入力範囲 ``0 <= n <= max_vertex_count`` を維持する。
Python の列挙 API は提供していない。

パッチと回転系をコピーし、探索待ちの子・兄弟集合・全出力を保持する。
正準縮約順序は本実装固有であり、buckygen のインプレース更新・軌道剪定・
先読みや速度・遅延保証は再現していない。

.. doxygenenum:: graph_recognition::FullereneUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::FullereneUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::FullereneUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_fullerene_unlabeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/fullerene.h"

   int main() {
       using namespace graph_recognition;

       // Dodecahedron (C_20 fullerene: 20 vertices, 30 edges)
       Graph g(20, {
           {1, 2}, {1, 5}, {1, 6},
           {2, 3}, {2, 7},
           {3, 4}, {3, 8},
           {4, 5}, {4, 9},
           {5, 10},
           {6, 11}, {6, 15},
           {7, 11}, {7, 12},
           {8, 12}, {8, 13},
           {9, 13}, {9, 14},
           {10, 14}, {10, 15},
           {11, 16},
           {12, 17},
           {13, 18},
           {14, 19},
           {15, 20},
           {16, 17}, {16, 20},
           {17, 18},
           {18, 19},
           {19, 20}
       });
       auto result = check_fullerene(g);

       std::cout << std::boolalpha << result.is_fullerene << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/fullerene_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_fullerene_unlabeled_graphs(20);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "The generation of fullerenes."
  *Journal of Chemical Information and Modeling*, 52(11):2910--2918, 2012.
  `DOI:10.1021/ci3003107 <https://doi.org/10.1021/ci3003107>`_
