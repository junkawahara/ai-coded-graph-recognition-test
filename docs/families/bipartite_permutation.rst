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

両列挙器の既定は ``STRING_REPRESENTATION`` です。連結グラフは Saitoh らの
論文 §4 の端点交換による reverse search で生成します。各二部集合サイズ
``p >= q`` について ``K_{p,q}`` から出発し、最大3か所の候補位置で親操作を
逆向きに適用します。接頭辞の高さで連結性を判定し、正準文字列で反転・上下交換・
回転の重複を除きます。認識器や汎用のグラフ正準化は呼び出しません。
非正準な途中表現も正準な子孫を持つ場合があるため、正準性は出力時だけ判定します。

非連結グラフは連結成分の多重集合として合成します。成分サイズを非増加順、
同サイズの成分番号を非減少順に選ぶことで、同型な成分の交換による重複を防ぎます。
ラベル付き版は隣接ラベル交換と訪問済み辺集合で各代表を
``n! / |Aut(G)|`` 個の異なるラベル付きグラフに展開します。

従来方式はラベル付き版の ``REVERSE_SEARCH``、ラベルなし版の
``CANONICAL_AUGMENTATION`` として残しています。認識器で候補を判定する
頂点追加方式と従来の出力順を維持し、ラベルなし版では McKay の正準親判定も
使用します。新方式では出力順やラベルなし代表元のラベル付けが変わり得ます。

CLI は ``--algo string_representation``（既定）、ラベル付き版では
``--algo reverse_search``、ラベルなし版では ``--algo canonical_augmentation``
を選択できます。ラベルなし版の ``--connected`` は両方式で使えます。
Python は同じ名前を ``algorithm=`` で指定し、ラベルなし版の第2位置引数
``connected_only=False`` も維持しています。

C++・CLI の頂点数上限は従来どおりラベル付き64、ラベルなし63です。
``n = 0`` は ``connected_only`` を指定しても空グラフ1件を返します。
Python の範囲はラベル付き ``1 <= n <= 6``、ラベルなし ``1 <= n <= 8`` です。

文字列走査は O(n)、辺リスト構築は O(n²) で、連結代表1件あたり償却 O(n²) 時間、
出力を除く探索領域は O(n²) です。成分代表・ラベル展開の訪問済み集合・全出力を
メモリに保持します。定数時間の更新・prepostorder・二部集合サイズ間の jump は
実装しておらず、論文の O(1) 遅延保証は適用されません。
非同型グラフの個数は ``1, 2, 3, 7, 13, 34, 81, 239, 693, ...``、
連結のみでは ``1, 1, 1, 3, 5, 16, 38, 126, 375, ...``（``n = 1, 2, ...``）です。

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
   #include "util/graph.h"
   #include "recognizers/bipartite_permutation.h"

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
   #include "enumerators/bipartite_permutation_labeled_enum.h"

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
   #include "enumerators/bipartite_permutation_unlabeled_enum.h"

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
