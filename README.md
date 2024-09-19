# Writing an OS in 1000 lines

2024/9/15 ～

https://operating-system-in-1000-lines.vercel.app/ja/welcome

みかん本の前段階として一通りやってみる

## 機能

- マルチタスク: プロセスの切り替えを行い、複数のプログラムが同時に動作している (ように見せかける) 機能
- 例外ハンドラ: 実行時エラーなどの CPU が OS の介入を必要とするイベントを処理する機能
- ページング: 各プロセスごとの独立したメモリ空間を実現
- システムコール: アプリケーションから OS の機能を呼び出す機能
- デバイスドライバ: ディスクの読み書きなど、ハードウェアを操作する機能
- ファイルシステム: ディスク上のファイルを管理する機能
- コマンドラインシェル: 利用者がコマンドを入力して OS の機能を呼び出せる機能

## ファイル構造

```
├── disk/     - ファイルシステムの中身
├── common.c  - カーネル・ユーザー共通ライブラリ: printf関数やmemset関数など
├── common.h  - カーネル・ユーザー共通ライブラリ: 各種構造体・定数などの定義
├── kernel.c  - カーネル: プロセス管理、システムコール、デバイスドライバ、ファイルシステム
├── kernel.h  - カーネル: 各種構造体・定数などの定義
├── kernel.ld - カーネル: リンカスクリプト (メモリレイアウトの定義)
├── shell.c   - コマンドラインシェル
├── user.c    - ユーザー用ライブラリ: システムコールの呼び出し関数など
├── user.h    - ユーザー用ライブラリ: 各種構造体・定数などの定義
├── user.ld   - ユーザー: リンカスクリプト (メモリレイアウトの定義)
└── run.sh    - ビルドスクリプト
```
