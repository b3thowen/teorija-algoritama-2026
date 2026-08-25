# autor: Ime Prezime INDEX raXXXX-20XX
.section .data
    input_msg: .ascii "Unesite string: \0"
    input_msg_len = . - input_msg

    izlaz_poruka: .ascii "Izlaz: \0"
    izlaz_poruka_len = . - izlaz_poruka

    novi_red: .byte 10

    unos_max = 32                  # 30 slova + '\n' + rezerva
    unos: .fill unos_max, 1, 0
    unos_len: .long 0

    izlaz_max = 31
    izlaz: .fill izlaz_max, 1, 0
    izlaz_len: .long 0

.section .text
.globl main
main:
    # 1. Ispis poruke input_msg (sys_write - $4)
    movl $4, %eax
    movl $1, %ebx
    movl $input_msg, %ecx
    movl $input_msg_len, %edx
    int $0x80

    # 2. Ucitavanje unosa (sys_read - $3)
    movl $3, %eax
    movl $0, %ebx
    movl $unos, %ecx
    movl $unos_max, %edx
    int $0x80
    movl %eax, unos_len

    # 3. Postavljanje pokazivaca i pocetne duzine izlaza
    movl $0, izlaz_len
    movl $unos, %esi
    movl $0, %ecx           # brojac obradjenih karaktera - cuva granicu bafera

petlja:
    # Edge case: zastita od prekoracenja bafera unos ako '\n' nedostaje
    cmpl $unos_max, %ecx
    jge kraj_unosa

    movb (%esi), %al

    # Edge case: prazan unos / kraj unosa
    cmpb $10, %al
    je kraj_unosa

    cmpb $'i', %al
    je obrni

    # dodavanje karaktera u izlazni bafer
    movl izlaz_len, %edx
    movl $izlaz, %edi
    addl %edx, %edi
    movb %al, (%edi)
    incl %edx
    movl %edx, izlaz_len

    incl %esi
    incl %ecx
    jmp petlja

obrni:
    movl izlaz_len, %edx     # broj karaktera trenutno u izlazu

    # Edge case: 'i' nailazi kad je izlaz prazan (npr. hipoteticki pocetak) - preskoci obrtanje
    cmpl $0, %edx
    je nastavi_posle_i

    movl $0, %edx             # levi indeks
    movl izlaz_len, %ebx
    decl %ebx                  # desni indeks = duzina - 1

rev_petlja:
    cmpl %ebx, %edx
    jge rev_kraj             # pokazivaci se sreli/presli - gotovo obrtanje

    movl $izlaz, %edi
    addl %edx, %edi           # edi = &izlaz[levi]
    movb (%edi), %al

    movl $izlaz, %edi
    addl %ebx, %edi           # edi = &izlaz[desni]  (koristimo isti pokazivac ponovo)
    movb (%edi), %ah

    movl $izlaz, %edi
    addl %edx, %edi
    movb %ah, (%edi)          # upisi desni karakter na levu poziciju

    movl $izlaz, %edi
    addl %ebx, %edi
    movb %al, (%edi)          # upisi levi karakter na desnu poziciju

    incl %edx
    decl %ebx
    jmp rev_petlja

rev_kraj:
nastavi_posle_i:
    incl %esi
    incl %ecx
    jmp petlja

kraj_unosa:
    # 4. Ispis poruke "Izlaz: "
    movl $4, %eax
    movl $1, %ebx
    movl $izlaz_poruka, %ecx
    movl $izlaz_poruka_len, %edx
    int $0x80

    # 5. Ispis obradjenog stringa (radi ispravno i kada je izlaz_len == 0)
    movl $4, %eax
    movl $1, %ebx
    movl $izlaz, %ecx
    movl izlaz_len, %edx
    int $0x80

    # 6. Ispis novog reda
    movl $4, %eax
    movl $1, %ebx
    movl $novi_red, %ecx
    movl $1, %edx
    int $0x80

kraj:
    # 7. Izlaz iz programa (sys_exit - $1)
    movl $1, %eax
    movl $0, %ebx
    int $0x80
