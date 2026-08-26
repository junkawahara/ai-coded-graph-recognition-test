オイラーグラフ
============================

グラフがオイラーグラフであるかどうかを判定する。
オイラーグラフとはすべての頂点の次数が偶数であるグラフ (連結性は要求しない。
連結なオイラーグラフはオイラー閉路を持つ)。

認識
----------

.. doxygenenum:: graph_recognition::EulerianAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EulerianResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_eulerian
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::EulerianLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EulerianLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_eulerian_labeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6`` に対して、オイラーグラフ列挙器が返す個数が
`OEIS A058878 <https://oeis.org/A058878>`_ の行和 (添字を 1 ずらした
`OEIS A006125 <https://oeis.org/A006125>`_ と同値) の
``1, 1, 2, 8, 64, 1024`` と一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "eulerian.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_eulerian(g);

       std::cout << std::boolalpha << result.is_eulerian << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "eulerian_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_eulerian_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* L. Euler. "Solutio problematis ad geometriam situs pertinentis."
  *Commentarii Academiae Scientiarum Petropolitanae*, 8:128--140, 1741
  (presented 1736).
