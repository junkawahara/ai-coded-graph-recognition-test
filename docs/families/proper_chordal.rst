Proper Chordal グラフ
========================================

グラフが **proper chordal** であるとは、indifference tree-layout を持つ
弦グラフであることをいう (Paul & Protopapas 2024)。「どの部分木も他の部分木に
包含されない部分木交差表現」はこのクラスの定義にはならないことに注意:
すべての弦グラフはそのような表現を持つ (各部分木に専用の新しい葉を
付け加えれば包含関係は常に解消できる)。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1363.html>`_ を参照。

認識
------------

.. doxygenstruct:: graph_recognition::ProperChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_proper_chordal
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ProperChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_chordal_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "proper_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_chordal(g);

       std::cout << std::boolalpha << result.is_proper_chordal << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "proper_chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* C. Paul, E. Protopapas. "Proper chordal graphs."
  *Proceedings of STACS 2024*, LIPIcs 289, 53:1--53:17, 2024.
  `DOI:10.4230/LIPIcs.STACS.2024.53 <https://doi.org/10.4230/LIPIcs.STACS.2024.53>`_
