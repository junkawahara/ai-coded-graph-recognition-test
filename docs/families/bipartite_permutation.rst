二部順列グラフ
============================

グラフが二部順列グラフであるかを判定する。
二部グラフかつ順列グラフであるグラフを指す。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_81.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartitePermutationAlgorithm``
     - 説明
   * - ``BIPARTITE_AND_PERMUTATION`` **(既定)**
     - 二部グラフ認識器を実行した後、汎用の順列グラフ認識器を実行し、
       両方が成功した場合のみ受理する。chain 構造や strong ordering は
       計算しない。

.. doxygenenum:: graph_recognition::BipartitePermutationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bipartite_permutation
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::BipartitePermutationLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_permutation_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは順列グラフ・サークルグラフ・
弦グラフ・外平面グラフ・直並列グラフ・カクタスグラフの列挙器と同じ McKay の
canonical construction path 法です。二部順列グラフは遺伝的 (頂点を削除しても
二部性・順列グラフ性はともに保たれる) なので、頂点を 1 つずつ追加してグラフを
成長させてよいことが保証されます。枝刈りは候補となる子グラフごとの
``check_bipartite_permutation`` 呼び出し (二部性判定 + 汎用の順列グラフ判定)
で、これを同型除去より先に行うので、より高価な正準化は二部順列グラフに対して
しか実行されません。Saitoh, Otachi, Yamanaka, Uehara の専用列挙器は連結な
二部順列グラフを正準な文字列表現を通じて同型除去なしに生成し、Kawahara,
Saitoh, Takeda, Yoshinaka, Yoshioka の BDD ベースの構成は非同型なグラフを
n の多項式時間で列挙しますが、本実装は共有の canonical-augmentation 機構を
再利用する方式で、``n = 10`` 程度まで実用的です。個数は
1, 2, 3, 7, 13, 34, 81, 239, 693, ... (n = 1, 2, ...; 2026 年時点で OEIS に
未登録)、``connected_only`` を指定した場合はそのうち連結なもの
(1, 1, 1, 3, 5, 16, 38, 126, 375, ...; こちらも OEIS に未登録) です。

.. doxygenenum:: graph_recognition::BipartitePermutationUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::BipartitePermutationUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_permutation_unlabeled_graphs
   :project: graph_recognition

カウント検証
--------------------------------

非同型列挙については ``n = 8`` まで、列挙器自身とは独立に個数を検証した
(二部グラフの非同型列挙を ``check_bipartite_permutation`` で絞り込んでも
同じ個数 ``1, 2, 3, 7, 13, 34, 81, 239`` (連結では
``1, 1, 1, 3, 5, 16, 38, 126``) が得られる。二部順列グラフはすべて
二部グラフである)。総数の数列も連結の数列も OEIS に登録されていない
(2026-08 確認)。静的テストケースは ``n = 9`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "bipartite_permutation.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_bipartite_permutation(g);

       std::cout << std::boolalpha << result.is_bipartite_permutation << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "bipartite_permutation_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_permutation_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "bipartite_permutation_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_permutation_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 34
       return 0;
   }


参考文献
------------

* J. P. Spinrad, A. Brandstädt, L. Stewart. "Bipartite permutation graphs."
  *Discrete Applied Mathematics*, 18(3):279--292, 1987.
  `DOI:10.1016/S0166-218X(87)80003-3 <https://doi.org/10.1016/S0166-218X(87)80003-3>`_

* T. Saitoh, Y. Otachi, K. Yamanaka, R. Uehara. "Random generation and enumeration of bipartite permutation graphs."
  *Journal of Discrete Algorithms*, 10:84--97, 2012.
  `DOI:10.1016/j.jda.2011.11.001 <https://doi.org/10.1016/j.jda.2011.11.001>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* J. Kawahara, T. Saitoh, H. Takeda, R. Yoshinaka, Y. Yoshioka. "Efficient non-isomorphic graph enumeration algorithms for several intersection graph classes."
  *Theoretical Computer Science*, 1003:114591, 2024.
  `DOI:10.1016/j.tcs.2024.114591 <https://doi.org/10.1016/j.tcs.2024.114591>`_
