自己補グラフ
====================

**自己補グラフ** は自身の補グラフと同型なグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1059.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SelfComplementaryAlgorithm``
     - 説明
   * - ``ISOMORPHISM_CHECK`` **(既定)**
     - グラフが自身の補グラフと同型かどうかを判定する。

.. doxygenenum:: graph_recognition::SelfComplementaryAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_self_complementary
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::SelfComplementaryEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_self_complementary_graphs
   :project: graph_recognition


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "self_complementary.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_self_complementary(g);

       std::cout << std::boolalpha << result.is_self_complementary << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "self_complementary_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_self_complementary_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* H. Sachs. "Über selbstkomplementäre Graphen."
  *Publicationes Mathematicae Debrecen*, 9:270--288, 1962.

* G. Ringel. "Selbstkomplementäre Graphen."
  *Archiv der Mathematik*, 14(1):354--358, 1963.
  `DOI:10.1007/BF01234967 <https://doi.org/10.1007/BF01234967>`_
