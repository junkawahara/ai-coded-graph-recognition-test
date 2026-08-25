強弦グラフ
====================

グラフが **強弦グラフ** (strongly chordal graph) であるとは、弦グラフであり、
かつ長さ 6 以上のすべての偶サイクルが奇弦 (サイクルに沿って奇数距離にある
2 頂点を結ぶ弦) を持つことをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_125.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``StronglyChordalAlgorithm``
     - 説明
   * - ``STRONG_ELIMINATION``
     - 強消去順序 (strong elimination ordering) の検査、O(n^4)
   * - ``PEO_MATRIX``
     - 全走査による simple vertex 消去 (名前は歴史的経緯によるもので、
       行列は構築しない)、最悪 O(n m Delta)
   * - ``MCS_SEO`` **(既定)**
     - 次数順ソートした包含判定を用いる simple vertex 消去、
       最悪 O(n m Delta)

   * - ``FARBER_SEO``
     - Farber の部分順序構成 O(n^4)。strong elimination ordering を報告する
       唯一のバリアント (:doc:`../api/utilities` の ``elimination_orderings.h``)。
       他の 3 つは任意の simple 頂点を消去するため、認識はできても
       strong elimination ordering は得られない。

.. doxygenenum:: graph_recognition::StronglyChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_chordal_elimination
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal_peo_matrix
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal_mcs_seo
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal
   :project: graph_recognition


列挙
------------

既定の ``KIYOMI_EDGE_ADDITION`` は、Kiyomi の強弦グラフ部分グラフ列挙に
基づく専用逆探索である。空グラフを根とし、辺を 1 本ずつ追加する。非空な
強弦グラフ ``H`` の strong elimination ordering において、最初の非孤立頂点
``v`` と、順序上で最初の隣接頂点 ``w`` を選び、``H - vw`` を一意な親とする。
Kiyomi の Lemma 4.11 により、同じ ordering は辺 ``vw`` の削除後も strong
elimination ordering である。子候補 ``H + e`` は、強弦性を持ち、かつその親で
削除される辺が ``e`` の場合だけ探索する。このため、弦グラフ全体を生成して
フィルタすることはない。

親を一意にする strong elimination ordering は Farber の構成に従う。各消去段階
``i`` で閉近傍の真包含 ``N_i[x] ⊂ N_i[y]`` を部分順序へ累積し、その極小元で
ある simple vertex を選ぶ（複数なら最小ラベル）。任意の simple vertex を順に
除去するだけでは、強弦性を認識できても strong elimination ordering そのものに
ならない場合がある。

``LEGACY_CHORDAL_FILTER`` は従来の弦グラフ頂点追加木と強弦認識フィルタを
比較検証用に残したものである。``REVERSE_SEARCH`` は後方互換のための
``KIYOMI_EDGE_ADDITION`` の別名である。

原論文は strong elimination ordering を ``O(min(m log n, n^2))`` で構成し、
ホストグラフの辺数を ``M`` として 1 出力あたり
``O(M min(m log n, n^2))`` 時間、``O(n + M)`` 空間を示す。本実装は既存の
素朴な ``O(n^4)`` Farber 部分順序構成を候補辺ごとに再計算し、隣接行列と
完全な辺リストを使うため、原論文の時間・空間境界はそのまま適用されない。
callback API では出力全体を保持せず、探索状態を ``O(n^2)`` に抑える。

.. doxygenenum:: graph_recognition::StronglyChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_chordal_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_strongly_chordal_graphs_reverse_search_cb
   :project: graph_recognition


列挙数検証
----------------------------

``n = 1, 2, 3, 4, 5, 6`` のラベル付き強弦グラフ数
``1, 2, 8, 61, 822, 18034`` と一致すること、および同じ頂点数で旧方式と
集合が一致することをテストしている。また、``n <= 6`` の全単純グラフを
総当たりした認識結果とも集合比較している。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "strongly_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_strongly_chordal(g);

       std::cout << std::boolalpha << result.is_strongly_chordal << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "strongly_chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* M. Kiyomi. *Studies on Subgraph and Supergraph Enumeration Algorithms*.
  Ph.D. thesis, The Graduate University for Advanced Studies, 2006,
  Section 4.1.3.
  `PDF <https://www.nii.ac.jp/graduate/wp-content/themes/nii_original/assets/pdf/students_thesis/18/kiyomi_Dr_thesis.pdf>`_

* M. Farber. "Characterizations of strongly chordal graphs."
  *Discrete Mathematics*, 43(2--3):173--189, 1983.
  `DOI:10.1016/0012-365X(83)90154-1 <https://doi.org/10.1016/0012-365X(83)90154-1>`_

* E. Dahlhaus, P. Duchet. "On strongly chordal graphs."
  *Ars Combinatoria*, 24B:23--30, 1987.
