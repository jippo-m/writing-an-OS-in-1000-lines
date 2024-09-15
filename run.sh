#!/bin/bash

# setコマンドを使うことでシェルの設定の確認や変更ができる
# x: コマンドと引数の展開処理を表示
# u: 未定義の変数を参照するとエラーメッセージを表示
# e: コマンドが0以外のステータスで終了した場合、一部の場合を除いて即座に終了する
set -xue

# clangのパス
CC=clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"
# -ffreestanding: ホスト環境（開発環境）の標準ライブラリを使用しない
# -nostdlib:      リンカスクリプトを使用してリンクするため、標準ライブラリを使用しない

# カーネルをビルド
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf kernel.c
# -Wl: コンパイラではなくリンカにオプションを渡す
# -Wl,-Tkernel.ld: リンカスクリプトを指定
# -Wl,-Map=kernel.map: マップファイル（リンカによる配置結果）を出力


# QEMUのファイルパス
QEMU=qemu-system-riscv32

# QEMUを起動
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf

# -machine virt:     virtマシンとして起動する。
#                    ちなみに-machine '?'オプションで対応している環境を確認できる。
# -bios default:     デフォルトのBIOS (ここではOpenSBI) を使用する。
# -nographic:        QEMUをウィンドウなしで起動する。
# -serial mon:stdio: QEMUの標準入出力を仮想マシンのシリアルポートに接続する。
#                    mon:を指定することで、QEMUモニターへの切り替えも可能になる。
# --no-reboot:       仮想マシンがクラッシュしたら、再起動せずに停止させる (デバッグに便利)。

# -serial mon:stdioオプションを指定しているため、QEMUへの標準入出力が仮想マシンのシリアルポートに接続されている。
# ここで文字を入力すると、OSへ文字が送られることになる。
# ただ、現時点ではOSが起動しておらず、OpenSBIも入力を無視しているため、文字が表示されない。

# ctrl + A を押した直後にcと入力するとQEMUのデバッグコンソール（QEMUモニター）に移行する。
# モニター上でqコマンドを実行すると、QEMUを終了できる。
# C-a h    print this help
# C-a x    exit emulator
# C-a s    save disk data back to file (if -snapshot)
# C-a t    toggle console timestamps
# C-a b    send break (magic sysrq)
# C-a c    switch between console and monitor
# C-a C-a  sends C-a