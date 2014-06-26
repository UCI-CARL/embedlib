; delay code for dsPIC33FJ series

#include "board.def"

#if _MIPS_ < 5
#error "DELAYS: The delay library was not designed to work below 5MIPS!"
#endif

#if _MIPS_ > 60
#warning "DELAYS: The delay_ms() function is not designed to work above 60MIPS!"
#endif

.bss
.global _delay_us_ins_count
.global _delay_ms_ins_count
.align 2
_delay_us_ins_count: .space 2
_delay_ms_ins_count: .space 2

.text
.global _delay_init
_delay_init:
    mov #_MIPS_,w0
    sub w0,#5,w1
    mov w1,_delay_us_ins_count

    mov #1000,w1
    mul.uu w0,w1,w2
    sub w2,#5,w2
    mov w2,_delay_ms_ins_count
    return

.global _delay_us
_delay_us:
    mov _delay_us_ins_count,w1

_delay_us_loop:
    repeat w1
    nop

    dec w0,w0
    bra NZ,_delay_us_loop
    return

.global _delay_ms
_delay_ms:
    mov _delay_ms_ins_count,w1

_delay_ms_loop:
    repeat w1
    nop

    dec w0,w0
    bra NZ,_delay_ms_loop
    return


.end
