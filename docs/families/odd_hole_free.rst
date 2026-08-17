Odd-hole-free グラフ
======================================

**Odd-hole-free グラフ** は、長さ 5 以上の奇数長の誘導サイクルを含まない
グラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_356.html>`_ を参照。

認識
------------

認識関数 ``check_odd_hole_free`` はアルゴリズムパラメータを取らない。
``perfect.h`` の ``has_odd_hole`` を再利用しており、各辺について制限した
部分グラフ上の BFS と DFS により奇数ホールを検出する。

.. doxygenstruct:: graph_recognition::OddHoleFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_odd_hole_free
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::OddHoleFreeEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OddHoleFreeEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_odd_hole_free_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "odd_hole_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_odd_hole_free(g);

       std::cout << std::boolalpha << result.is_odd_hole_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "odd_hole_free_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_odd_hole_free_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* M. Chudnovsky, A. Scott, P. Seymour, S. Spirkl. "Detecting an odd hole."
  *Journal of the ACM*, 67(1):5:1--5:12, 2020.
  `DOI:10.1145/3375720 <https://doi.org/10.1145/3375720>`_
