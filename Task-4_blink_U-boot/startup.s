.section .text._start
.global _start

_start:
    /* Set up stack pointer */
    ldr     x0, =_start
    add     sp, x0, #0x10000

    /* Zero out the .bss section */
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end

bss_loop:
    cmp     x0, x1
    b.ge    bss_done
    str     xzr, [x0], #8
    b       bss_loop

bss_done:
    /* Branch to main */
    bl      main

    /* Hang forever if main returns — never crash */
hang:
    wfe
    b       hang
