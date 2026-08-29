カクタスグラフ
====================

各二連結成分が単一の辺または単純サイクルであるグラフを **カクタスグラフ** という。
同値な定義として、任意の 2 つの単純サイクルが高々 1 つの頂点しか共有しない、とも言える。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_108.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CactusAlgorithm``
     - 説明
   * - ``DFS`` **(既定)**
     - DFS による二連結成分分解。各成分が単一の辺または単純サイクルであることを検証する。計算量: O(n + m)。

.. doxygenenum:: graph_recognition::CactusAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cactus
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::CactusLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cactus_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは順列グラフ・サークルグラフ・
弦グラフ・外平面グラフ・直並列グラフの列挙器と同じ McKay の canonical
construction path 法です。カクタスグラフは遺伝的 (頂点を削除してもブロックが
縮むだけ) なので、頂点を 1 つずつ追加してグラフを成長させてよいことが保証
されます。枝刈りは候補となる子グラフごとの ``check_cactus`` 呼び出し (DFS に
よる二連結成分分解) で、これを同型除去より先に行うので、より高価な正準化は
カクタスグラフに対してしか実行されません。Bahrani と Lumbroso の専用列挙器は
クラスの split-decomposition 文法を導出して非同型なカクタスグラフを直接生成し
(根付き版は 1 グラフあたり定数時間)、正準化を行いませんが、本実装は共有の
canonical-augmentation 機構を再利用する方式で、``n = 10`` 程度まで実用的です。
個数は 1, 2, 4, 9, 20, 51, 133, 380, 1144, ... (n = 1, 2, ...; 2026 年時点で
OEIS に未登録)、``connected_only`` を指定した場合はそのうち連結なもの
(`A000083 <https://oeis.org/A000083>`_: 1, 1, 2, 4, 9, 23, 63, 188, 596, ...)
です。

.. doxygenenum:: graph_recognition::CactusUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CactusUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cactus_unlabeled_graphs
   :project: graph_recognition

カウント検証
--------------------------------

非同型列挙については ``n = 8`` まで、列挙器自身とは独立に個数を検証した
(平面グラフの非同型列挙を ``check_cactus`` で絞り込んでも同じ個数
``1, 2, 4, 9, 20, 51, 133, 380`` (連結では ``1, 1, 2, 4, 9, 23, 63, 188``)
が得られる。カクタスグラフはすべて平面グラフである)。連結の個数は
``n = 10`` (1979) まで `A000083 <https://oeis.org/A000083>`_ と一致する。
非連結を含む総数の数列は OEIS に登録されていない (2026-08 確認)。
静的テストケースは ``n = 8`` までである。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cactus.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_cactus(g);

       std::cout << std::boolalpha << result.is_cactus << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cactus_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cactus_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cactus_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cactus_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 51
       return 0;
   }


参考文献
------------

* F. Harary, G. E. Uhlenbeck. "On the number of husimi trees, I."
  *Proceedings of the National Academy of Sciences*, 39(4):315--322, 1953.
  `DOI:10.1073/pnas.39.4.315 <https://doi.org/10.1073/pnas.39.4.315>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* M. Bahrani, J. Lumbroso. "Split-decomposition trees with prime nodes: enumeration and random generation of cactus graphs."
  *Proceedings of ANALCO 2018*, 2018.
  `arXiv:1711.10647 <https://arxiv.org/abs/1711.10647>`_
