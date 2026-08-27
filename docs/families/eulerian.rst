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

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction
path 法に偶数次数制約を組み合わせたものです。オイラーグラフは遺伝的では
ありません (次数が正の頂点を削除すると、その近傍の次数の偶奇が崩れます) が、
頂点を 1 つずつ追加する探索の中間レベルではすべてのグラフを生成します。
代わりに偶数次数制約は最終レベルを 1 通りに確定させます: オイラーグラフ
``G`` において ``G - v`` の奇数次数頂点はちょうど ``v`` の近傍なので、
``n - 1`` 頂点の任意のグラフはちょうど 1 通りの方法でオイラーグラフに拡張
されます (新しい頂点を奇数次数頂点すべてに接続します。握手補題によりこの
集合の大きさは偶数です)。したがって候補近傍にわたる分岐は最終レベルの手前
までしか行われません。子グラフは、追加した頂点がその子の正準ラベリングで
最後に置かれる頂点の自己同型軌道に属するときに限り採用されます。個数は
OEIS A002854(n) (1, 1, 2, 3, 7, 16, 54, 243, 2038, 33120, ...)、
``connected_only`` を指定した場合はそのうち連結なもの (A003049: 1, 0, 1,
1, 4, 8, 37, 184, ...) です。

.. doxygenenum:: graph_recognition::EulerianUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EulerianUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::EulerianUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_eulerian_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6`` に対して、オイラーグラフ列挙器が返す個数が
`OEIS A058878 <https://oeis.org/A058878>`_ の行和 (添字を 1 ずらした
`OEIS A006125 <https://oeis.org/A006125>`_ と同値) の
``1, 1, 2, 8, 64, 1024`` と一致することを検証した。

非同型列挙については ``n = 10`` まで
`OEIS A002854 <https://oeis.org/A002854>`_ の
``1, 1, 2, 3, 7, 16, 54, 243, 2038, 33120`` と一致することを検証し、
``n = 6`` まではラベル付き列挙器の出力を正準化した集合とも照合した。
``connected_only`` を指定した場合の個数は
`OEIS A003049 <https://oeis.org/A003049>`_ の
``1, 0, 1, 1, 4, 8, 37, 184`` と一致する。静的テストケースは ``n = 8``
までである。


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

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "eulerian_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_eulerian_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 16 = A002854(6)
       return 0;
   }


参考文献
----------------

* L. Euler. "Solutio problematis ad geometriam situs pertinentis."
  *Commentarii Academiae Scientiarum Petropolitanae*, 8:128--140, 1741
  (presented 1736).

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
