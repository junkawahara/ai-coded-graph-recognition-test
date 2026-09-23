余インターバルグラフ
========================================

グラフが co-interval グラフかどうかを判定する。
補グラフがインターバルグラフであるグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_157.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoIntervalAlgorithm``
     - 説明
   * - ``COMPLEMENT`` **(既定)**
     - 補グラフを構築し、インターバルグラフの認識を適用する。

.. doxygenenum:: graph_recognition::CoIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoIntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_interval
   :project: graph_recognition


列挙
------------

既定の ``KIYOMI_KIJIMA_UNO`` は、Kiyomi--Kijima--Uno のインターバルグラフ
専用列挙器が出力する各ラベル付きグラフを、同じ頂点ラベル上の補グラフに
変換する。補グラフは全単射なので、追加の認識判定や重複排除は不要。
変換は interval 列挙器のコールバック内で行い、co-interval の結果だけを保持する。
補グラフの構成に 1 件あたり ``O(n²)`` の時間が加わる。原論文の出力時間の
保証がそのまま本実装に適用されるわけではない。

``CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH`` を指定すると、従来の
最大ラベルの頂点追加と co-interval 認識による方式を選択できる。
両方式とも既存の入力範囲 ``0 <= n <= 64`` を維持し、``n = 0`` では空グラフを
1 個返す。出力集合は同じだが、列挙順序は異なる。

CLI は ``bin/co_interval_labeled_enum --algo kiyomi_kijima_uno`` （既定）または
``--algo reverse_search`` を受け付ける。Python では
``enumerate_co_interval_labeled_graphs(n, algorithm="kiyomi_kijima_uno")`` または
``algorithm="reverse_search"`` で選択する。

.. doxygenenum:: graph_recognition::CoIntervalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoIntervalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_interval_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙はラベル付きグラフを出力する。もう一方の列挙は、同型類ごとに
代表元を 1 つ出力する。非同型なインターバルグラフの列挙 (McKay の
canonical construction path) を行い、各グラフの補グラフを出力する。
補グラフ操作は頂点の付け替えと可換なので同型類上の全単射であり、個数は
インターバルグラフの個数と一致する。

.. doxygenstruct:: graph_recognition::CoIntervalUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CoIntervalUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_interval_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
----------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付き co-interval グラフの個数が、
インターバルグラフとの補グラフによる全単射を通じて
`OEIS A005215 <https://oeis.org/A005215>`_ の ``1, 2, 8, 61, 822`` と一致することを検証した。

非同型列挙については、``n = 8`` までの個数が
`OEIS A005975 <https://oeis.org/A005975>`_ (インターバルグラフの個数;
``1, 2, 4, 10, 27, 92, 369, 1807``) と一致することを検証し、``n = 6`` までは
ラベル付き列挙の出力を正準形化して得られる同型類の集合と一致することも
検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/co_interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_co_interval(g);

       std::cout << std::boolalpha << result.is_co_interval << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/co_interval_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_interval_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       auto legacy = enumerate_co_interval_labeled_graphs_reverse_search(
           4, CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH);
       std::cout << legacy.graphs.size() << '\n';  // こちらも 61 個
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/co_interval_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_interval_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 27
       return 0;
   }


参考文献
--------------

* M. Kiyomi, S. Kijima, T. Uno. "Listing Chordal Graphs and Interval Graphs."
  *WG 2006*, LNCS 4271, 68--77, 2006.
  `DOI:10.1007/11917496_7 <https://doi.org/10.1007/11917496_7>`_
* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_
