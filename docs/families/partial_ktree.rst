部分 k-木
=========

**部分 k-木** (partial k-tree) は k-木の部分グラフ、同値に **木幅が k 以下**
のグラフである。``k = 1`` は森、``k = 2`` は直並列 (K4 マイナーフリー)
グラフに一致する。固定した ``k`` ごとにマイナー閉なクラスである。本ライブラリ
の他のクラスと異なり所属判定はパラメータ付きである: すべてのグラフは部分
``(n-1)``-木なので ``k`` を推定することはできず、認識器・列挙器ともに ``k``
を入力にとる。追加パラメータ ``k`` がラッパーの統一シグネチャに合わないため、
このクラスに Python バインディングはない。

認識
----------

``k`` を入力とする木幅 ``<= k`` の判定は NP 完全 (Arnborg--Corneil--
Proskurowski 1987) なので、認識器は **消去順序** 上のメモ化厳密探索である:
グラフの木幅が ``k`` 以下であることは、各頂点を消去するとき現在の *fill
グラフ* (内部がすでに消去された頂点のみからなる道で結ばれた 2 頂点を隣接と
みなすグラフ) における隣接頂点数が ``k`` 以下になるように、頂点を 1 つずつ
消去できることと同値である。探索は次に消去する頂点で分岐し、消去済み頂点
集合でメモ化する — fill グラフはその集合だけで決まるため、探索は
Bodlaender-Fomin-Koster-Kratsch-Thilikos の O*(2^n) 消去順序動的計画法で
抑えられる。ほとんどの入力はその上界に達する前に 3 つの枝刈りで決着する:
fill 次数 ``<= k`` の単体的頂点は分岐せず強制消去 (単体的 ``v`` について
``tw(G) = max(deg(v), tw(G - v))``)、fill 次数 ``> k`` の単体的頂点はその
枝全体を棄却 (閉 fill 近傍が ``k + 1`` 頂点超のクリーク)、残り頂点が
``k + 1`` 個以下になれば任意の順で完了。さらに縮退数 (degeneracy) の下界
(縮退数 ``<=`` 木幅) が多くの NO 入力を探索前に棄却する。

YES の場合は証明書として消去順序とその再生幅 (木幅の上界であり ``k`` 以下)
を返す。順序を逆から読むと、ある上位グラフの k-木構成順序になる。

.. doxygenenum:: graph_recognition::PartialKTreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PartialKTreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_partial_ktree
   :project: graph_recognition


列挙
----------

列挙器は ``n`` 頂点で木幅 ``<= k`` のグラフの同型類ごとに代表元を 1 つ出力
します — サーベイの「geng + 木幅フィルタ」経路を共有 canonical augmentation
機構の上で実現したものです。頂点を 1 つずつ追加して成長させます。クラスは
マイナー閉、特に遺伝的なので、はるかに高価な厳密正準化の前に
``check_partial_ktree`` がすべての候補の子を枝刈りします。同型除去は McKay
の canonical construction path 法です (``canonicalize_bitmask_graph`` が
報告する正準削除軌道に新頂点が入っている子だけが生き残ります)。

.. doxygenenum:: graph_recognition::PartialKTreeUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PartialKTreeUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PartialKTreeUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_partial_ktree_unlabeled_graphs
   :project: graph_recognition

数え上げの検証
--------------

``k = 1`` はラベルなしの森 (OEIS A005195: 1, 2, 3, 6, 10, 20, 37, ...;
連結モードでは木 A000055)、``k = 2`` は直並列グラフ (1, 2, 4, 10, 27, 92,
360, 1715, ...) を、専用列挙器と同型類単位で一致して再現する。
``k >= n - 1`` は全グラフ (A000088: 1, 2, 4, 11, 34, 156, ...) を与える。
木幅 ``<= 3`` の個数 (n = 1..8 で 1, 2, 4, 11, 33, 145, 861, 7604) は OEIS
に存在せず、``n <= 6`` の全グラフに対する力まかせの木幅フィルタと照合して
検証した。実用範囲は ``n = 8`` 程度まで。


使用例
--------

認識の例
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "partial_ktree.h"

   int main() {
       using namespace graph_recognition;

       // C5 の木幅は 2
       Graph g(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1}});
       std::cout << std::boolalpha
                 << check_partial_ktree(g, 1).is_partial_ktree << '\n';  // false
       std::cout << check_partial_ktree(g, 2).is_partial_ktree << '\n';  // true
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "partial_ktree_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_partial_ktree_unlabeled_graphs(6, 3);
       std::cout << result.graphs.size() << '\n';  // 145
       return 0;
   }


参考文献
----------

* S. Arnborg, D. G. Corneil, A. Proskurowski. "Complexity of finding
  embeddings in a k-tree." *SIAM Journal on Algebraic and Discrete
  Methods*, 8(2):277--284, 1987.

* H. L. Bodlaender. "A partial k-arboretum of graphs with bounded
  treewidth." *Theoretical Computer Science*, 209(1--2):1--45, 1998.

* H. L. Bodlaender, F. V. Fomin, A. M. C. A. Koster, D. Kratsch,
  D. M. Thilikos. "On exact algorithms for treewidth." *ACM Transactions
  on Algorithms*, 9(1):12:1--12:23, 2012.

* M. J. Dinneen. "Practical enumeration methods for graphs of bounded
  pathwidth and treewidth." CDMTCS Research Report CDMTCS-055,
  University of Auckland, 1997.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.

* `OEIS A005195 <https://oeis.org/A005195>`_, `OEIS A000055 <https://oeis.org/A000055>`_, `OEIS A000088 <https://oeis.org/A000088>`_
