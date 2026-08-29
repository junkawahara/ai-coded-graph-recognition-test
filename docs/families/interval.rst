インターバルグラフ
========================

グラフがインターバルグラフかどうかを判定する。
各頂点は実数直線上の区間に対応し、区間が重なる頂点間に辺を持つ。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_234.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``IntervalAlgorithm``
     - 説明
   * - ``BACKTRACKING``
     - 弦グラフ性を確認した後、クリーク木上でクリークパスをバックトラッキングで
       探索する (各頂点のクリークは連結な部分木をなす必要がある)。
   * - ``AT_FREE`` **(既定)**
     - Lekkerkerker--Boland の定理に基づき、グラフが弦グラフかつ
       AT-free (asteroidal triple を持たない) であることを検証する。
   * - ``PQ_TREE``
     - Fulkerson--Gross の定理に基づき、弦グラフ性を確認した後、
       頂点/極大クリーク接続行列の連続 1 性 (C1P) を Booth--Lueker の
       PQ-tree で判定する (:doc:`../api/utilities` の ``pq_tree.h``)。

.. doxygenenum:: graph_recognition::IntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_interval
   :project: graph_recognition


列挙
------------

既定の ``KIYOMI_KIJIMA_UNO`` はインターバルグラフ専用の辺削除逆探索である。
``K_n`` を根とし、最大ラベルの非全域頂点と、区間モデル上で最も近い
非隣接頂点を結ぶ辺を追加する操作を一意な親として定める。子生成では辺を
1 本削除し、その親が現在のグラフに戻る場合だけ再帰する。最大ラベルの
非全域頂点より大きい頂点はすべて true twin な全域頂点なので、同型な辺削除の
認識結果を共有する。

``LEGACY_CHORDAL_FILTER`` を指定すると、従来の弦グラフ頂点追加木と
インターバル認識フィルタを用いる。``REVERSE_SEARCH`` は後方互換のための
``KIYOMI_KIJIMA_UNO`` の別名である。

原論文は線形時間の区間表現構築を用いて 1 出力あたり ``O(n^3)`` 時間、
``O(n^2)`` 空間を示す。本実装は既存の ``check_interval`` を子候補の認識に
再利用し、callback ごとに完全な辺リストを構築するため、この時間境界は
そのままでは適用されない。

.. doxygenenum:: graph_recognition::IntervalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_interval_labeled_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_interval_labeled_graphs_reverse_search_cb
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは順列グラフ・サークルグラフ・
弦グラフの列挙器と同じ McKay の canonical construction path 法です。
インターバルグラフは遺伝的なので、頂点を 1 つずつ追加してグラフを成長させて
よいことが保証されます。枝刈りは候補となる子グラフごとの ``check_interval``
呼び出し (弦グラフ性 + asteroidal triple 探索 + クリークパス構築) で、
これを同型除去より先に行うので、より高価な正準化はインターバルグラフに
対してしか実行されません。Yamazaki らの専用非同型列挙 (MPQ 木の正準形に
よる ``O(n^4)`` 遅延) と Mikos の改良 (``O(n^3 log n)`` 遅延) は正準化を
まったく使いませんが、本実装は共有の canonical-augmentation 機構を再利用
する方式で、``n = 9`` 程度まで実用的です。個数は OEIS A005975(n)
(1, 2, 4, 10, 27, 92, 369, 1807, 10344, ...)、``connected_only`` を指定した
場合はそのうち連結なもの (A005976: 1, 1, 2, 5, 15, 56, 250, 1328,
8069, ...) です。

.. doxygenenum:: graph_recognition::IntervalUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::IntervalUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_interval_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
----------------------------

``n = 1, 2, 3, 4, 5, 6`` について、列挙されたラベル付きインターバルグラフの個数が
`OEIS A005215 <https://oeis.org/A005215>`_ の ``1, 2, 8, 61, 822, 17914`` と一致することを検証した。

非同型列挙については ``n = 9`` まで
`OEIS A005975 <https://oeis.org/A005975>`_ の
``1, 2, 4, 10, 27, 92, 369, 1807, 10344`` と一致することを、列挙器自身とは
独立に検証した (非同型弦グラフ列挙の出力を ``check_interval`` で絞り込んでも
同じ個数が得られる)。``connected_only`` を指定した場合の個数は
`OEIS A005976 <https://oeis.org/A005976>`_ の
``1, 1, 2, 5, 15, 56, 250, 1328`` と一致する。静的テストケースは
``n = 8`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_interval(g);

       std::cout << std::boolalpha << result.is_interval << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "interval_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_interval_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "interval_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_interval_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 92 = A005975(6)
       return 0;
   }


参考文献
--------------

* M. Kiyomi, S. Kijima, T. Uno. "Listing Chordal Graphs and Interval Graphs."
  *Graph-Theoretic Concepts in Computer Science (WG 2006)*,
  LNCS 4271:68--77, 2006.
  `DOI:10.1007/11917496_7 <https://doi.org/10.1007/11917496_7>`_

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_

* D. Corneil, S. Olariu, L. Stewart. "The LBFS structure and recognition of interval graphs."
  *SIAM Journal on Discrete Mathematics*, 23(4):1905--1953, 2009.
  `DOI:10.1137/S0895480100373455 <https://doi.org/10.1137/S0895480100373455>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* K. Yamazaki, T. Saitoh, M. Kiyomi, R. Uehara. "Enumeration of nonisomorphic interval graphs and nonisomorphic permutation graphs."
  *Theoretical Computer Science*, 806:310--322, 2020.
  `DOI:10.1016/j.tcs.2019.04.017 <https://doi.org/10.1016/j.tcs.2019.04.017>`_

* P. Mikos. "Efficient enumeration of non-isomorphic interval graphs."
  *Discrete Mathematics & Theoretical Computer Science*, 23(1), 2021.
  `DOI:10.46298/dmtcs.6164 <https://doi.org/10.46298/dmtcs.6164>`_
