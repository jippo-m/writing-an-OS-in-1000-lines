#pragma once

// va_listなどはC標準ライブラリの<stdarg.h>に定義されているマクロだが、
// 今回は標準ライブラリに頼らずに自前で用意する。
// __builtin_がついたものはコンパイラ(clang)が用意してくれる
#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg
#define PAGE_SIZE 4096

typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;
typedef uint32_t paddr_t; // physical address
typedef uint32_t vaddr_t; // virtual address 標準ライブラリでいうuintptr_t

#define true  1
#define false 0
#define NULL  ((void *)0)
#define align_up(value, align)   __builtin_align_up(value, align)
#define is_aligned(value, align) __builtin_is_aligned(value, align)
// 構造体のメンバのオフセット（メンバが構造体の先頭から何バイト目にあるか）
#define offsetof(type, member)   __builtin_offsetof(type, member)

void *memset(void *buf, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
void printf(const char *fmt, ...);