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

.. doxygenstruct:: graph_recognition::TournamentLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TournamentLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tournaments
   :project: graph_recognition

上記の列挙器はラベル付きトーナメントを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction path 法で、
nauty のトーナメント専用生成器 gentourng と同じ方式です。トーナメントは頂点を
1 つずつ追加して成長させ (トーナメントからどの頂点を削除してもトーナメントのままなので
すべての同型類に到達できる)、子グラフは既存の k 頂点に対する ``2^k`` 通りの向き付け
パターンのいずれかで新頂点を追加します。構成の各中間段階が定義上トーナメントなので
認識フィルタは不要です。同型除去は out-隣接ビットマスクに対して共有の無向正準化を
そのまま再利用します。この再利用が正当なのは台グラフが完全グラフだからです:
正準形は各行を先に置かれた位置に対してのみ詰め込みますが、トーナメントでは
ビットが立っていないことが逆向きの弧そのものを意味するため、正準形がすべての
頂点対の向きを決定します。``n = 9`` (191536 類) 程度まで実用的です。
トーナメントはすべて弱連結なので ``connected_only`` フラグはありません。

.. doxygenenum:: graph_recognition::TournamentUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TournamentUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TournamentUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tournament_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6`` について、列挙されたラベル付きトーナメントの個数が
`OEIS A006125 <https://oeis.org/A006125>`_ (コメントで ``a(n) = 2^(n(n-1)/2)``
がラベル付き n チーム総当たり戦の結果の総数であると述べられている) と一致する
ことを検証した: ``1, 2, 8, 64, 1024, 32768``。

非同型列挙は `OEIS A000568 <https://oeis.org/A000568>`_ (ラベルなし
トーナメントの個数) を再現する: 静的テストケースの ``n = 1, ..., 8`` で
``1, 1, 2, 4, 12, 56, 456, 6880``、さらに ``n = 9`` の ``191536`` (約 9 秒)
を手動で確認した。``n <= 6`` ではラベル付き列挙器の出力を正準化した同型類の
集合と完全に一致する。


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
   #include "tournament_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_tournaments(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "tournament_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_tournament_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 12
       return 0;
   }


参考文献
------------

* J. W. Moon. *Topics on Tournaments.*
  Holt, Rinehart and Winston, New York, 1968.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
