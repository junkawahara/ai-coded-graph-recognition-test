Co-chordal グラフ
================================

補グラフが弦グラフであるグラフを **co-chordal** グラフという。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_145.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoChordalAlgorithm``
     - 説明
   * - ``COMPLEMENT`` **(既定)**
     - 補グラフを構築し、弦グラフ認識を適用する。

.. doxygenenum:: graph_recognition::CoChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_chordal
   :project: graph_recognition


列挙
------------

ラベル付き列挙の既定は ``KIYOMI_UNO`` で、弦グラフ専用の Kiyomi--Uno
列挙器の各出力を、同じ頂点ラベル上の補グラフに変換する。補グラフは全単射なので、
追加の同型排除や co-chordal 認識は不要である。変換はコールバック内で行い、
弦グラフの全出力を別途保持しない。補グラフの辺リスト生成には各出力あたり
``O(n²)`` 時間がかかり、論文の差分出力に対する定数時間保証は適用されない。

``CoChordalLabeledEnumAlgorithm::REVERSE_SEARCH`` を指定すると、従来の
最大ラベル頂点追加と co-chordal 認識による方式を選べる。
両方式とも従来の入力範囲 ``0 <= n <= 64`` を維持し、``n = 0`` は空グラフを
1 件出力する。出力集合は同じだが、列挙順序は異なる。

CLI は ``bin/co_chordal_labeled_enum --algo kiyomi_uno`` （既定）または
``--algo reverse_search``、Python は
``enumerate_co_chordal_labeled_graphs(n, algorithm="kiyomi_uno")`` または
``algorithm="reverse_search"`` で選択する。

.. doxygenenum:: graph_recognition::CoChordalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoChordalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_chordal_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙はラベル付きグラフを出力する。もう一方の列挙は、同型類ごとに
代表元を 1 つ出力する。非同型な弦グラフの列挙 (McKay の canonical
construction path) を行い、各グラフの補グラフを出力する。補グラフ操作は
頂点の付け替えと可換なので同型類上の全単射であり、個数は弦グラフの個数と
一致する。

.. doxygenstruct:: graph_recognition::CoChordalUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CoChordalUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_chordal_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` に対して、列挙されたラベル付き co-chordal グラフの個数が、
弦グラフとの補グラフによる全単射を通じて
`OEIS A058862 <https://oeis.org/A058862>`_ の ``2, 8, 61, 822, 18154`` と
一致することを検証した。

非同型列挙については、``n = 8`` までの個数が
`OEIS A048193 <https://oeis.org/A048193>`_ (弦グラフの個数;
``1, 2, 4, 10, 27, 94, 393, 2119``) と一致することを検証し、``n = 6`` までは
ラベル付き列挙の出力を正準形化して得られる同型類の集合と一致することも
検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/co_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_co_chordal(g);

       std::cout << std::boolalpha << result.is_co_chordal << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/co_chordal_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_chordal_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       auto legacy = enumerate_co_chordal_labeled_graphs_reverse_search(
           4, CoChordalLabeledEnumAlgorithm::REVERSE_SEARCH);
       std::cout << legacy.graphs.size() << '\n';  // 同じ 61 件
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/co_chordal_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_chordal_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 27
       return 0;
   }


参考文献
----------------

* M. Kiyomi, T. Uno. "Generating Chordal Graphs Included in Given Graphs."
  *IEICE Transactions on Information and Systems*, E89-D(2):763--770, 2006.
  `DOI:10.1093/ietisy/e89-d.2.763 <https://doi.org/10.1093/ietisy/e89-d.2.763>`_

* D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs."
  *Pacific Journal of Mathematics*, 15(3):835--855, 1965.
  `DOI:10.2140/pjm.1965.15.835 <https://doi.org/10.2140/pjm.1965.15.835>`_

* R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs."
  *SIAM Journal on Computing*, 13(3):566--579, 1984.
  `DOI:10.1137/0213035 <https://doi.org/10.1137/0213035>`_
