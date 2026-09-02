コグラフ
====================

誘導部分グラフとして P4 (4 頂点のパス) を含まないグラフを **コグラフ** という。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_151.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CographAlgorithm``
     - 説明
   * - ``COTREE``
     - 連結成分 / co-component (補グラフの連結成分) への再帰的分解による
       コツリー構築。
   * - ``PARTITION_REFINEMENT`` **(既定)**
     - 同じ分解を、連結リストによる高速な co-component 探索で行う。
       計算量: 最悪 O(n (n + m)) (分解の各レベルで隣接リストを
       再走査する)。

   * - ``MODULAR``
     - modular decomposition 木を構築し PRIME ノードを持たないことを確認する
       (:doc:`../api/utilities` の ``modular_decomposition.h``)。cograph の
       modular decomposition はその cotree そのものなので、まったく別の機構で
       同じ構造に到達する独立確認になる。O(n^4) と低速。

.. doxygenenum:: graph_recognition::CographAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cograph
   :project: graph_recognition

cotree の構築
--------------

認識アルゴリズムが行う分解そのものが cotree である。``build_cotree()`` は
それを破棄せずに :doc:`../api/utilities` の ``MDTree`` として返す
(union ステップが PARALLEL ノード、join ステップが SERIES ノード、
PRIME ノードは決して現れない)。``check_cograph()`` は木を構築しないため、
認識のコストは従来どおり。

.. doxygenstruct:: graph_recognition::CotreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::build_cotree
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::CographLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cograph_labeled_graphs_cotree
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は、cotree の
再帰的構成により同型類ごとに代表元を 1 つだけ出力します。n >= 2 では補グラフを
とる操作が cotree の根の種別 (union/join) を入れ替えるため、連結なコグラフは
非連結なコグラフの補グラフに他なりません。非連結なコグラフはより小さい連結
コグラフを整数分割上で合成して得られるので、同型判定なしで各同型類がちょうど
1 回ずつ現れます。個数は OEIS A000084(n) (1, 2, 4, 10, 24, 66, 180, 522, ...)、
``connected_only`` を指定した場合は A000669(n) (1, 1, 2, 5, 12, 33, 90,
261, ...) です。

.. doxygenenum:: graph_recognition::CographUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CographUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cograph_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` に対して、列挙されたラベル付きコグラフの個数が
`OEIS A006351 <https://oeis.org/A006351>`_ に記録された整数列
``2, 8, 52, 472, 5504`` と一致することを検証した。非同型列挙については
``n = 10`` まで `OEIS A000084 <https://oeis.org/A000084>`_ の値
(``1, 2, 4, 10, 24, 66, 180, 522, 1532, 4624``) と、``connected_only``
指定時は `OEIS A000669 <https://oeis.org/A000669>`_ の値 (``1, 1, 2, 5,
12, 33, 90, 261, 766, 2312``) と一致することを検証済み。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/cograph.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cograph(g);

       std::cout << std::boolalpha << result.is_cograph << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/cograph_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cograph_labeled_graphs_cotree(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* D. Seinsche. "On a property of the class of n-colorable graphs."
  *Journal of Combinatorial Theory, Series B*, 16(2):191--193, 1974.
  `DOI:10.1016/0095-8956(74)90063-X <https://doi.org/10.1016/0095-8956(74)90063-X>`_

* D. G. Corneil, H. Lerchs, L. Stewart Burlingham. "Complement reducible graphs."
  *Discrete Applied Mathematics*, 3(3):163--174, 1981.
  `DOI:10.1016/0166-218X(81)90013-5 <https://doi.org/10.1016/0166-218X(81)90013-5>`_

* D. G. Corneil, Y. Perl, L. K. Stewart. "A linear recognition algorithm for cographs."
  *SIAM Journal on Computing*, 14(4):926--934, 1985.
  `DOI:10.1137/0214065 <https://doi.org/10.1137/0214065>`_

* Á. A. Jones, F. Protti, R. R. Del-Vecchio. "Cograph generation with linear delay."
  *Theoretical Computer Science*, 713:1--10, 2018.
  `DOI:10.1016/j.tcs.2017.12.037 <https://doi.org/10.1016/j.tcs.2017.12.037>`_
