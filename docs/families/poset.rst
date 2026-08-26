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

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付き半順序集合の個数が
`OEIS A001035 <https://oeis.org/A001035>`_ と一致することを検証した:
``1, 3, 19, 219, 4231``。


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


参考文献
------------

* R. P. Dilworth. "A decomposition theorem for partially ordered sets."
  *Annals of Mathematics*, 51(1):161--166, 1950.
  `DOI:10.2307/1969503 <https://doi.org/10.2307/1969503>`_

* G. Brinkmann, B. D. McKay. "Posets on up to 16 Points."
  *Order*, 19(2):147--179, 2002.
  `DOI:10.1023/A:1016543307592 <https://doi.org/10.1023/A:1016543307592>`_
