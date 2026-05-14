# unsigned int maska(unsigned int n, unsigned int v)
# n = 8(%ebp), v = 12(%ebp)
# n-ti bit = v, svi ostali bitovi = NOT v

.globl maska
maska:
    pushl   %ebp
    movl    %esp, %ebp

    movl    8(%ebp), %ecx      # n
    movl    12(%ebp), %edx     # v

    # validacija n: n > 31 => neispravno
    cmpl    $31, %ecx
    ja      greska

    # validacija v: v > 1 => neispravno
    cmpl    $1, %edx
    ja      greska

    # provjera: da li je v = 1 ili v = 0
    cmpl    $1, %edx
    je      v_je_jedan

v_je_nula:
    # v = 0: svi bitovi treba da budu 1, osim n-tog koji je 0
    # postavljamo 1 na bit poziciju n, pa invertujemo sve bitove
    movl    $1, %eax
    shll    %cl, %eax          # pomjeramo 1 na n-tu poziciju
    notl    %eax               # invertujemo => n-ti bit = 0, ostali = 1
    jmp     kraj

v_je_jedan:
    # v = 1: svi bitovi treba da budu 0, osim n-tog koji je 1
    # postavljamo 1 na bit poziciju n
    movl    $1, %eax
    shll    %cl, %eax          # pomjeramo 1 na n-tu poziciju => n-ti bit = 1, ostali = 0
    jmp     kraj

greska:
    movl    $0, %eax

kraj:
    popl    %ebp
    ret