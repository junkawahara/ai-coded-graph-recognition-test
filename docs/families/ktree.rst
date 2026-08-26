k-木
==========

グラフが **k-木** であるとは、弦グラフであり、すべての極大クリークのサイズが
ちょうど k + 1、すべての極小セパレータのサイズがちょうど k であることをいう。
同値な定義として、k-木は完全グラフ ``K_{k+1}`` から出発し、クリークをなす
ちょうど k 個の頂点に隣接する新しい頂点を繰り返し追加することで構成できる。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_594.html>`_ を参照。

認識
------------

.. doxygenenum:: graph_recognition::KTreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KTreeResult
   :project: graph_recognition
   :members:

``KTreeResult::construction_order`` は構築順序そのものを返す。先頭 k+1 頂点が
最初の K_{k+1} で、それ以降の各頂点はちょうど k 個の既出頂点に隣接し、その
k 個はクリークをなす。認識器が行う simplicial 頂点消去の逆順である。
返す前にグラフに対して再生して検証している。

.. doxygenfunction:: graph_recognition::check_ktree
   :project: graph_recognition


列挙
------------

.. doxygenstruct:: graph_recognition::KTreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_ktree_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "ktree.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_ktree(g);

       std::cout << std::boolalpha << result.is_ktree << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "ktree_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_ktree_labeled_graphs_reverse_search(4, 1);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
--------------

* D. J. Rose. "On simple characterizations of k-trees."
  *Discrete Mathematics*, 7(3--4):317--322, 1974.
  `DOI:10.1016/0012-365X(74)90042-9 <https://doi.org/10.1016/0012-365X(74)90042-9>`_

* L. W. Beineke, R. E. Pippert. "The number of labeled k-dimensional trees."
  *Journal of Combinatorial Theory*, 6(2):200--205, 1969.
  `DOI:10.1016/S0021-9800(69)80120-1 <https://doi.org/10.1016/S0021-9800(69)80120-1>`_
