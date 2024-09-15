#include "kernel.h"
#include "common.h"

typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;
typedef uint32_t size_t;

// リンカスクリプト内で定義されるシンボルを宣言
// シンボルのアドレスだけが必要なので適当にchar型にしている
extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid,  long eid) {
    // 指定したレジスタに値を入れるようにコンパイラに指示
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a7 __asm__("a6") = fid;
    register long a6 __asm__("a7") = eid;

    // CPUの実行モードをカーネル用（S-Mode）からOpenSBI用（M-Mode）に切り替え、
    // OpenSBIの処理ハンドラを呼び出す
    // OpenSBIの処理が終わると、再びカーネル用のモードに切り替わる
    __asm__ __volatile__(
        "ecall"
        : "=r"(a0), "=r"(a1)
        : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        : "memory"
    );
    return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *)buf;
    while (n--) {
        *p++ = c;
    }
    return buf;
}

void kernel_main() {
    // bss領域を0で初期化
    // ブートローダが.bss領域を認識してゼロクリアしてくれることもあるが、
    // その確証がないので自分で初期化するのが無難
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    const char *s = "\n\nHello, World!\n";
    for (int i = 0; s[i] != '\0'; i++) {
        putchar(s[i]);
    }
    printf("Hello, %s\n", "World!");

    // 無限ループ
    for (;;) {
        // wait for interrupt
        __asm__ __volatile__ ("wfi");
    };
}

__attribute__((section(".text.boot")))
// 関数の本文の前後に余計なコード（関数プロローグ、エピローグ）を
// 生成しないようにコンパイラに指示
__attribute__((naked))
// 最初に呼ばれる関数
void boot(void) {
    __asm__ __volatile__ (
        // リンカスクリプトで用意したスタック領域の末尾アドレスをスタックポインタに設定
        "mv sp, %[stack_top]\n"
        // kernel_main関数を呼び出す
        "j kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
    );
}