/* リンカスクリプト、調べてもあんまり出てこないしふわっとしか理解できてない */

/* カーネルのエントリポイントはboot関数 */
ENTRY(boot)

SECTIONS {
    /* ベースアドレスは0x80200000 */
    . = 0x80200000;

    /* カーネルが利用するアドレスの先頭。
       カーネルのマッピングは、カーネルの仮想アドレスと物理アドレスが一致するように設定する */
    __kernel_base = .;

    /* .は現在のアドレスを表す変数のようなもの。
       *(.text)等でデータが配置されるたびに自動的に加算されていく。
       ALIGN(4)は「4バイト境界になるようにアドレスを調整する」という意味 */

    .text :{
        /* 必ず.text.bootセクションを先頭にする */
        KEEP(*(.text.boot));
        /* 全ファイル中(*)の.textと.textで始まる名前のセクションをそこに配置 */
        *(.text .text.*);
    }

    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    .data : ALIGN(4) {
        *(.data .data.*);
    }

    .bss : ALIGN(4) {
        /* __bssというシンボルに現在のアドレスを割り当てる
           シンボルは関数や静的変数を表すもので、
           C言語では`extern char シンボル名`で定義したシンボルを参照できる */
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        /* .bssセクションの末尾に、ブート時に使うスタック領域を配置 */
        __bss_end = .;
    }

    . = ALIGN(4);
    . += 128 * 1024; /* 128KiB */
    __stack_top = .;

    /* 動的に割り当てるメモリ領域(free_ram ～ free_ram_end) */
    /* アドレスを決め打ちで定義せずにリンカスクリプト上に定義することで、
       カーネルの静的データと被らないようにリンカが位置を決定してくれる */
    /* 実用的なOSでは、このようにデバイスごとにメモリサイズを決め打ちで定義する場合の他に、
       起動時にハードウェアから利用可能なメモリ領域の情報を取得して決定することもある（例：UEFIのGetMemoryMap） */
    . = ALIGN(4096); /* 4KiB境界 */
    __free_ram = .;
    . += 64 * 1024 * 1024; /* 64MiB */
    __free_ram_end = .;
}