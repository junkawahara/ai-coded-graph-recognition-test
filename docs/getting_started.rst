はじめに
================

インストール
--------------------

本ライブラリはヘッダオンリー。``include/`` ディレクトリをプロジェクトにクローンまたはコピーするだけで使える:

.. code-block:: bash

   git clone https://github.com/junkawahara/ai-coded-graph-recognition-test.git
   # コンパイラフラグに -Ipath/to/include を追加

ライブラリ本体にビルド作業は不要。
リポジトリの ``Makefile`` は CLI 実行ファイルとテストをビルドする:

.. code-block:: bash

   make          # 全 CLI ターゲットをビルド
   make clean    # バイナリを削除


C++ クイックスタート
----------------------------

.. code-block:: cpp

   #include "graph.h"
   #include "interval.h"
   #include <iostream>

   int main() {
       using namespace graph_recognition;
       Graph g = Graph::read(std::cin);
       IntervalResult res = check_interval(g);
       if (res.is_interval) {
           std::cout << "YES" << std::endl;
           for (int v = 1; v <= g.n; ++v) {
               std::cout << v << ": ["
                         << res.intervals[v].first << ", "
                         << res.intervals[v].second << "]"
                         << std::endl;
           }
       } else {
           std::cout << "NO" << std::endl;
       }
       return 0;
   }


Python クイックスタート
------------------------------

.. code-block:: bash

   pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

.. code-block:: python

   from graph_recognition import is_interval, is_chordal

   is_interval(4, [(1, 2), (2, 3), (3, 4)])    # True
   is_interval(4, [(1, 2), (2, 3), (3, 4), (4, 1)])  # False

   is_chordal(4, [(1, 2), (2, 3), (3, 4)])      # True


次のステップ
--------------------

* :doc:`usage` -- 入出力形式、アルゴリズム選択、テスト
* :doc:`families/index` -- グラフクラス別 API リファレンス (認識 + 列挙)
* :doc:`python` -- Python API リファレンス
