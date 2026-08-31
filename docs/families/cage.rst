(k,g)-グラフ / ケージ
=====================

**(k,g)-グラフ** とは内周 (girth) が ``g`` 以上の ``k``-正則グラフである
(GENREG が生成するクラス。内周がちょうど ``g`` であることを要求する流儀も
あり、それは本ライブラリでは ``is_kg_graph && girth == g`` に当たる)。
**(k,g)-ケージ** とは内周がちょうど ``g`` の ``k``-正則グラフのうち頂点数
が最小のものであり、その頂点数を ``n(k,g)`` と書く: 完全グラフ
``K_{k+1}`` は (k,3)-ケージ、完全二部グラフ ``K_{k,k}`` は (k,4)-ケージ、
閉路 ``C_g`` は (2,g)-ケージ、Petersen グラフは (3,5)-ケージ、Heawood
グラフは (3,6)-ケージ、McGee グラフは (3,7)-ケージである。内周 ``>= g``
の ``k``-正則グラフ (``k >= 2``) は少なくとも ``M(k,g)`` 頂点を持つ —
**Moore 限界** で、``g`` が奇数なら
``1 + k((k-1)^{(g-1)/2} - 1)/(k-2)``、偶数なら
``2((k-1)^{g/2} - 1)/(k-2)`` である。部分 k-木や k-縮退グラフと同様
(本ライブラリの他のクラスとは異なり) 所属判定はパラメータ付きである:
``k`` と ``g`` は認識器・列挙器の入力であり、追加パラメータがラッパーの
統一シグネチャに合わないため、このクラスに Python バインディングはない。

認識
----------

正則性と内周は直接検査する: 内周は全頂点からの打ち切り BFS による標準的な
方法で ``O(nm)`` 時間で厳密に求め、どの入力に対しても報告する (0 = 非輪状
= 内周無限大)。ケージ性はさらに最小性を要し、多項式サイズの証明書は知られ
ていないため、列挙器の内周制約付き存在探索を Moore 限界 ``M(k,g)`` から
``n - 1`` までの実現可能な各頂点数に対して走らせて *厳密に* 判定する。
したがって Moore 限界と一致するケージ (``C_g``、``K_{k+1}``、``K_{k,k}``、
Petersen グラフ、Heawood グラフ) は探索なしで確定する一方、限界を超える
頂点数 1 つごとに全数的な非存在証明が必要になる — 最悪の場合指数時間で、
これが実用範囲の限界であり McGee グラフ (``n = 24``、``M(3,7) = 22`` の
2 つ上) には届かない。最小性は内周 ``>= g`` のグラフに対して検査するが、
これは ``n(k,g)`` の ``g`` に関する狭義単調性 (Fu--Huang--Rodger 1997)
により、内周がちょうど ``g`` という標準的な定義と一致する。

.. doxygenenum:: graph_recognition::CageAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CageResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cage
   :project: graph_recognition


列挙
----------

列挙器は ``n`` 頂点の内周 ``>= g`` の ``k``-正則グラフの同型類ごとに代表
元を 1 つ出力します — GENREG (Meringer 1999) の次数・内周制約付き orderly
generation を共有 canonical augmentation 機構の上で実現したもので、
k-正則列挙器 (``g <= 3`` の特殊ケース) の探索に内周制約を折り込んだもの
です。新頂点を近傍 ``S`` に結合して生じる閉路はすべて新頂点を通るため、
内周 ``>= g`` が保たれることと ``S`` の要素が現在のグラフで互いに距離
``>= g - 2`` にあることは同値です (スナーク列挙器の ``g = 5`` 規則の一般
化)。これは各レベルで 1 回計算する打ち切り BFS の「far マスク」で強制し
ます。k-正則探索の次数充足可能性枝刈りはそのまま引き継がれ、Moore 限界
未満の ``n`` は即座に空を返します。同型除去は McKay の canonical
construction path 法です。同じ探索の早期打ち切り版が
``cage_kg_graph_exists`` として公開されています — ケージ位数計算の非存在
証明側であり、``check_cage`` が最小性の保証に使うものです。

.. doxygenenum:: graph_recognition::CageUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CageUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CageUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cage_unlabeled_graphs
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::cage_kg_graph_exists
   :project: graph_recognition

数え上げの検証
--------------

``g <= 3`` は単純グラフに何の制約も課さないため、列挙は k-正則列挙器と
同型類単位で一致します (OEIS 三角形 A051031 の各行)。``k = 2`` は ``n``
の「各部分 ``>= g``」の分割数を数え (長さ ``>= g`` の閉路の非交和)、
``k <= 1`` のグラフは非輪状なので内周制約は空虚に成立します。(3,5) は
``n = 10`` で初めて空でなくなり、ちょうど Petersen グラフのみで、
``n = 10, 12, 14`` の個数は 1, 2, 9 — 連結の個数 A014372 と一致します
(``n = 20`` 未満に非連結な要素は存在しない)。(3,6) は ``n = 14`` で
初めて空でなくなり、ちょうど Heawood グラフのみ、``n = 16`` はちょうど
Möbius--Kantor グラフのみです。三正則ケージの位数は A000066 です。
GENREG のスループットは引き継がれません: 疎で内周の大きいグラフ (局所的
に木に見えるため多くの順序が同点になる) の上では厳密正準化の分岐が激しく、
実用範囲は (3,5) で ``n = 14``、(3,6) で ``n = 16`` 程度までです。


例
--------

認識の例
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cage.h"

   int main() {
       using namespace graph_recognition;

       // Petersen グラフは (3,5)-ケージ
       Graph g(10, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1},
                    {1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10},
                    {6, 8}, {8, 10}, {10, 7}, {7, 9}, {9, 6}});
       auto res = check_cage(g, 3, 5);
       std::cout << std::boolalpha
                 << res.is_cage << '\n';      // true
       std::cout << res.girth << '\n';        // 5
       std::cout << check_cage(g, 3, 6).is_kg_graph << '\n';  // false
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cage_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       // 10 頂点の (3,5)-グラフは Petersen グラフのみ
       auto result = enumerate_cage_unlabeled_graphs(10, 3, 5);
       std::cout << result.graphs.size() << '\n';  // 1
       // ... より少ない頂点数には存在しない (ケージ性)
       std::cout << cage_kg_graph_exists(8, 3, 5) << '\n';  // 0
       return 0;
   }


参考文献
----------

* M. Meringer. "Fast generation of regular graphs and construction of
  cages." *Journal of Graph Theory*, 30(2):137--146, 1999.

* G. Exoo, R. Jajcay. "Dynamic cage survey." *Electronic Journal of
  Combinatorics*, Dynamic Survey DS16, 2008 (revised 2013).

* H.-L. Fu, K.-C. Huang, C. A. Rodger. "Connectivity of cages."
  *Journal of Graph Theory*, 24(2):187--191, 1997.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.

* `OEIS A000066 <https://oeis.org/A000066>`_, `OEIS A051031 <https://oeis.org/A051031>`_, `OEIS A014372 <https://oeis.org/A014372>`_
