多面体グラフ
====================

**多面体グラフ** は 3-連結平面グラフである。Steinitz の定理により、
これはちょうど凸多面体のグラフに一致する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_986.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::PolyhedralAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_polyhedral
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::PolyhedralLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_labeled_graphs
   :project: graph_recognition

両列挙器の既定方式は ``PLANTRI`` です。Maximal planar 列挙の E3/E4/E5 拡張で
得られる三角形分割の埋め込みを根とし、三角形に接する辺を削除します
(Brinkmann--McKay, §4)。結合面と他の面との共通部分が空・1頂点・1境界辺の
いずれかであることを調べて3連結性を維持します。新方式では平面性・3連結性の
認識器を呼びません。

正準埋め込み上の最初の欠けた ear 対角線を追加したグラフを親と定義します。
親操作を繰り返すと三角形分割に到達します。親の一致と兄弟の正準形集合によって、
異なる削除経路や三角形分割からの重複を除きます。向き付き辺からの BFS を
両方向で行って鏡映を同一視し、3連結平面グラフの埋め込みの一意性を利用して
抽象グラフの同型類を判別します。ラベル付き版は各代表元を
``n! / |Aut(G)|`` 個の異なるラベル付きグラフに展開します。

従来の平面グラフの頂点追加・終端での多面体認識方式は、
``PolyhedralLabeledEnumAlgorithm::REVERSE_SEARCH`` または
``PolyhedralUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION`` で選択できます。
従来方式の出力順・代表元のラベルは維持されます。CLI は ``--algo plantri``
(既定)、``--algo reverse_search`` (ラベル付き)、
``--algo canonical_augmentation`` (ラベルなし) に対応します。
既存 Python API の ``enumerate_polyhedral_unlabeled_graphs(n,
algorithm="plantri")`` でも ``algorithm="canonical_augmentation"`` を指定できます。

C++・CLI の範囲は従来どおりラベル付き0〜64頂点、ラベルなし0〜63頂点、
Python は1〜8頂点です。4頂点未満は0件で、全出力が3連結なので
``connected_only`` オプションはありません。三角形分割の生成に加えて、辺削除探索は
1出力あたり O(n) 候補、候補あたり O(n²) 時間、探索領域 O(n³) を使います。
全出力と現在の代表元のラベル展開集合は別途保持します。外部の plantri 本体には
依存せず、そのインプレース更新・軌道による候補削減・処理速度は保証しません。

.. doxygenenum:: graph_recognition::PolyhedralUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PolyhedralUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 4, 5, 6`` について、列挙されたラベル付き多面体グラフの個数が
`OEIS A096330 <https://oeis.org/A096330>`_ の値 ``1, 25, 1227`` と
一致することを検証した。

非同型列挙は ``n = 10`` まで `OEIS A000944 <https://oeis.org/A000944>`_
(32,300件) と一致します。新旧方式の出力集合をラベル付きは ``n = 6``、
ラベルなしは ``n = 7`` まで比較しています。8頂点までの全同型類について全辺の
削除可否を既存の頂点対除去による3連結認識器と照合し、最小次数3を保っても
3連結性を失う場合も検証しています。静的テストケースは ``n = 8`` までです。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/polyhedral.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_polyhedral(g);

       std::cout << std::boolalpha << result.is_polyhedral << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/polyhedral_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/polyhedral_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 7
       return 0;
   }


参考文献
------------

* E. Steinitz. "Polyeder und Raumeinteilungen."
  *Encyclopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.

* B. Grünbaum. *Convex Polytopes.*
  Graduate Texts in Mathematics 221, Springer, 2nd edition, 2003.
  `DOI:10.1007/978-1-4613-0019-9 <https://doi.org/10.1007/978-1-4613-0019-9>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
