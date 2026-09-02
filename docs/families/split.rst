スプリットグラフ
================================

グラフが **スプリットグラフ** であるとは、頂点集合をクリークと独立集合に
分割できることをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_39.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SplitAlgorithm``
     - 説明
   * - ``DEGREE_SEQUENCE``
     - グラフとその補グラフの弦グラフ性検査 (enum 名は歴史的経緯による
       もので、次数列に基づく手法は ``HAMMER_SIMEONE``)、O(n^2)
   * - ``HAMMER_SIMEONE`` **(既定)**
     - Hammer-Simeone の次数列条件。次数列は隣接リストの長さから得られ、
       計数ソートで整列するので O(n)

.. doxygenenum:: graph_recognition::SplitAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitResult
   :project: graph_recognition
   :members:

``SplitResult::side`` は分割そのものを返す (1 = クリーク側 K, 2 = 独立集合側 S)。
Hammer--Simeone (1981) により、次数条件が成り立つとき次数最大の頂点群が
クリーク側になる。返す前に分割を検証しているため、これは証明書として使える。

.. doxygenfunction:: graph_recognition::check_split
   :project: graph_recognition


列挙
------------

既定の ``KS_PARTITION_CANONICAL`` は split graph 固有の構造を直接使う。
頂点集合の分割 ``V = K ∪ S`` を選び、``K`` をクリーク、``S`` を独立集合と
する。各 ``k ∈ K`` の ``S`` 内近傍を空でない集合に限定することで、
``S`` が最大独立集合である S-max 分割だけを生成する。したがって、候補は
すべて split graph であり、認識器によるフィルタは行わない。

同じグラフが複数の S-max 分割を持つ場合、swing vertex はクリーク ``A`` を
形成する。``S`` 側に置かれた swing vertex ``a`` が ``A`` の最小ラベルである
分割だけを受理することで、各ラベル付きグラフをちょうど一度出力する。
``LEGACY_CHORDAL_FILTER`` は、旧来の chordal 頂点追加探索と split 認識による
枝刈りを差分検証用に残したものである。

.. doxygenenum:: graph_recognition::SplitLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_split_labeled_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_split_labeled_graphs_reverse_search_cb
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは弦グラフ・順列グラフ・サークル
グラフの列挙器と同じ McKay の canonical construction path 法です。スプリット
グラフは遺伝的なので、頂点を 1 つずつ追加してグラフを成長させてよいことが
保証されます。枝刈りは候補となる子グラフごとの ``check_split`` 呼び出し
(Hammer--Simeone の次数列条件) で、これを同型除去より先に行うので、より
高価な正準化はスプリットグラフに対してしか実行されません。個数は OEIS
A048194(n) (1, 2, 4, 9, 21, 56, 164, 557, 2223, ...)、``connected_only``
を指定した場合はそのうち連結なもの (A048194 の階差 1, 1, 2, 5, 12, 35,
108, 393, ...; 独立した OEIS 系列は無い) です。非連結なスプリットグラフは
辺を持つ成分を高々 1 つしか持たない (辺を持つ成分が 2 つあれば 2K2 を誘導
する) ため、連結な個数は階差に一致します。

.. doxygenenum:: graph_recognition::SplitUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SplitUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_split_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6, 7, 8`` について、列挙されたラベル付きスプリットグラフの個数が
`OEIS A179534 <https://oeis.org/A179534>`_ の値
``2, 8, 58, 632, 9654, 202484, 5843954`` と一致することを検証済み。

非同型列挙については ``n = 9`` まで
`OEIS A048194 <https://oeis.org/A048194>`_ の
``1, 2, 4, 9, 21, 56, 164, 557, 2223`` と一致することを、列挙器自身とは
独立に検証した (``n`` 頂点の非同型グラフをすべて生成して ``check_split``
で絞り込んでも同じ個数が得られる)。``connected_only`` を指定した場合の個数は
A048194 の階差 ``1, 1, 2, 5, 12, 35, 108, 393`` と一致する。静的テスト
ケースは ``n = 8`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/split.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_split(g);

       std::cout << std::boolalpha << result.is_split << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/split_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_split_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/split_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_split_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 56 = A048194(6)
       return 0;
   }


参考文献
----------------

* P. L. Hammer, B. Simeone. "The splittance of a graph."
  *Combinatorica*, 1(3):275--284, 1981.
  `DOI:10.1007/BF02579333 <https://doi.org/10.1007/BF02579333>`_

* S. Földes, P. L. Hammer. "Split graphs."
  *Congressus Numerantium*, 19:311--315, 1977.

* C. Cheng, K. L. Collins, A. N. Trenk. "Split graphs and
  Nordhaus--Gaddum graphs." *Discrete Mathematics*, 339(9):2345--2356, 2016.
  `DOI:10.1016/j.disc.2016.04.001 <https://doi.org/10.1016/j.disc.2016.04.001>`_

* J. M. Troyka. "Split graphs: combinatorial species and asymptotics."
  *Electronic Journal of Combinatorics*, 26(2):P2.42, 2019.
  `arXiv:1803.07248 <https://arxiv.org/abs/1803.07248>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
