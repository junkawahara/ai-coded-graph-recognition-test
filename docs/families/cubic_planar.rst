3-正則平面グラフ
========================

**3-正則平面グラフ** は、すべての頂点の次数がちょうど 3 である平面グラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1102.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::CubicPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic_planar
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::CubicPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_planar_labeled_graphs
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは 3-正則グラフ列挙器の次数制約付き探索に
平面性の枝刈りを加えた McKay の canonical construction path 法です。どちらの制約も
頂点削除に関して遺伝的 (最大次数 3 以下、および平面性) なので、探索の中間段階は
最大次数 3 以下の平面グラフの範囲を動き、レベル n に到達したグラフは構成により
3-正則平面グラフです。候補となる子グラフごとに 3-正則への完成可能性条件と
``check_planar`` 呼び出し (最大次数 3 のグラフが常に平面的である 6 頂点未満では
省略) で枝刈りします。このクラスの専用生成器は plantri (Brinkmann, McKay:
平面三角分割を生成して双対を取る、または埋め込まれた 3-正則平面グラフを直接生成)
ですが、本実装は共有の canonical-augmentation 機構を再利用する方式で、``n = 14``
程度まで実用的です。``connected_only`` を指定したときの個数は OEIS A005964
(n = 4, 6, 8, ... で 1, 1, 3, 9, 32, 133, ...) です。非連結を許した総数
(1, 1, 4, 10, 37, 146, ...) は OEIS にありません。奇数の n と n < 4 では
3-正則グラフが存在しないため何も出力されません。

.. doxygenenum:: graph_recognition::CubicPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CubicPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_planar_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

非同型列挙は ``connected_only`` 指定時に ``n = 14`` まで
`OEIS A005964 <https://oeis.org/A005964>`_ (連結 3-正則平面グラフ) を再現する
(A005964 の n = 14 の値は 133、約 94 秒)。また ``n <= 10`` では個数が一般の
3-正則グラフ列挙器の代表元のうち平面的なものの個数と一致し、``n <= 6`` では
ラベル付き逆探索列挙器の出力を正準化した同型類の集合と完全に一致する。
静的テストケースは ``n = 12`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cubic_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cubic_planar(g);

       std::cout << std::boolalpha << result.is_cubic_planar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_planar_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 4
       return 0;
   }


参考文献
------------

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
