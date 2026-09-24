平面グラフ
====================

辺の交差なしに平面に描画できるグラフを **平面グラフ** という。
Wagner の定理により、グラフが平面的であることと、
K\ :sub:`5` マイナーも K\ :sub:`3,3` マイナーも含まないことは同値である。
(Kuratowski の定理は、マイナーの代わりに細分を用いた同値な特徴付けを与える。)

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_43.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PlanarAlgorithm``
     - 説明
   * - ``LEFT_RIGHT`` **(既定)**
     - 辺数上界 (3n - 6) の事前検査の後、de Fraysseix, Ossona de Mendez,
       Rosenstiehl の left-right 平面性判定条件 (競合ペアのスタックを用いた
       2 回の DFS) を適用する。計算量は O(n + m log Δ) (2 回の DFS は線形だが、隣接リストを nesting depth で
       比較ソートする分だけ線形を超える)。
   * - ``MINOR_CHECK``
     - 辺数上界 (3n - 6) の事前検査の後、K\ :sub:`5` と K\ :sub:`3,3` のマイナーをバックトラッキングで探索する。
       正確だが最悪の場合は指数時間であり、小さなグラフのクロスチェック用として残している。

.. doxygenenum:: graph_recognition::PlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_planar
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::PlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_planar_labeled_graphs_reverse_search
   :project: graph_recognition

両列挙器の既定は ``PLANTRI`` です。Brinkmann--McKay (2007) §4 に従い、
単純三角形分割から三角形に接する非橋を削除します。正準 ear 追加による
親判定と兄弟の埋め込みコードで、鏡映も同一視して平面地図を1回ずつ生成します。
面境界が同じ頂点を複数回通る場合にも対応し、橋・関節点・2頂点切断を含めます。
抽象正準形で異なる埋め込みの同型重複を出力時に除きますが、その子孫は探索します。
連結成分の多重集合で孤立点・同型成分も合成し、ラベル付き版は全ラベルを展開します。

従来の頂点追加方式とその出力順・代表元は、ラベル付きの ``REVERSE_SEARCH`` と
ラベルなしの ``CANONICAL_AUGMENTATION`` で選べます。CLI の ``--algo`` と
Python の ``algorithm`` は ``plantri`` または各従来方式の小文字名を受け付けます。
``connected_only`` はラベルなし版だけで、従来の位置引数も有効です。
C++・CLI の上限は新方式が63頂点、旧方式がラベル付き64・ラベルなし63頂点です。
Python はラベル付き1〜6・ラベルなし1〜8頂点です。C++・CLI の n=0 は
連結のみの指定も含め、両方式とも空グラフ1件を返します。

新方式は平面性認識器や外部 plantri を呼びません。埋め込み1件あたり O(n) 候補、
候補あたり O(n²) 時間、探索領域 O(n³) に加え、最悪階乗時間の抽象正準化と、
正準形集合・成分代表・全出力・ラベル展開の訪問済み集合を保持します。
同じ抽象グラフに複数の埋め込みがあり、抽象グラフあたりの多項式遅延や
plantri 本体の速度・空間保証は主張しません。ラベルなしの個数は OEIS
A005470(n) (1, 2, 4, 11, 33, 142, 822, 6966, 79853, ...)、
``connected_only`` を指定した場合はそのうち連結なもの (A003094: 1, 1, 2,
6, 20, 99, 646, 5974, ...) です。

.. doxygenenum:: graph_recognition::PlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_planar_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付き平面グラフの個数が
`OEIS A066537 <https://oeis.org/A066537>`_ と一致することを検証した:
``1, 2, 8, 64, 1023``。

従来の非同型列挙については ``n = 9`` まで
`OEIS A005470 <https://oeis.org/A005470>`_ の
``1, 2, 4, 11, 33, 142, 822, 6966, 79853`` と一致することを検証した。
``n = 8`` までは列挙器自身とは独立にも検証した (``n`` 頂点の非同型グラフを
すべて生成して ``check_planar`` で絞り込んでも同じ個数が得られる)。
``connected_only`` を指定した場合の個数は
`OEIS A003094 <https://oeis.org/A003094>`_ の
``1, 1, 2, 6, 20, 99, 646, 5974`` と一致する。静的テストケースは ``n = 8``
までである。

新方式はラベル付き n=6、ラベルなし n=7 まで旧方式との出力集合一致を検証した。
n=8 では抽象グラフ6,966件（連結5,974件）と、論文 Table 19 の埋め込み
31,178件を区別し、埋め込みの辺数別件数も検証した。n=7 までは全非橋削除の
閉包と比較し、三角形に接する辺だけの候補生成と正準親判定の完全性を確認した。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_planar(g);

       std::cout << std::boolalpha << result.is_planar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_planar_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 142 = A005470(6)
       return 0;
   }


参考文献
------------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_

* K. Wagner. "Über eine Eigenschaft der ebenen Komplexe."
  *Mathematische Annalen*, 114(1):570--590, 1937.
  `DOI:10.1007/BF01594196 <https://doi.org/10.1007/BF01594196>`_

* J. Hopcroft, R. Tarjan. "Efficient planarity testing."
  *Journal of the ACM*, 21(4):549--568, 1974.
  `DOI:10.1145/321850.321852 <https://doi.org/10.1145/321850.321852>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
