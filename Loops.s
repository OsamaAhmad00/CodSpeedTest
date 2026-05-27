.intel_syntax noprefix

.globl aligned_to_64
.globl aligned_to_63
.globl aligned_to_31
.globl aligned_to_15

.text

aligned_to_64:
    xor rax, rax
    jl 1f
.align 64
1:
    inc rax
    cmp rax, rdi
    jl 1b
    ret

aligned_to_63:
    xor rax, rax
    jl 1f
.align 64
.rept 63
1:
nop
.endr
    inc rax
    cmp rax, rdi
    jl 1b
    ret

aligned_to_31:
    xor rax, rax
    jl 1f
.align 64
.rept 31
1:
nop
.endr
    inc rax
    cmp rax, rdi
    jl 1b
    ret

aligned_to_15:
    xor rax, rax
    jl 1f
.align 64
.rept 15
1:
nop
.endr
    inc rax
    cmp rax, rdi
    jl 1b
    ret