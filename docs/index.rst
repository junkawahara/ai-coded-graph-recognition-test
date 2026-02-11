Graph Recognition Library
=========================

グラフクラスの認識アルゴリズムを提供する C++11 ヘッダオンリーライブラリ。

.. toctree::
   :maxdepth: 2
   :caption: 目次

   usage
   api/index


概要
----

本ライブラリは、以下のグラフクラスの認識アルゴリズムを実装しています。
各認識関数は ``enum class`` パラメータによりアルゴリズムを選択でき、
デフォルト値が設定されているため省略も可能です。

**弦グラフ系**

* **弦グラフ** (chordal) -- MCS + PEO 検証
* **強弦グラフ** (strongly chordal) -- simple vertex 消去
* **弦二部グラフ** (chordal bipartite) -- 誘導偶閉路検査 / bisimplicial 辺消去
* **弱弦グラフ** (weakly chordal) -- G と補 G の誘導閉路検査
* **スプリットグラフ** (split) -- 次数列判定
* **閾値グラフ** (threshold) -- 次数列判定

**インターバルグラフ系**

* **インターバルグラフ** (interval) -- クリークパス探索 / AT-free 判定
* **固有インターバルグラフ** (proper interval) -- PQ 木
* **単位インターバルグラフ** (unit interval) -- 固有インターバル判定
* **余インターバルグラフ** (co-interval) -- 補グラフのインターバル判定
* **円弧グラフ** (circular-arc) -- 端点順序探索 + 2-SAT

**順列グラフ系**

* **順列グラフ** (permutation) -- 推移的向き付け / Gamma クラス分割
* **比較可能性グラフ** (comparability) -- モジュラー分解
* **余比較可能性グラフ** (co-comparability) -- 補グラフの比較可能性
* **二部順列グラフ** (bipartite permutation) -- 二部性 + 順列性

**二部グラフ系**

* **二部グラフ** (bipartite) -- BFS 2-彩色
* **チェーングラフ** (chain) -- 近傍包含判定
* **余チェーングラフ** (cochain) -- 補グラフのチェーン判定

**その他の完全グラフ系**

* **コグラフ** (cograph) -- 余木分解 / 分割細分化による高速版
* **ブロックグラフ** (block) -- DFS 二重連結成分分解
* **距離遺伝グラフ** (distance-hereditary) -- ツイン/ペンダント頂点除去
* **プトレマイオスグラフ** (ptolemaic) -- 弦性 + 距離遺伝性
* **自明完全グラフ** (trivially perfect) -- DFS 木判定
* **準閾値グラフ** (quasi-threshold) -- DFS 木判定
* **AT-free グラフ** -- 小惑星三つ組の非存在

**平面グラフ系**

* **平面グラフ** (planar) -- K5 / K3,3 マイナーチェック
* **外平面グラフ** (outerplanar) -- K4 / K2,3 マイナーチェック
* **カクタスグラフ** (cactus) -- DFS 二重連結成分分解
* **直並列グラフ** (series-parallel) -- 次数 2 以下の頂点除去

**列挙**

* **弦グラフ列挙** -- 逆探索 (reverse search)

**ユーティリティ**

* **余弦グラフ** (co-chordal) -- 補グラフの弦性判定
* **クリーク木** -- Kruskal 法 / インクリメンタル構築


特徴
----

* **C++11 互換** -- モダンコンパイラ不要
* **ヘッダオンリー** -- ``#include`` するだけで使用可能
* **1-indexed** -- 頂点番号は 1 から n
* **名前空間** -- ``graph_recognition`` に統一
* **アルゴリズム選択** -- ``enum class`` パラメータで切り替え可能


インデックス
------------

* :ref:`genindex`
