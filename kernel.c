typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;
typedef uint32_t size_t;

// リンカスクリプト内で定義されるシンボルを宣言
// シンボルのアドレスだけが必要なので適当にchar型にしている
extern char __bss[], __bss_end[], __stack_top[];

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

    // 無限ループ
    for (;;);
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