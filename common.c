#include "common.h"
// カーネル・ユーザーランド共通のコード用のファイル

void putchar(char ch);

// bufの先頭からnバイト分をcで埋める
void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *)buf;
    while (n--) {
        // *p = c; p++;
        *p++ = c;
    }
    return buf;
}

// dstの先頭からnバイト分をsrcからコピーする
void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *) dst;
    const uint8_t *s = (const uint8_t *) src;
    while (n--) {
        // *d = *s; d++; s++;
        *d++ = *s++;
    }
    return dst;
}

char *strcpy(char *dst, const char *src) {
    char *d = dst;
    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';
    return dst;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            break;
        }
        s1++;
        s2++;
    }
    return *(unsigned char *) s1 - *(unsigned char *) s2;
}

// 最低限の機能に絞ったprintf
// %d, %x, %sに対応
void printf(const char *fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            // %の次の文字によって処理を分ける
            switch (*fmt) {
                case '\0':
                    putchar('%');
                    goto end;
                case '%': // %% -> %
                    putchar('%');
                    break;

                case 'd': {
                    // case文の中で変数の宣言をするためには{}で囲む（スコープを変える）必要がある
                    int value = va_arg(vargs, int);
                    if (value < 0) {
                        putchar('-');
                        value = -value;
                    }

                    // valueの最上位の桁を求めるために桁数を求める
                    int divisor = 1;
                    while (value / divisor >= 10) divisor *= 10;
                    while (divisor > 0) {
                        // 最上位の桁から順に出力
                        putchar('0' + value / divisor);
                        value %= divisor;
                        divisor /= 10;
                    }
                    break;
                }
                case 'x': {
                    int value = va_arg(vargs, int);
                    for (int i = 7; i >= 0; i--) {
                        int nibble = (value >> (i * 4)) & 0xf;
                        putchar("0123456789abcdef"[nibble]);
                    }
                    break;
                }
                case 's': {
                    const char *s = va_arg(vargs, const char *);
                    while (*s) {
                        putchar(*s);
                        s++;
                    }
                    break;
                }
            }
        } else {
            // %でない場合はそのまま出力
            putchar(*fmt);
        }

        fmt++;
    }

end:
    va_end(vargs);
}