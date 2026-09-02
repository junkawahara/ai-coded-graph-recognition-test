アポロニアンネットワーク
========================

**アポロニアンネットワーク** (平面 3-木) は、三角形から始めて「(三角形の) 面を
1 つ選び、その 3 頂点すべてと結んだ新頂点を内部に挿入する」操作を繰り返して
得られるグラフである。同値な特徴づけとして、平面 3-木 (``n >= 4``)、
積層三角形分割 (stacked triangulation / stack polytope のグラフ)、
一意 4-彩色可能平面グラフ、そして **弦的な極大平面グラフ** がある。
``n >= 3`` 頂点のアポロニアンネットワークの辺数はちょうど ``3n - 6`` である。

認識
----------

認識は極大平面性判定と弦グラフ判定の組み合わせである: 挿入列に関する帰納法で
アポロニアンネットワークは弦的かつ極大平面であり、逆に ``n >= 5`` 頂点の弦的
極大平面グラフの単体的頂点は次数ちょうど 3 (次数 4 以上の単体的頂点は平面
グラフに K5 を埋め込んでしまう) なので、それを削除すると再び弦的極大平面
グラフになり、帰納的に挿入列を逆向きに再構成できる。

.. doxygenenum:: graph_recognition::ApollonianAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ApollonianResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_apollonian
   :project: graph_recognition


列挙
----------

列挙器は同型類ごとに代表元を 1 つだけ出力します。K3 から出発し、現在の
グラフの各三角形に新頂点を結んで子を生成します。構成上 ``m = 3k - 6`` と
弦性 (新頂点は単体的) は自動的に保たれるので、子が平面かどうかの検査
(``check_planar`` 1 回) だけがクラス所属判定になります — 面への挿入は平面性を
保ち、分離三角形への挿入は必ず平面性を壊します。

同型除去は McKay の canonical construction path 法ですが、このクラスは遺伝的
ではないため削除規則を **次数 3 の頂点** に制限します (次数 3 頂点のリンクは
三角形なので削除してもクラスに留まり、``n >= 4`` では必ず存在します)。正準親は
「正準形の中で次数 3 を持つ最後の位置」の頂点を削除します: 位置ごとの次数は
正準形から復元でき、``canonicalize_bitmask_graph_orbits``
(util/canonical_augmentation.h) が各位置の自己同型軌道を報告するので、
新頂点がその位置の軌道に属する子だけが生き残ります。正準化の対象は
**補グラフ** です: このクラスは K5 を含まないため、補グラフの辞書式最小探索は
4 位置でタイが消滅し、グラフ自身を正準化する場合 (大きな独立集合のタイで
爆発) の約 100 倍高速です。

個数は ``n <= 3`` で 1 (K0, K1, K2, K3)、以降は OEIS A027610(n-3)
(n = 4, 5, ... に対して 1, 1, 1, 3, 7, 24, 93, 434, 2110, 11002, ...) です。
鏡像を区別する A007173 ではないことに注意 (グラフ同型は鏡映を含みます)。
実用範囲はおよそ ``n = 13`` まで (11,002 類、約 5 分; n = 12 は約 8 秒)。
アポロニアンネットワークはすべて連結なので、``connected_only`` フラグは
ありません。

.. doxygenenum:: graph_recognition::ApollonianUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ApollonianUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ApollonianUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_apollonian_unlabeled_graphs
   :project: graph_recognition

個数チェック
------------

非同型列挙は ``n = 13`` まで OEIS A027610(n-3) を再現します (n = 4..13 に
対して 1, 1, 1, 3, 7, 24, 93, 434, 2110, 11002)。``n <= 6`` では、``K_n`` の
辺集合のうちサイズ ``3n - 6`` で認識器が受理するすべての部分集合を正準化した
結果と同型類集合が完全に一致し、さらに ``n <= 8`` では極大平面グラフの
非同型列挙器の出力を弦性でフィルタした結果とも一致します。静的テストケースは
``n = 11`` までです。


例
--------

認識の例
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/apollonian.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_apollonian(g);

       std::cout << std::boolalpha << result.is_apollonian << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/apollonian_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_apollonian_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 7
       return 0;
   }


参考文献
----------

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.

* F. Hering, R. Hoede, H. Fleischner. "The enumeration of stack polytopes
  and simplicial clusters." *Discrete Mathematics*, 40(2--3):203--217, 1982.

* L. W. Beineke, R. E. Pippert. "Enumerating dissectable polyhedra by
  their automorphism groups." *Canadian Journal of Mathematics*,
  26(1):50--67, 1974.

* `OEIS A027610 <https://oeis.org/A027610>`_, `OEIS A001764 <https://oeis.org/A001764>`_
