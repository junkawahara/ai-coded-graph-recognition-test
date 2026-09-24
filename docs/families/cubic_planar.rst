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
代表元を 1 つだけ出力します。両者の既定は ``PLANTRI`` です。
連結成分の頂点数 n に対して n/2+2 頂点の単純三角形分割を生成し、
Brinkmann–McKay (2007) §2・Figure 5 の辺反転で多重辺、次にループを導入します。
有向半辺の双対と面巡回を保持し、最小次数3の三角形分割のうち双対が単純なものを
出力します。鏡映を含む埋め込みの重複と、異なる埋め込みが同じ抽象グラフを表す
重複をそれぞれ除きます。橋や2頂点切断を持つ連結グラフも生成し、成分の多重集合
で非連結グラフを合成します。ラベル付き版は全ての異なるラベル付けに展開します。

従来の頂点追加・認識方式は ``REVERSE_SEARCH`` (ラベル付き)、
``CANONICAL_AUGMENTATION`` (ラベルなし) として出力順・代表元も維持します。
CLI は ``--algo plantri`` / ``reverse_search`` / ``canonical_augmentation``、
Python の既存ラベルなし API は ``algorithm="plantri"`` /
``"canonical_augmentation"`` で選択できます。ラベルなし版の
``connected_only`` / ``--connected`` は両方式に対応します。
新方式の C++・CLI 上限は63頂点、旧方式はラベル付き ``max_vertex_count``・
ラベルなし63頂点、Python は1〜8頂点です。奇数の n と n < 4 では空出力になります。

埋め込み1件あたり O(n) 個の辺候補に O(n²) 時間の埋め込み正準化を行い、
抽象グラフの正準化には幅優先ラベル付けを全探索するため最悪指数時間かかります。
埋め込み集合とキュー、成分代表、全出力、ラベル展開の訪問済み集合を保持します。
外部の plantri や認識器は呼び出さず、plantri の専用正準縮約・軌道による
候補削減・処理速度や空間の保証は主張しません。

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

非同型列挙は ``connected_only`` 指定時に ``n = 16`` まで
`OEIS A005964 <https://oeis.org/A005964>`_ (連結 3-正則平面グラフ) を再現する
(n = 14 は133件、n = 16 は681件)。論文 Table 1 の埋め込み件数も16頂点まで一致する。
旧方式との出力集合一致はラベルなし10頂点・ラベル付き8頂点まで検証し、
橋・2頂点切断・同型な成分の繰り返しもテストする。静的テストケースは
``n = 12`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/cubic_planar.h"

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
   #include "enumerators/cubic_planar_labeled_enum.h"

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
   #include "enumerators/cubic_planar_unlabeled_enum.h"

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
