#include "kernel.h"
#include "common.h"

typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;
typedef uint32_t size_t;
void kernel_entry(void);

// リンカスクリプト内で定義されるシンボルを宣言
// シンボルのアドレスだけが必要なので適当にchar型にしている
extern char __bss[], __bss_end[], __stack_top[], __free_ram[], __free_ram_end[];

// nページ分のメモリを動的に割り当てて、その先頭アドレスを返す
// BumpアロケータやLinearアロケータと呼ばれる割り当てアルゴリズム。
// 解法処理が必要ない場面で実際に使われている
paddr_t alloc_pages(uint32_t n) {
    // next_paddrは関数呼び出し間で値が保持される。
    // 次に割り当てられる領域（空いている領域）の先頭アドレスを指す。
    // 初期値は__free_ramのアドレス（__free_ram）から順にメモリを割り当てていく
    static paddr_t next_paddr = (paddr_t) __free_ram;
    paddr_t paddr = next_paddr;
    next_paddr += PAGE_SIZE * n;
    
    // malloc()がNULLを返すのと同じように0を返すこともできるが、
    // 今回はわかりやすさのためパニックさせる
    if (next_paddr > (paddr_t) __free_ram_end)
        PANIC("out of memory");

    // 割り当てたメモリをゼロクリア
    memset((void *) paddr, 0, PAGE_SIZE * n);

    // __free_ramはリンカスクリプトのALIGN(4096)により4KB境界に配置される
    // つまりalloc_pages関数も必ず4KBでアラインされたアドレスを返す
    return paddr;
}

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

void kernel_main() {
    // bss領域を0で初期化
    // ブートローダが.bss領域を認識してゼロクリアしてくれることもあるが、
    // その確証がないので自分で初期化するのが無難
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    paddr_t paddr0 = alloc_pages(2);
    paddr_t paddr1 = alloc_pages(1);
    printf("alloc_pages test: paddr0 = %x\n", paddr0);
    printf("alloc_pages test: paddr1 = %x\n", paddr1);
    // alloc_pages test: paddr0 = 80221000
    // alloc_pages test: paddr1 = 80223000

    PANIC("booted!");
    // printf("unreachable here!\n");
    WRITE_CSR(stvec, (uint32_t) kernel_entry);
    __asm__ __volatile__("unimp"); // 無効な命令

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

__attribute__((aligned(4)))
void kernel_entry(void) {
    __asm__ __volatile__(
        // sscratchレジスタに例外発生時のspを保存
        "csrw sscratch, sp\n"
        "addi sp, sp, -4 * 31\n"
        "sw ra, 4 * 0(sp)\n"
        "sw gp, 4 * 1(sp)\n"
        "sw tp, 4 * 2(sp)\n"
        "sw t0, 4 * 3(sp)\n"
        "sw t1, 4 * 4(sp)\n"
        "sw t2, 4 * 5(sp)\n"
        "sw t3, 4 * 6(sp)\n"
        "sw t4, 4 * 7(sp)\n"
        "sw t5, 4 * 8(sp)\n"
        "sw t6, 4 * 9(sp)\n"
        "sw a0, 4 * 10(sp)\n"
        "sw a1, 4 * 11(sp)\n"
        "sw a2, 4 * 12(sp)\n"
        "sw a3, 4 * 13(sp)\n"
        "sw a4, 4 * 14(sp)\n"
        "sw a5, 4 * 15(sp)\n"
        "sw a6, 4 * 16(sp)\n"
        "sw a7, 4 * 17(sp)\n"
        "sw s0, 4 * 18(sp)\n"
        "sw s1, 4 * 19(sp)\n"
        "sw s2, 4 * 20(sp)\n"
        "sw s3, 4 * 21(sp)\n"
        "sw s4, 4 * 22(sp)\n"
        "sw s5, 4 * 23(sp)\n"
        "sw s6, 4 * 24(sp)\n"
        "sw s7, 4 * 25(sp)\n"
        "sw s8, 4 * 26(sp)\n"
        "sw s9, 4 * 27(sp)\n"
        "sw s10, 4 * 28(sp)\n"
        "sw s11, 4 * 29(sp)\n"

        "csrr a0, sscratch\n"
        "sw a0, 4 * 30(sp)\n"

        "mv a0, sp\n"
        "call handle_trap\n"

        "lw ra, 4 * 0(sp)\n"
        "lw gp, 4 * 1(sp)\n"
        "lw tp, 4 * 2(sp)\n"
        "lw t0, 4 * 3(sp)\n"
        "lw t1, 4 * 4(sp)\n"
        "lw t2, 4 * 5(sp)\n"
        "lw t3, 4 * 6(sp)\n"
        "lw t4, 4 * 7(sp)\n"
        "lw t5, 4 * 8(sp)\n"
        "lw t6, 4 * 9(sp)\n"
        "lw a0, 4 * 10(sp)\n"
        "lw a1, 4 * 11(sp)\n"
        "lw a2, 4 * 12(sp)\n"
        "lw a3, 4 * 13(sp)\n"
        "lw a4, 4 * 14(sp)\n"
        "lw a5, 4 * 15(sp)\n"
        "lw a6, 4 * 16(sp)\n"
        "lw a7, 4 * 17(sp)\n"
        "lw s0, 4 * 18(sp)\n"
        "lw s1, 4 * 19(sp)\n"
        "lw s2, 4 * 20(sp)\n"
        "lw s3, 4 * 21(sp)\n"
        "lw s4, 4 * 22(sp)\n"
        "lw s5, 4 * 23(sp)\n"
        "lw s6, 4 * 24(sp)\n"
        "lw s7, 4 * 25(sp)\n"
        "lw s8, 4 * 26(sp)\n"
        "lw s9, 4 * 27(sp)\n"
        "lw s10, 4 * 28(sp)\n"
        "lw s11, 4 * 29(sp)\n"
        "lw sp, 4 * 30(sp)\n"
        "sret\n"
    );
}

void handle_trap(struct trap_frame *f) {
    uint32_t scause = READ_CSR(scause);
    uint32_t stval = READ_CSR(stval);
    uint32_t user_pc = READ_CSR(sepc);

    PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
}


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