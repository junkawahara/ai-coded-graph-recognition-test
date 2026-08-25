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

.. doxygenenum:: graph_recognition::CographEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cograph_graphs_cotree
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` に対して、列挙されたラベル付きコグラフの個数が
`OEIS A006351 <https://oeis.org/A006351>`_ に記録された整数列
``2, 8, 52, 472, 5504`` と一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cograph.h"

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
   #include "cograph_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cograph_graphs_cotree(4);
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
