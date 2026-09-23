プロパーインターバルグラフ
================================

グラフがプロパーインターバルグラフかどうかを判定する。
真の包含関係を持たない区間の族で表現できるインターバルグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_298.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ProperIntervalAlgorithm``
     - 説明
   * - ``TRIPLE_LOOP_CLAW_CHECK``
     - インターバルグラフ判定 + 三重ループによる claw (K_{1,3}) 検出。計算量: O(n^3 + n * Delta^3)
       (先行するインターバル判定 O(n^3) を含む)。
   * - ``FAST_CLAW_CHECK`` **(既定)**
     - インターバルグラフ判定 + 辺カウントによる claw 検出。
       N(c) の辺数が d(d-1)/2 未満の場合のみ詳細な探索を行う。
       claw 検出のフィルタは O(m * Delta) だが詳細探索を含む最悪は O(n * Delta^3) で、
       インターバル判定の O(n^3) と合わせて全体では O(n^3 + n * Delta^3)。

.. doxygenenum:: graph_recognition::ProperIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalResult
   :project: graph_recognition
   :members:

``ProperIntervalResult`` は indifference ordering (各閉近傍が連続する頂点順序)
も返す。区間認識が構築する区間モデルから (左端, 右端) でソートするだけで
得られる。一般の区間グラフではこの方法は成り立たないが、claw-free な区間
グラフではどのクリーク区間も他の内側に両側から真に含まれることがない
(そうなると、内側の区間より前で終わる頂点と後で始まる頂点が取れる。この
2 頂点はどちらも外側頂点に隣接するが、互いにも内側頂点にも非隣接なので、
外側頂点を中心、この 2 頂点と内側頂点を葉とする claw ができる) ため成立する。
返す前に順序を直接検証している。

.. doxygenfunction:: graph_recognition::check_proper_interval
   :project: graph_recognition


列挙
------------

ラベル付き列挙の既定方式は ``STRING_REPRESENTATION`` です。下記のラベルなし版の
括弧列生成と非連結成分の合成を再利用し、各代表元からすべての異なるラベル付けを
生成します。隣接するラベルの交換は全順列を生成でき、訪問済みのソート済み辺集合を
記録することで自己同型による重複を除きます。反転、twin の交換、同じ連結成分の交換も
処理され、代表元 G ごとに ``n! / |Aut(G)|`` 個のラベル付きグラフを出力します。

代表元の異なるラベル付けが L 個、辺数が m、n >= 1 の場合、展開は L(n-1) 回の
交換候補を調べ、各回に m 本の辺のソートと最大 L 要素の集合への照合を行います。
集合は追加で O(L(m+1)) の領域を使い、代表元ごとに解放します。
ラベルなし代表元とラベル付き結果は全件保持します。括弧列生成は Dyck 語 DFS であり、
原論文の最適化された逆探索ではなく、ラベルなし表現あたり O(1) の時間も保証しません。

``REVERSE_SEARCH`` は、従来の弦グラフ探索木 + proper interval 認識方式とその列挙順序を
維持します。両方式は同じラベル付きグラフ集合を出力しますが、列挙順序は異なり得ます。
n = 0 では両方式とも空グラフを 1 個返します。C++ の従来の関数名は変更していません。
CLI は ``--algo string_representation`` (既定) または ``--algo reverse_search`` を
受け付けます。Python の ``enumerate_proper_interval_labeled_graphs`` は
``algorithm="string_representation"`` または ``algorithm="reverse_search"`` で選択でき、
対応範囲は従来どおり 1 <= n <= 6 です。

.. doxygenenum:: graph_recognition::ProperIntervalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_interval_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は Saitoh らの
括弧文字列表現を用いて、同型類ごとに代表元を 1 つだけ出力します。n 頂点の連結
単位区間グラフは、n 個の ``[`` と n 個の ``]`` からなり、すべての真の非空接頭辞で
``[`` が ``]`` より真に多い文字列に対応します。この表現は反転を除いて一意なので、
反転かつ ``[`` と ``]`` を交換した文字列を超えないものだけを残せば、各同型類が
ちょうど 1 回ずつ現れます。非連結グラフは整数分割上で連結成分を合成して構成します。
個数は OEIS A005217 (1, 2, 4, 9, 21, 55, 151, 447, ...)、``connected_only`` を
指定した場合は A007123 (1, 1, 2, 4, 10, 26, 76, 232, ...) です。

.. doxygenenum:: graph_recognition::ProperIntervalUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperIntervalUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ProperIntervalUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_interval_unlabeled_graphs
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/proper_interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_interval(g);

       std::cout << std::boolalpha << result.is_proper_interval << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/proper_interval_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_interval_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
--------------

* T. Saitoh, K. Yamanaka, M. Kiyomi, R. Uehara.
  "Random Generation and Enumeration of Proper Interval Graphs."
  *IEICE Transactions on Information and Systems*, E93-D(7):1816--1823, 2010.
  `DOI:10.1587/transinf.E93.D.1816 <https://doi.org/10.1587/transinf.E93.D.1816>`_

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
