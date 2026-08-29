外平面グラフ
========================

全ての頂点が外面上に位置するように平面に埋め込めるグラフを
**外平面グラフ** という。これは K\ :sub:`4` マイナーも
K\ :sub:`2,3` マイナーも含まないことと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_110.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``OuterPlanarAlgorithm``
     - 説明
   * - ``AUGMENTED_PLANARITY`` **(既定)**
     - 拡張グラフ G + K\ :sub:`1` (他の全頂点に隣接する新頂点を追加したグラフ) の平面性を検査する。G が外平面的であることと G + K\ :sub:`1` が平面的であることは同値。
   * - ``MINOR_CHECK``
     - 辺数上界 (2n - 3) の事前検査の後、K\ :sub:`4` と K\ :sub:`2,3` のマイナーをバックトラッキングで探索する。

.. doxygenenum:: graph_recognition::OuterPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OuterPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_outer_planar
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::OuterPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OuterPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_outer_planar_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは順列グラフ・サークルグラフ・
弦グラフ・インターバルグラフの列挙器と同じ McKay の canonical construction
path 法です。外平面グラフは遺伝的なので、頂点を 1 つずつ追加してグラフを
成長させてよいことが保証されます。枝刈りは候補となる子グラフごとの
``check_outer_planar`` 呼び出し (G + K\ :sub:`1` の線形時間平面性判定) で、
これを同型除去より先に行うので、より高価な正準化は外平面グラフに対してしか
実行されません。Wang と Nagamochi の専用列挙器は根付き連結外平面グラフを
1 グラフあたり定数時間で正準化なしに生成しますが、本実装は共有の
canonical-augmentation 機構を再利用する方式で、``n = 10`` 程度まで実用的
です。個数は OEIS A111564(n) (1, 2, 4, 10, 25, 80, 277, 1150, 5291, ...)、
``connected_only`` を指定した場合はそのうち連結なもの
(A111563: 1, 1, 2, 5, 13, 46, 172, 777, 3783, ...) です。

.. doxygenenum:: graph_recognition::OuterPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OuterPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::OuterPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_outer_planar_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付き外平面グラフの個数が
`OEIS A098000 <https://oeis.org/A098000>`_ と一致することを検証した:
``1, 2, 8, 63, 893``。

非同型列挙については ``n = 9`` まで
`OEIS A111564 <https://oeis.org/A111564>`_ の
``1, 2, 4, 10, 25, 80, 277, 1150, 5291`` と一致することを、列挙器自身とは
独立に検証した (非同型平面グラフ列挙の出力を ``check_outer_planar`` で
絞り込んでも同じ個数が得られる)。``connected_only`` を指定した場合の個数は
`OEIS A111563 <https://oeis.org/A111563>`_ の
``1, 1, 2, 5, 13, 46, 172, 777`` と一致する。静的テストケースは
``n = 8`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "outer_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_outer_planar(g);

       std::cout << std::boolalpha << result.is_outer_planar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "outer_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_outer_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "outer_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_outer_planar_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 80 = A111564(6)
       return 0;
   }


参考文献
------------

* G. Chartrand, F. Harary. "Planar permutation graphs."
  *Annales de l'Institut Henri Poincaré B*, 3(4):433--438, 1967.

* S. L. Mitchell. "Linear algorithms to recognize outerplanar and maximal outerplanar graphs."
  *Information Processing Letters*, 9(5):229--232, 1979.
  `DOI:10.1016/0020-0190(79)90075-9 <https://doi.org/10.1016/0020-0190(79)90075-9>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* J. Wang, H. Nagamochi. "Constant time generation of rooted and colored outerplanar graphs."
  *Algorithmic Aspects in Information and Management (AAIM 2010)*, LNCS 6124, 300--309, 2010.
  `DOI:10.1007/978-3-642-14355-7_31 <https://doi.org/10.1007/978-3-642-14355-7_31>`_
