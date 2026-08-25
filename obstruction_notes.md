# NO 証明書 (obstruction) の抽出

38 クラスが NO 側の証明書を返す (規約は CLAUDE.md の「Result 構造体の規約」節)。共有語彙は
`include/forbidden_subgraph.h`、共有抽出器は `include/obstruction_extract.h`、
定義レベルの検証器は `tests/gtest/helpers/certificates.cpp` の
`verify_obstruction` と `python/tests/test_obstructions.py` の `verify`。
再実装時に踏み抜きやすい点のみ記録する (CLAUDE.md から移設)。

- **ホールに shortcut 処理は要らない**: 誘導部分グラフの最短路はそれ自体が
  誘導パス。BFS を `N[u] ∪ N[v]` の補集合に制限すれば、内部頂点は u,v へ弦を
  持たず、最短路なので自分自身にも弦を持たない。`hole_from_bfs_path` が
  この形を閉じる。逆に **ODD_HOLE を短絡してはいけない** (長さ 5 未満や
  三角形に潰れる)。chordless を要求しないのは ODD_CYCLE (bipartite) だけ。
- **PEO 失敗点は必ずホール上にある**: `verify_peo` の失敗 3 点組 (v, parent[v], u)
  に対する局所抽出は n<=7 の全グラフ・全 variant の NO 4438431 件すべてで成功した。
  汎用 `find_hole` フォールバックは保険であって想定経路ではない。
- **合成クラスは合成クラスの kind へ写像する**: 生の C6 は split の障害物でも
  trivially perfect の障害物でもない。長いホールの連続 4 頂点は誘導 P4、
  補グラフ側では同じ 4 頂点が G の C4 になる (`split_obstruction_from_hole` /
  `tp_obstruction_from_hole`)。split の証明書が常に `in_complement == false`
  なのはこのため。
- **FORCING_CYCLE は solver の伝播トレイルから取らない**: トレイルには推移律と
  確定済みクラスが混ざり、検証器が再生できない。純 Γ 関係だけを BFS する
  (`find_forcing_cycle`)。Γ は対称なので 1 回の探索が含意クラス全体を覆い、
  失敗した探索は到達した全アークを候補から外せる。
- **parity の証明書はパス 2 本**: 誤パリティの誘導パス 1 本では「どちらが誤りか」を
  検証器が判定できない。最短 u-v パス (誘導かつ d(u,v) のパリティ) を添える。
- **minor モデル抽出は memo の true を使えない**: `serialize` のキーは同型な状態を
  同一視するので、true のキャッシュは別の縮約履歴 (groups) に属し model を
  供給できない。false のキャッシュは構造だけの性質なので再利用してよい
  (`MinorChecker::find_model` は専用の dead_ を持つ)。
- **degree sequence だけで判定する variant は証明書を持てない**: threshold の
  FAST、split の HAMMER_SIMEONE、LR 平面性は指し示す頂点を持たない。
  `kind == NONE` + Doxygen 注記 + ビルダーで対応する。
- **biconnected の n < 3 は証明書なし**: K2 は連結でカット頂点も無く、
  「サイズ不足」以外に指すものが無い。
- **見送ったクラスと理由**: circular_arc / proper_circular_arc (使える有限禁止
  部分グラフ特徴付けが無い、Tucker は無限族)、trapezoid (2+2 証人は選んだ
  向き付けの B(P) 内にあり独立検証に certified poset が要る)、strongly_chordal
  (sun は任意サイズ)、proper_chordal (認識自体が factorial)、line_graph
  (Beineke 検出器が未実装で、失敗点が 9 個のどれかを特定しない)、circle
  (Naji の NO は GF(2) 上の階数の事実で組合せ証人にならない)、five_leaf_power
  (予算超過は「不明」であって NO ではない)。
