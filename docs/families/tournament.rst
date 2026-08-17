トーナメント
========================

有向グラフがトーナメントかどうかを判定する。
完全有向グラフ、すなわち任意の 2 頂点の組に対してちょうど一方向の有向辺が存在するグラフである。

認識
----------

.. doxygenenum:: graph_recognition::TournamentAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TournamentResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_tournament
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::read_directed
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::TournamentEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TournamentEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tournaments
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6`` について、列挙されたラベル付きトーナメントの個数が
`OEIS A006125 <https://oeis.org/A006125>`_ (コメントで ``a(n) = 2^(n(n-1)/2)``
がラベル付き n チーム総当たり戦の結果の総数であると述べられている) と一致する
ことを検証した: ``1, 2, 8, 64, 1024, 32768``。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "tournament.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {1, 3}, {2, 3}};
       auto result = check_tournament(3, arcs);

       std::cout << std::boolalpha << result.is_tournament << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "tournament_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_tournaments(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* J. W. Moon. *Topics on Tournaments.*
  Holt, Rinehart and Winston, New York, 1968.
