k-縮退グラフ
============

グラフが **k-縮退** (k-degenerate) であるとは、空でないすべての誘導部分
グラフが次数 ``k`` 以下の頂点を持つことをいう (Lick--White 1970)。同値に、
削除時に残っている頂点のうち隣接するものが ``k`` 個以下になるように頂点を
1 つずつ削除できることと同値である。そのような最小の ``k`` が **縮退数**
(degeneracy) である。``k = 1`` は森に一致し、外平面グラフは 2-縮退、平面
グラフは 5-縮退であり、木幅 ``<= k`` ならば k-縮退である。固定した ``k``
ごとに遺伝的なクラスである。部分 k-木と同様 (本ライブラリの他のクラスとは
異なり) 所属判定はパラメータ付きである: すべてのグラフは ``(n-1)``-縮退
なので ``k`` を推定することはできず、認識器・列挙器ともに ``k`` を入力に
とる。追加パラメータ ``k`` がラッパーの統一シグネチャに合わないため、この
クラスに Python バインディングはない。

認識
----------

認識器は古典的な **最小次数剥ぎ取り** (Matula--Beck 1983 の smallest-last
順序) をバケットキューで実装したものである: 残りのグラフで次数最小の頂点を
繰り返し削除する。削除時に観測した次数の最大値がちょうど縮退数であり、
``O(n + m)`` 時間で求まる。厳密な縮退数が無償で得られるため、YES/NO の
どちらの場合にも縮退数を報告する。YES の場合は剥ぎ取り順序が証明書になる
(各頂点は削除時に未削除の隣接頂点を ``k`` 個以下しか持たない)。NO の場合は
**(k+1)-コア** — 次数 ``<= k`` の頂点を消去し尽くして得られる、最小次数
``>= k + 1`` の唯一の極大誘導部分グラフ — が証明書になる: すべての頂点が
その内部に ``k`` 個超の隣接頂点を持つ空でない誘導部分グラフであり、削除
順序が存在しないことを保証する。

.. doxygenenum:: graph_recognition::DegenerateAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DegenerateResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_degenerate
   :project: graph_recognition


列挙
----------

列挙器は ``n`` 頂点の k-縮退グラフの同型類ごとに代表元を 1 つ出力します —
サーベイの「geng + 縮退数フィルタ」経路を共有 canonical augmentation 機構
の上で実現したものです。頂点を 1 つずつ追加して成長させます。クラスは遺伝
的なので、はるかに高価な厳密正準化の前に ``O(n + m)`` の
``check_degenerate`` 剥ぎ取りがすべての候補の子を枝刈りします。同型除去は
McKay の canonical construction path 法です (``canonicalize_bitmask_graph``
が報告する正準削除軌道に新頂点が入っている子だけが生き残ります)。
Bauer--Krug--Wagner (ANALCO 2010) の専用アルゴリズムは well-ordering に
基づいて *ラベル付き* k-縮退グラフを数え上げ・生成するもので、この非同型
列挙経路では使用していません。

.. doxygenenum:: graph_recognition::DegenerateUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DegenerateUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DegenerateUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_degenerate_unlabeled_graphs
   :project: graph_recognition

数え上げの検証
--------------

``k = 1`` はラベルなしの森 (OEIS A005195: 1, 2, 3, 6, 10, 20, 37, ...;
連結モードでは木 A000055) を専用列挙器と同型類単位で一致して再現する。
``k = 0`` は無辺グラフのみ、``k >= n - 1`` は全グラフ (A000088: 1, 2, 4,
11, 34, 156, ...) を与える。2-縮退グラフの個数 (n = 1..9 で 1, 2, 4, 10,
28, 105, 508, 3454, 31935)、連結 2-縮退グラフの個数 (1, 1, 2, 5, 16, 68,
375, 2822, ...)、3-縮退グラフの個数 (n = 1..7 で 1, 2, 4, 11, 33, 148,
950) は OEIS に存在せず、``n <= 5`` の全グラフに対する力まかせの縮退数
フィルタと照合して検証した。木幅 ``<= k`` ならば k-縮退なので、出力は部分
k-木の同型類を含み、``n = 5, k = 2`` 以降は真に含む (28 対 27: K4 の 1 辺
を細分したグラフは 2-縮退だが K4 マイナーを持つため木幅 3)。実用範囲は
``n = 8`` 程度まで。


使用例
--------

認識の例
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "degenerate.h"

   int main() {
       using namespace graph_recognition;

       // C5 の縮退数は 2
       Graph g(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1}});
       std::cout << std::boolalpha
                 << check_degenerate(g, 1).is_degenerate << '\n';  // false
       std::cout << check_degenerate(g, 2).is_degenerate << '\n';  // true
       std::cout << check_degenerate(g, 1).degeneracy << '\n';     // 2
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "degenerate_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_degenerate_unlabeled_graphs(6, 2);
       std::cout << result.graphs.size() << '\n';  // 105
       return 0;
   }


参考文献
----------

* D. R. Lick, A. T. White. "k-degenerate graphs." *Canadian Journal of
  Mathematics*, 22(5):1082--1096, 1970.

* D. W. Matula, L. L. Beck. "Smallest-last ordering and clustering and
  graph coloring algorithms." *Journal of the ACM*, 30(3):417--427,
  1983.

* S. B. Seidman. "Network structure and minimum degree." *Social
  Networks*, 5(3):269--287, 1983.

* R. Bauer, M. Krug, D. Wagner. "Enumerating and generating labeled
  k-degenerate graphs." *Proceedings of ANALCO 2010*, pp. 90--98, 2010.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.

* `OEIS A005195 <https://oeis.org/A005195>`_, `OEIS A000055 <https://oeis.org/A000055>`_, `OEIS A000088 <https://oeis.org/A000088>`_
