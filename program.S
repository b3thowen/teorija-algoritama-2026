# autor: Ime Prezime INDEX raXXXX-20XX
.section .data
    ulaz_poruka: .ascii "Unesite string (mala slova a-z, max 30 karaktera, zavrsen sa Enter): \0"
    ulaz_poruka_len = . - ulaz_poruka

    izlaz_poruka: .ascii "Sortirani niz: \0"
    izlaz_poruka_len = . - izlaz_poruka

    novi_red: .byte 10

    unos_max = 32
    unos: .fill unos_max, 1, 0
    unos_len: .long 0        # duzina citanja (ukljucuje Enter)
    niz_duzina: .long 0      # stvarna duzina stringa bez Enter-a

.section .text
.globl main
main:
    # 1. Ispis poruke (sys_write - $4)
    movl $4, %eax
    movl $1, %ebx
    movl $ulaz_poruka, %ecx
    movl $ulaz_poruka_len, %edx
    int $0x80

    # 2. Ucitavanje unosa (sys_read - $3)
    movl $3, %eax
    movl $0, %ebx
    movl $unos, %ecx
    movl $unos_max, %edx
    int $0x80
    movl %eax, unos_len

    # 3. Pronalazenje stvarne duzine stringa (do znaka za novi red)
    movl $unos, %esi
    movl $0, %ecx           # brojac duzine

trazenje_duzine:
    movb (%esi), %al
    cmpb $10, %al
    je duzina_pronadjena
    incl %ecx
    incl %esi
    jmp trazenje_duzine

duzina_pronadjena:
    movl %ecx, niz_duzina

    # 4. Bubble sort niza (unos[0] .. unos[niz_duzina-1])
    movl niz_duzina, %ecx
    decl %ecx                  # broj prolaza = duzina - 1

spoljna_petlja:
    cmpl $0, %ecx
    je kraj_sortiranja

    movl %ecx, %edx             # broj poredjenja u ovom prolazu
    movl $unos, %esi             # pokazivac na pocetak niza

unutrasnja_petlja:
    cmpl $0, %edx
    je kraj_unutrasnje

    movb (%esi), %al
    movb 1(%esi), %ah
    cmpb %ah, %al
    jbe bez_zamene

    # zamena mesta susednih karaktera
    movb %al, 1(%esi)
    movb %ah, (%esi)

bez_zamene:
    incl %esi
    decl %edx
    jmp unutrasnja_petlja

kraj_unutrasnje:
    decl %ecx
    jmp spoljna_petlja

kraj_sortiranja:
    # 5. Ispis poruke i sortiranog niza (sys_write - $4)
    movl $4, %eax
    movl $1, %ebx
    movl $izlaz_poruka, %ecx
    movl $izlaz_poruka_len, %edx
    int $0x80

    movl $4, %eax
    movl $1, %ebx
    movl $unos, %ecx
    movl niz_duzina, %edx
    int $0x80

    movl $4, %eax
    movl $1, %ebx
    movl $novi_red, %ecx
    movl $1, %edx
    int $0x80

kraj:
    # 6. Izlaz iz programa (sys_exit - $1)
    movl $1, %eax
    movl $0, %ebx
    int $0x80
