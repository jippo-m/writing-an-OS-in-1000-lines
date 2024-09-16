#pragma once

// カーネルパニックは、カーネルが続行不能なエラーに遭遇したときに発生する
// GoやRustでいうpanicに似た概念
// バックスラッシュで行末を継続させる必要があるので注意
// __FILE__と__LINE__はそれぞれコンパイル時のファイル名と行番号を示すマクロ
// 複数の文からなるマクロはdo-whileループで包む
#define PANIC(fmt, ...)\
    do {\
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);\
        while (1) {}  \
    } while (0)

// SBIの処理結果を返す構造体
struct sbiret {
    long error;
    long value;
};