強正則グラフ
========================

グラフがパラメータ (n, k, lambda, mu) の強正則グラフかどうかを判定する。
k-正則グラフであって、隣接する任意の 2 頂点がちょうど lambda 個の共通近傍を持ち、
非隣接な任意の 2 頂点がちょうど mu 個の共通近傍を持つグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1185.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::StronglyRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_regular
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::StronglyRegularLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_regular_labeled_graphs
   :project: graph_recognition

上記の列挙器はラベル付きグラフを列挙する。もう一つの列挙器は同型類ごとに
代表元を 1 つずつ列挙する。McKay--Spence の方式に従い、実現可能なパラメータ組
``(k, lambda, mu)`` (ラベル付き列挙器と共有する等式
``k(k - lambda - 1) = mu(n - k - 1)`` と固有値の整数性条件で事前に絞り込む)
ごとに 1 回の探索を行い、各探索の内部では共有の正準拡大機構による
McKay の正準構成路法で同型除去を行う。強正則グラフは遺伝的でないが、
``srg(n, k, lambda, mu)`` の任意の誘導部分グラフは k-正則の次数不足条件と、
頂点対ごとの共通近傍数の窓 (共通近傍数は目標値 ``lambda`` / ``mu`` を
超えず、将来の共通近傍 1 つが両頂点の残り次数を 1 ずつ消費する) を満たす。
これらの必要条件を枝刈りに用いると、最終レベルでは全次数と全頂点対の
共通近傍数が目標値に一致することが強制されるため、レベル ``n`` に到達した
グラフは構成により強正則である。パラメータ組をまたぐ同型除去は不要である:
強正則グラフは ``(k, lambda, mu)`` を一意に定めるため、異なる探索が出力する
同型類は互いに素である。

.. doxygenenum:: graph_recognition::StronglyRegularUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::StronglyRegularUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_regular_unlabeled_graphs
   :project: graph_recognition

個数の検証
----------------

非同型列挙の個数は小さい強正則グラフの既知の分類と一致する:
``n = 4, ..., 15`` に対して ``2, 1, 4, 0, 4, 3, 6, 0, 8, 1, 4, 6``
(パラメータ依存のため単一の OEIS 数列は存在しない)。``n = 10`` の 6 類は
Petersen グラフ・その補グラフ・非原始的な 4 類 (``2K5``, ``5K2`` と
それらの補グラフ) であり、``n = 13`` では Paley グラフのみが得られる。
``n = 7`` までは、ラベル付き列挙の出力を正準形化して得られる同型類の
集合と一致することを検証した。静的テストケースは ``n = 13`` までである
(列挙が実用的なのはおよそ ``n = 15`` まで)。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/strongly_regular.h"

   int main() {
       using namespace graph_recognition;

       Graph g(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1}});
       auto result = check_strongly_regular(g);

       std::cout << std::boolalpha << result.is_strongly_regular << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/strongly_regular_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_regular_labeled_graphs(5);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/strongly_regular_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_regular_unlabeled_graphs(10);
       std::cout << result.graphs.size() << '\n';  // 6 (Petersen グラフを含む)
       return 0;
   }


参考文献
------------

* R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs."
  *Pacific Journal of Mathematics*, 13(2):389--419, 1963.
  `DOI:10.2140/pjm.1963.13.389 <https://doi.org/10.2140/pjm.1963.13.389>`_

* B. D. McKay, E. Spence. "Classification of regular two-graphs on 36 and 38 vertices."
  *Australasian Journal of Combinatorics*, 24:293--300, 2001.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
