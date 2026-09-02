ライングラフ (線グラフ)
============================================

**ライングラフ** L(H) は、あるグラフ H の辺を頂点とし、対応する辺が端点を
共有するとき 2 頂点を隣接させたグラフである。Krausz の定理 (1943) により、
グラフがライングラフであることと、Krausz 分割 (各頂点が高々 2 つのクリークに
属する辺クリーク被覆) を持つことは同値である
(Whitney の定理 (1932) は、K_3 と K_{1,3} の場合を除いて L(H) から H が
一意に定まることを述べたもので、別の定理である)。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_249.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``LineGraphAlgorithm``
     - 説明
   * - ``BRUTE``
     - Krausz 分割のバックトラッキング探索。各辺を含むクリーク候補を全て
       列挙する。小さなグラフに向く。
   * - ``KRAUSZ`` **(既定)**
     - 前提条件フィルタ (各頂点の近傍の補グラフの二部性判定) の後、
       枝刈り付きで Krausz 分割を構築する。フィルタは O(m * Delta) で
       大半の入力を判定できるが、フィルタを通過した曖昧なケースでは
       分割の構築がバックトラッキングになるため、最悪計算量は指数時間。

.. doxygenenum:: graph_recognition::LineGraphAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LineGraphResult
   :project: graph_recognition
   :members:

``LineGraphResult`` は Krausz 分割 (``krausz``) と根グラフ (``root_graph``)、
および各頂点が対応する根グラフの辺 (``vertex_to_root_edge``) を返す。
g の頂点は、それを含む (高々 2 個の) クリークを結ぶ H の辺になる。含まれる
クリークが 2 個未満の頂点には専用の H 頂点を補うため、孤立頂点や端点辺も
正しく扱える。返す前に L(root_graph) == g を検証している。

.. doxygenfunction:: graph_recognition::check_line_graph
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::LineGraphLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LineGraphLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_line_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/line_graph.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_line_graph(g);

       std::cout << std::boolalpha << result.is_line_graph << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/line_graph_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_line_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* H. Whitney. "Congruent graphs and the connectivity of graphs."
  *American Journal of Mathematics*, 54(1):150--168, 1932.
  `DOI:10.2307/2371086 <https://doi.org/10.2307/2371086>`_

* J. Krausz. "Démonstration nouvelle d'un théorème de Whitney sur les réseaux."
  *Matematikai és Fizikai Lapok*, 50:75--85, 1943.

* L. W. Beineke. "Derived graphs of digraphs."
  In H. Sachs, H.-J. Voss, H.-J. Walther (eds.), *Beiträge zur Graphentheorie*,
  Teubner, pp. 17--33, 1968.

* N. D. Roussopoulos. "A max {m, n} algorithm for determining the graph H from its line graph G."
  *Information Processing Letters*, 2(4):108--112, 1973.
  `DOI:10.1016/0020-0190(73)90029-X <https://doi.org/10.1016/0020-0190(73)90029-X>`_

* P. G. H. Lehot. "An optimal algorithm to detect a line graph and output its root graph."
  *Journal of the ACM*, 21(4):569--575, 1974.
  `DOI:10.1145/321850.321853 <https://doi.org/10.1145/321850.321853>`_
