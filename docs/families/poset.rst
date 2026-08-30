半順序集合 (poset)
====================================

有向グラフが半順序集合の **ハッセ図** (被覆関係) かどうかを判定する。
入力は被覆関係として解釈され、有向辺 ``u -> v`` は ``v`` が ``u`` を
被覆する (``u < v``) ことを意味する。基礎となる半順序はその反射推移閉包である。
したがって、有効なハッセ図とは自身の推移簡約と一致する有向非巡回グラフ
である。半順序そのもの (反射ループや推移辺を含むもの) を入力として
与えては **ならない**。

認識
----------

.. doxygenenum:: graph_recognition::PosetAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PosetResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_poset
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::PosetLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PosetLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_posets
   :project: graph_recognition

上記の列挙器はラベル付き半順序集合を出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは McKay の canonical construction path 法です
(nauty スイートの ``genposetg`` はハッセ図をレベルごとに構築して同じリストを得ます)。
半順序集合は要素を 1 つずつ追加して成長させ (半順序集合からどの要素を削除しても
半順序集合のままなのですべての同型類に到達できる)、成長・正準化の対象は
狭義順序の有向グラフ — 推移閉包そのもの — であってハッセ図ではありません
(ハッセ図の頂点削除は部分半順序を取る操作と可換でないため)。子は新要素に
後続の上方集合と、それと交わらず全メンバーが全後続の下にある先行の下方集合を
組にして追加します。ちょうどこの組だけが推移閉包された DAG を保つので
認識フィルタは不要です。同型除去には有向グラフ列挙器の真に有向な正準形を
再利用します。出力はラベル付き列挙器に合わせて各半順序集合のハッセ図です。
``n = 9`` (183231 類、約 33 秒) 程度まで実用的です。ラベル付き列挙器に合わせて
``connected_only`` フラグはありません。

.. doxygenenum:: graph_recognition::PosetUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PosetUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PosetUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_poset_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付き半順序集合の個数が
`OEIS A001035 <https://oeis.org/A001035>`_ と一致することを検証した:
``1, 3, 19, 219, 4231``。

非同型列挙は `OEIS A000112 <https://oeis.org/A000112>`_ (ラベルなし
半順序集合の個数) を再現する: 静的テストケースの ``n = 1, ..., 8`` で
``1, 2, 5, 16, 63, 318, 2045, 16999`` (``n = 8`` は約 1.3 秒)、さらに
``n = 9`` の ``183231`` (約 33 秒) を手動で確認した。``n <= 5`` では
ラベル付き列挙器の出力を正準化した同型類の集合と完全に一致する。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "poset.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {2, 3}};
       auto result = check_poset(3, arcs);

       std::cout << std::boolalpha << result.is_poset << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "poset_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_posets(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "poset_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_poset_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';  // 16
       return 0;
   }


参考文献
------------

* R. P. Dilworth. "A decomposition theorem for partially ordered sets."
  *Annals of Mathematics*, 51(1):161--166, 1950.
  `DOI:10.2307/1969503 <https://doi.org/10.2307/1969503>`_

* G. Brinkmann, B. D. McKay. "Posets on up to 16 Points."
  *Order*, 19(2):147--179, 2002.
  `DOI:10.1023/A:1016543307592 <https://doi.org/10.1023/A:1016543307592>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
