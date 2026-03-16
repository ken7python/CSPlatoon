# CSplatoon

ローカル2人対戦の簡易スプラトゥーン風ゲームです（ウィンドウタイトル: **C-Spraytoon**）。プレイヤー（円）と弾でフィールドのドットを自色に塗り、制限時間内により多くの面積を確保した方が勝利します。

**デプロイ先**

https://pages.kencode.jp/CSPlatoon


**特徴**
- 2人同時プレイ（同一画面 1280x720、120FPS）
- ドット塗りによる陣取り（5x5ピクセルのグリッド）
- 弾のヒットで相手が縮小、自分が拡大
- プレイヤー同士の衝突バウンド
- タイトル待機 → カウントダウン → ゲーム → リザルト の5シーン構成

**操作**
- プレイヤー1（青）
  - 移動: `W/A/S/D`
  - ショット: `F`
  - ゲームパッド(0番): 左スティック/十字キーで移動、`右側面右ボタン`でショット
- プレイヤー2（赤）
  - 移動: `↑/←/↓/→`
  - ショット: `K`
  - ゲームパッド(1番): 左スティック/十字キーで移動、`右側面右ボタン`でショット
- 共通
  - タイトル画面: 2人とも「スタート」ボタン領域に同時に乗ると開始
  - リザルト画面: `Space` でリスタート

**ルール**
- 60秒のタイム制
- プレイヤーや弾が触れたドットが自色に変わります
- 弾は相手に当たると相手が縮小（最小半径 12）、自分が拡大（最大半径 96）
- 弾の最大同時発射数: 5発/人
- 勝敗は塗り面積のパーセンテージで決定（同率の場合は引き分け）

**シーン構成**
1. タイトル待機: ロゴ・説明表示、2人がスタートボタンに乗るとフェードアウトして次へ
2. カウントダウン: 3・2・1・はじめ！の音声つきカウントダウン
3. ゲーム: 制限時間 60 秒。残り 10 秒以下でBGMのピッチが上がり、タイマー色が優勢プレイヤーの色に変わる
4. タイムアップ: 「タイムアップ!」表示
5. リザルト: 塗り割合（%）と勝者を表示。`Space` でリスタート

**ビルド & 実行**

### macOS（デスクトップ）

```bash
cmake -S . -B build
cmake --build build
open build/CSplatoon.app
```

または付属スクリプトを使用:
```bash
sh buildMac.sh
```

**実行時の注意**
- CMake ビルドでは `.app` の Resources に `assets/` が自動コピーされます。
- 単体バイナリを起動する場合は、`src/assets` を実行ディレクトリ直下に `assets/` として配置してください。

---

### Windows

#### 必要環境
- CMake 3.25 以上
- 以下のいずれか:
  - **MinGW-w64**（`gcc`/`g++` が PATH に通っていること）
  - **MSVC**（Visual Studio 2022 の「C++ によるデスクトップ開発」ワークロードと Developer Command Prompt）

#### 付属スクリプト（自動判別）

`gcc` が PATH にあれば MinGW、なければ NMake を自動選択します。

```bat
buildWin.bat
```

ツールチェーンを明示する場合:
```bat
buildWin.bat mingw    :: MinGW Makefiles を強制
buildWin.bat nmake    :: NMake Makefiles を強制（Developer Command Prompt で実行）
```

#### 手動ビルド — MinGW Makefiles

```bat
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build build
```

#### 手動ビルド — NMake Makefiles（Developer Command Prompt）

```bat
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

成果物は `build\` に生成されます:
- `CSplatoon.exe`
- `assets\`（自動コピー済み）

> **注意:** `WIN32` サブシステムが CMakeLists.txt で設定済みのためコンソールウィンドウは表示されません。デバッグ時は `-DCMAKE_BUILD_TYPE=Debug` を使ってください。

---

### Web（Emscripten）

#### 必要環境
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) をインストール・アクティベート済みであること

#### ビルド手順

```bash
emcmake cmake -S . -B build-web
cmake --build build-web
```

または付属スクリプトを使用（`build-web/` を一旦削除してからビルド）:
```bash
./buildWeb.sh
```

成果物は `build-web/` に生成されます:
- `index.html` / `index.js` / `index.wasm` / `index.data`

#### ローカルで動作確認

```bash
cd build-web
python3 -m http.server 8080
# ブラウザで http://localhost:8080 を開く
```

> **注意:** `file://` では SharedArrayBuffer の制限により動作しません。必ずローカルサーバー経由でアクセスしてください。

**依存関係**
- C++20
- CMake 3.25 以上
- raylib 5.5（CMake の FetchContent で自動取得）
- Web ビルドのみ: Emscripten SDK（`emcmake` が使えること）
- Windows ビルドのみ: Visual Studio 2022 または MinGW-w64

**ディレクトリ構成（抜粋）**
- `src/main.cpp`: ゲーム本体
- `src/assets/`: BGM/SE（opening.mp3, bgm.mp3, ending.mp3, coll.mp3, shot.mp3, damage.mp3, 321.mp3, start.mp3, end.mp3, Retry.mp3, redWinner.wav, blueWinner.wav, logo.png, font.ttf）
