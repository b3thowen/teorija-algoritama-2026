# autor: Ime Prezime INDEX raXXXX-20XX
.section .data
    ulaz_poruka: .ascii "Unesite string (proizvoljni karakteri, max 30, zavrsen sa Enter): \0"
    ulaz_poruka_len = . - ulaz_poruka

    izlaz_poruka: .ascii "Sortirani niz: \0"
    izlaz_poruka_len = . - izlaz_poruka

    prazno_poruka: .ascii "(prazan unos - nema sta da se sortira)\0"
    prazno_poruka_len = . - prazno_poruka

    novi_red: .byte 10

    unos_max = 32
    unos: .fill unos_max, 1, 0
    unos_len: .long 0        # duzina procitana preko sys_read
    niz_duzina: .long 0      # stvarna duzina niza (bez Enter-a, unutar granica bafera)

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

    # 3. Trazenje stvarne duzine niza - SA PROVEROM GRANICE BAFERA
    #    (edge case: pun bafer bez '\n' na kraju - ne sme se citati van unos_max)
    movl $unos, %esi
    movl $0, %ecx

trazenje_duzine:
    cmpl $unos_max, %ecx
    jge duzina_pronadjena       # dostignuta granica bafera - prekini pre citanja
    movb (%esi), %al
    cmpb $10, %al
    je duzina_pronadjena        # pronadjen kraj unosa (Enter)
    incl %ecx
    incl %esi
    jmp trazenje_duzine

duzina_pronadjena:
    movl %ecx, niz_duzina

    # Edge case: prazan unos (duzina == 0) - preskoci sortiranje i stampanje niza
    cmpl $0, niz_duzina
    je prazan_unos

    # Edge case: niz od tacno 1 karaktera - nema sta da se sortira
    movl niz_duzina, %ecx
    cmpl $2, %ecx
    jl kraj_sortiranja

    # 4. Bubble sort niza (unos[0] .. unos[niz_duzina-1])
    decl %ecx                   # broj prolaza = duzina - 1

spoljna_petlja:
    cmpl $0, %ecx
    je kraj_sortiranja

    movl %ecx, %edx              # broj poredjenja u ovom prolazu
    movl $unos, %esi              # pokazivac na pocetak niza

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

    jmp kraj

prazan_unos:
    # Poseban ispis za edge case praznog unosa
    movl $4, %eax
    movl $1, %ebx
    movl $prazno_poruka, %ecx
    movl $prazno_poruka_len, %edx
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
