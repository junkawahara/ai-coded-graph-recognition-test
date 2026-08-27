サークルグラフ
============================

**サークルグラフ** は、円の弦の集合の交差グラフである。2 頂点が隣接するのは、
対応する弦が交差するとき、かつそのときに限る。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_132.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircleAlgorithm``
     - 説明
   * - ``NAJI_SYSTEM`` **(既定)**
     - Naji の特徴付けによる方法: グラフがサークルグラフであることと、
       GF(2) 上のある連立一次方程式 (相異なる頂点の順序対ごとに 1 変数)
       が可解であることは同値。可解性はビットセットによるガウス消去で
       多項式時間で判定する。判定のみで、弦図は生成しない。
       実装はまず双子頂点 (twin) のクラスを縮約して入力を縮小し、密な
       GF(2) 基底のメモリ使用量に上限を設けている (超過時は
       ``std::runtime_error`` を送出)。
       (Naji 1985; Gasse, *Discrete Math.* 173, 1997; Geelen–Lee, *J. Graph Theory* 93, 2020)
   * - ``DOW_BACKTRACKING``
     - DOW (double occurrence word) による弦図バックトラッキング。弦の端点を
       円周上に配置して、入力グラフと整合する弦図の構築を試みる。
       YES の場合は明示的な DOW 証明書を返すが、最悪の場合は指数時間かかる
       (実用上は n = 9 程度まで; NO の判定側が高コスト)。
       探索にはステップ数の予算が設けられており、使い切った場合は無限に
       走り続ける代わりに ``std::runtime_error`` を送出する。

.. doxygenenum:: graph_recognition::CircleAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_circle
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::CircleLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_circle_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction
path 法 (Johnston (2020) が順列グラフとサークルグラフに適用した canonical
deletion。サークルグラフの個数は n = 13 まで計算されています) です。
サークルグラフは遺伝的なので、頂点を 1 つずつ追加してグラフを成長させてよい
ことが保証されます。ただし頂点を追加したグラフがクラスに留まるかを判定する
軽い漸進的テストがないため、順列グラフの列挙器と同様に、枝刈りは候補となる
子グラフごとの認識器 (多項式時間の Naji システム) の呼び出しになります。
これを同型除去より先に行うので、より高価な正準化はサークルグラフに対して
しか実行されません。子グラフは、追加した頂点がその子の正準ラベリングで
最後に置かれる頂点の自己同型軌道に属するときに限り採用されます。個数は
OEIS A156809(n) (1, 2, 4, 11, 34, 154, 978, 9497, 127954, ...。5 頂点以下の
グラフはすべてサークルグラフです)、``connected_only`` を指定した場合は
そのうち連結なもの (A156808: 1, 1, 2, 6, 21, 110, 789, 8336, ...) です。

.. doxygenenum:: graph_recognition::CircleUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CircleUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_circle_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

非同型列挙については ``n = 9`` まで
`OEIS A156809 <https://oeis.org/A156809>`_ の
``1, 2, 4, 11, 34, 154, 978, 9497, 127954`` と一致することを、列挙器自身とは
独立に検証した (``n`` 頂点の非同型グラフをすべて生成して ``check_circle``
で絞り込んでも同じ個数が得られる)。``connected_only`` を指定した場合の個数は
`OEIS A156808 <https://oeis.org/A156808>`_ の
``1, 1, 2, 6, 21, 110, 789, 8336`` と一致する。静的テストケースは ``n = 7``
までである。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "circle.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_circle(g);

       std::cout << std::boolalpha << result.is_circle << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "circle_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_circle_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "circle_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_circle_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 154 = A156809(6)
       return 0;
   }


参考文献
----------------

* W. Naji. "Reconnaissance des graphes de cordes."
  *Discrete Mathematics*, 54(3):329--337, 1985.
  `DOI:10.1016/0012-365X(85)90117-7 <https://doi.org/10.1016/0012-365X(85)90117-7>`_

* E. Gasse. "A proof of a circle graph characterization."
  *Discrete Mathematics*, 173(1--3):277--283, 1997.
  `DOI:10.1016/S0012-365X(97)00068-X <https://doi.org/10.1016/S0012-365X(97)00068-X>`_

* J. Geelen, E. Lee. "Naji's characterization of circle graphs."
  *Journal of Graph Theory*, 93(1):21--33, 2020.
  `DOI:10.1002/jgt.22466 <https://doi.org/10.1002/jgt.22466>`_

* S. Even, A. Itai. "Queues, stacks and graphs."
  In Z. Kohavi, A. Paz (eds.), *Theory of Machines and Computations*, Academic Press,
  pp. 71--86, 1971.

* J. P. Spinrad. "Recognition of circle graphs."
  *Journal of Algorithms*, 16(2):264--282, 1994.
  `DOI:10.1006/jagm.1994.1012 <https://doi.org/10.1006/jagm.1994.1012>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* Johnston. Canonical-deletion enumeration of permutation / circle graphs, 2020.
