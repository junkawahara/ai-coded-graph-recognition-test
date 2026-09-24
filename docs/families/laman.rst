Laman グラフ
========================

グラフが Laman グラフかどうかを判定する。
Laman 条件、すなわち m = 2n - 3 かつ任意の k 頂点 (k >= 2) 上の部分グラフの
辺数が高々 2k - 3 であることを満たすグラフである (k = 1 では 2k - 3 < 0 と
なるので、部分集合条件は 2 頂点以上の部分集合に対して課す)。実装の
``check_laman`` は 1 頂点グラフ K_1 も、OEIS の慣例と列挙器の出力に合わせて
Laman グラフとして受理する。Laman グラフは平面上の極小剛性
フレームワークを特徴づける。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1206.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::LamanAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_laman
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::LamanLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_labeled_graphs
   :project: graph_recognition

上記はラベル付きグラフ、もう一方は同型類ごとに代表元を 1 つ出力する。
両方の既定方式は ``HENNEBERG`` である。K2 から、任意の異なる2頂点に
新頂点を接続する Henneberg I と、既存辺を細分し、新頂点を第3の既存頂点に
接続する Henneberg II を適用する。全中間グラフが Laman なので、新方式は
認識器や部分集合のスパース性検査を使わない。サイズごとの正準形集合で、
異なる親・操作から生じる同型重複を除き、現在・次の2レベルのみ保持する。
ラベル付き版は各代表元を隣接ラベル交換で展開し、訪問済み辺集合で重複を除く。
最大ラベルの頂点が逆 Henneberg 操作で削除できるとは限らないので、
構成順序をラベル順に固定するだけでは全ラベル付きグラフを生成できない。

従来の部分集合スパース性検査を使う方式は ``REVERSE_SEARCH`` (ラベル付き) /
``CANONICAL_AUGMENTATION`` (ラベルなし) として、出力順と代表元のラベル付けも
維持する。従来方式は (2,3)-スパースな中間グラフを生成し、出力時にタイト性を
課す。ラベルなし版では McKay の正準親判定も使う。新旧方式の出力順と
ラベルなし代表元のラベル付けは異なる場合がある。

両方式とも n = 0 は0件、n = 1 は K1、n = 2 は K2 を返す。
Laman グラフは連結なので ``connected_only`` フラグはない。
C++・CLI の上限は新方式およびラベルなし従来方式が63頂点、ラベル付きの
``REVERSE_SEARCH`` が64頂点。Python の Laman API は認識のみで、列挙は未公開。

CLI は ``--algo henneberg`` (既定)、``--algo reverse_search`` (ラベル付き)、
``--algo canonical_augmentation`` (ラベルなし) で選択する。
k 頂点の親ごとに k(k-1)/2 + (2k-3)(k-2) 個の Henneberg 候補があり、
各候補で既存の最悪階乗時間の正準化を行う。API は全出力を保持し、
ラベル展開時には現在の代表元の訪問済みラベル付き辺集合も保持する。
多項式遅延は保証しない。

.. doxygenenum:: graph_recognition::LamanUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::LamanUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

Henneberg 非同型列挙については、``n = 9`` までの個数が
`OEIS A227117 <https://oeis.org/A227117>`_ (``n = 1, ..., 9`` に対して
``1, 1, 1, 1, 3, 13, 70, 608, 7222``) と一致することを検証した
(従来方式の ``n = 10`` は以前に 110132 と一致することを確認済み)。
新旧方式のラベル付き出力集合は ``n = 6`` まで、ラベルなし同型類集合は
``n = 7`` まで一致する。``n = 6`` の全グラフを pebble game で認識した
結果とも照合した。Henneberg II が必要な K3,3・三角柱グラフと、最大ラベルの
頂点の次数が5のラベル付けも検証する。静的カウントテストは ``n = 9`` まで。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/laman.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}});
       auto result = check_laman(g);

       std::cout << std::boolalpha << result.is_laman << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/laman_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/laman_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 13
       return 0;
   }


参考文献
------------

* Taufiqurrachman, Y. Yamauchi, S. Kijima, M. Yamashita.
  "Enumeration of Henneberg decompositions of Laman graphs and M-Circuits."
  §2.4, §3.1.
  `著者版本文 <https://www.ipsj-kyushu.jp/page/ronbun/hinokuni/1001/B-1/B-1-3.pdf>`_

* G. Laman. "On graphs and rigidity of plane skeletal structures."
  *Journal of Engineering Mathematics*, 4(4):331--340, 1970.
  `DOI:10.1007/BF01534980 <https://doi.org/10.1007/BF01534980>`_

* D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation: the pebble game."
  *Journal of Computational Physics*, 137(2):346--365, 1997.
  `DOI:10.1006/jcph.1997.5809 <https://doi.org/10.1006/jcph.1997.5809>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
