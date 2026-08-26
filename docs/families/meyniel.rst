Meyniel グラフ
============================

長さ 5 以上の全ての奇サイクルが少なくとも 2 本の弦を持つグラフを
**Meyniel グラフ** という。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_194.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``MeynielAlgorithm``
     - 説明
   * - ``DIRECT_CHECK`` **(既定)**
     - 長さ 5 以上の全ての奇サイクルが少なくとも 2 本の弦を持つことを
       直接検証する。

.. doxygenenum:: graph_recognition::MeynielAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MeynielResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_meyniel_direct
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_meyniel
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::MeynielLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MeynielLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_meyniel_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "meyniel.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_meyniel(g);

       std::cout << std::boolalpha << result.is_meyniel << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "meyniel_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_meyniel_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* H. Meyniel. "On the perfect graph conjecture."
  *Discrete Mathematics*, 16(4):339--342, 1976.
  `DOI:10.1016/S0012-365X(76)80008-8 <https://doi.org/10.1016/S0012-365X(76)80008-8>`_

* M. Burlet, J. Fonlupt. "Polynomial algorithm to recognize a Meyniel graph."
  *Annals of Discrete Mathematics*, 21:225--252, 1984.
  `DOI:10.1016/S0304-0208(08)72938-4 <https://doi.org/10.1016/S0304-0208(08)72938-4>`_

* F. Roussel, I. Rusu. "An O(n\ :sup:`2`) algorithm to color Meyniel graphs."
  *Discrete Mathematics*, 235(1--3):107--123, 2001.
  `DOI:10.1016/S0012-365X(00)00264-8 <https://doi.org/10.1016/S0012-365X(00)00264-8>`_
