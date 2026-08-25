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

認識関数 ``check_proper_chordal`` はアルゴリズムパラメータを取らない。
各頂点を根とするブロック木を構築し (Paul & Protopapas 2024, Algorithm 1)、
各ブロックについて nested-convex 条件を検証する (Algorithm 2)。
論文のアルゴリズムは O(n^4) だが、本実装の nested-convex 検証はブロック頂点の
全順列を試すため、最大ブロックのサイズに関して最悪指数時間となる。

.. doxygenstruct:: graph_recognition::ProperChordalResult
   :project: graph_recognition
   :members:

``ProperChordalResult::layout_parent`` は認識が実際に構成している
indifference tree-layout そのものを返す (:doc:`../api/utilities` の
``tree_layout.h``)。

.. doxygenfunction:: graph_recognition::check_proper_chordal
   :project: graph_recognition


列挙
------------

既定の列挙器は proper chordal グラフ専用の辺追加逆探索である。空グラフを根とし、
認識器が構成する決定的な indifference tree-layout ``T(G)`` 上で、木距離最大の辺
（同率は辞書順最小）を ``e(G)`` とし、非空グラフ ``G`` の親を ``G-e(G)`` と
定義する。木距離最大の辺は同じ layout を保ったまま必ず削除できるので、親は常に
proper chordal である。子候補は欠けている辺を 1 本追加し、その辺が子の正規親辺に
なる場合だけ採用する。このため探索ノードはすべて proper chordal であり、chordal
グラフ全体を列挙してフィルタすることはない。

原論文は列挙法を提示していない。この逆探索は、同論文 Theorem 6 の
indifference tree-layout の特徴付けから導出したものである。旧 chordal-filter 実装は
``LEGACY_CHORDAL_FILTER`` として差分検証用に残している。現在の認識器は
nested-convex 条件を全順列で検査するため、実装の delay は理論上の多項式境界には
ならない。全出力を保持しない callback API では探索領域は O(n²) である。

.. doxygenenum:: graph_recognition::ProperChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_chordal_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_proper_chordal_graphs_reverse_search_cb
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

* C. Paul, E. Protopapas. "Tree-Layout Based Graph Classes: Proper Chordal Graphs."
  *Proceedings of STACS 2024*, LIPIcs 289, 55:1--55:18, 2024.
  `DOI:10.4230/LIPIcs.STACS.2024.55 <https://doi.org/10.4230/LIPIcs.STACS.2024.55>`_
