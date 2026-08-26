Even-hole-free グラフ
========================================

**Even-hole-free グラフ** は、長さ 4 以上の偶数長の誘導サイクルを含まない
グラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_547.html>`_ を参照。

認識
------------

認識関数 ``check_even_hole_free`` はアルゴリズムパラメータを取らない。
実装では、各辺 (u, v) について、N[u] ∪ N[v] の外側の部分グラフに制限した
N(u) と N(v) の間の奇数長誘導パスを探索する。このパスは u-v とあわせて
偶数ホールを成す。

.. doxygenstruct:: graph_recognition::EvenHoleFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_even_hole_free
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::EvenHoleFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EvenHoleFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_even_hole_free_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "even_hole_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_even_hole_free(g);

       std::cout << std::boolalpha << result.is_even_hole_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "even_hole_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_even_hole_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* M. Conforti, G. Cornuéjols, A. Kapoor, K. Vušković. "Even-hole-free graphs, Part I: Decomposition theorem."
  *Journal of Graph Theory*, 39(1):6--49, 2002.
  `DOI:10.1002/jgt.10006 <https://doi.org/10.1002/jgt.10006>`_

* M. V. G. da Silva, K. Vušković. "Decomposition of even-hole-free graphs with star cutsets and 2-joins."
  *Journal of Combinatorial Theory, Series B*, 103(1):144--183, 2013.
  `DOI:10.1016/j.jctb.2012.10.001 <https://doi.org/10.1016/j.jctb.2012.10.001>`_
