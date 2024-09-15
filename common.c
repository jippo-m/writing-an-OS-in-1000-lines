#include "common.h"
// カーネル・ユーザーランド共通のコード用のファイル

// 最低限の機能に絞ったprintf
// %d, %x, %sに対応
void putchar(char ch);

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