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

.. doxygenenum:: graph_recognition::SplitEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_split_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_split_graphs_reverse_search_cb
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6, 7, 8`` について、列挙されたラベル付きスプリットグラフの個数が
`OEIS A179534 <https://oeis.org/A179534>`_ の値
``2, 8, 58, 632, 9654, 202484, 5843954`` と一致することを検証済み。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "split.h"

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
   #include "split_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_split_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
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
