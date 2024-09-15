#pragma once

// va_listなどはC標準ライブラリの<stdarg.h>に定義されているマクロだが、
// 今回は標準ライブラリに頼らずに自前で用意する。
// __builtin_がついたものはコンパイラ(clang)が用意してくれる
#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

void printf(const char *fmt, ...);