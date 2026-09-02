極大外平面グラフ
================

**極大外平面グラフ** は、どの辺を追加しても外平面性が失われる外平面グラフである。
``n >= 3`` では凸多角形の三角形分割とちょうど一致する: 2-連結で、唯一の
ハミルトン外周サイクルを持ち、内面はすべて三角形であり、単純 2-木とも一致する。
``n >= 2`` 頂点の極大外平面グラフの辺数はちょうど ``2n - 3`` である。

認識
----------

認識は外平面性判定と辺数条件の組み合わせである: ``n >= 2`` 頂点の単純外平面
グラフの辺数は高々 ``2n - 3`` なので、この上界に達した外平面グラフにはもう
辺を追加できず、逆にすべての極大外平面グラフはこの上界を達成する。

.. doxygenenum:: graph_recognition::MaximalOuterPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalOuterPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_maximal_outer_planar
   :project: graph_recognition


列挙
----------

列挙器は同型類ごとに代表元を 1 つだけ出力します。汎用の canonical
augmentation 経路ではなく、専用の多角形三角形分割構成を用います。
``n >= 3`` 頂点の極大外平面グラフは凸 ``n`` 角形の三角形分割であり、
2-連結外平面グラフのハミルトンサイクルは*一意*なので、そのようなグラフの間の
同型写像は必ず外周サイクルを外周サイクルに写し、多角形の対称変換になります。
したがって同型類は ``Catalan(n-2)`` 個の三角形分割の二面体群 ``D_n`` に
よる軌道と 1 対 1 に対応し、列挙器は弦ごとの頂点 (apex) 選択による標準的な
再帰で三角形分割を走査し、対角線集合が自身の ``2n`` 個の二面体像の中で
辞書式最小のものだけを出力します — 認識器の呼び出しも汎用グラフの正準化も
一切不要です。個数は ``n = 0, 1, 2`` で 1 (K0, K1, K2)、以降は
OEIS A000207 (n = 3, 4, ... に対して 1, 1, 1, 3, 4, 12, 27, 82, 228,
733, ...) です。実用範囲はおよそ ``n = 16`` まで (2,674,440 個の三角形分割、
83,898 類、約 2 秒)。極大外平面グラフはすべて連結なので、
``connected_only`` フラグはありません。

.. doxygenenum:: graph_recognition::MaximalOuterPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalOuterPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::MaximalOuterPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_outer_planar_unlabeled_graphs
   :project: graph_recognition

個数チェック
------------

非同型列挙は ``n = 16`` まで OEIS A000207 を再現します (n = 3..16 に対して
1, 1, 1, 3, 4, 12, 27, 82, 228, 733, 2282, 7528, 24834, 83898)。また
``n <= 6`` では、``K_n`` の辺集合のうちサイズ ``2n - 3`` で認識器が受理する
すべての部分集合を正準化した結果と同型類集合が完全に一致します (このクラスには
照合対象となるラベル付き列挙器がありません)。静的テストケースは ``n = 12``
までです。


例
--------

認識の例
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/maximal_outer_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {1, 4}, {1, 3}});
       auto result = check_maximal_outer_planar(g);

       std::cout << std::boolalpha << result.is_maximal_outer_planar << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/maximal_outer_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_outer_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 12
       return 0;
   }


参考文献
----------

* M. Bodirsky, É. Fusy, M. Kang, S. Vigerske. "Enumeration and asymptotic
  properties of unlabeled outerplanar graphs."
  *Electronic Journal of Combinatorics*, 14(1):R66, 2007.

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.

* `OEIS A000207 <https://oeis.org/A000207>`_
