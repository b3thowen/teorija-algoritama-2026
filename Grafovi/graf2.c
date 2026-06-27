#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 100

// Struktura za stek (Stack)
typedef struct {
    int data[MAX_NODES];
    int top;
} Stack;

// Funkcije za rad sa stekom
void initStack(Stack *s) {
    s->top = -1;
}

bool isEmpty(Stack *s) {
    return s->top == -1;
}

void push(Stack *s, int value) {
    if (s->top < MAX_NODES - 1) {
        s->data[++(s->top)] = value;
    }
}

int pop(Stack *s) {
    if (!isEmpty(s)) {
        return s->data[(s->top)--];
    }
    return -1;
}

// Iterativna DFS funkcija koja koristi stek
void iterativniDFS(int graf[MAX_NODES][MAX_NODES], int n, int startniCvor) {
    bool posecen[MAX_NODES] = {false};
    Stack s;
    initStack(&s);

    // Postavljamo početni čvor na stek
    push(&s, startniCvor);

    printf("DFS obilazak: ");

    while (!isEmpty(&s)) {
        // Uzimamo čvor sa vrha steka
        int trenutni = pop(&s);

        // Čvor obrađujemo samo ako već nije posećen
        // (pošto se isti čvor može naći na steku više puta preko različitih grana)
        if (!posecen[trenutni]) {
            printf("%d ", trenutni);
            posecen[trenutni] = true;
        }

        // Prolazimo kroz sve susede trenutnog čvora.
        // Idemo unazad (od n-1 do 0) da bi se manji indeksi čvorova 
        // našli na vrhu steka i samim tim prvi obradili (standardno za DFS).
        for (int i = n - 1; i >= 0; i--) {
            if (graf[trenutni][i] == 1 && !posecen[i]) {
                push(&s, i);
            }
        }
    }
    printf("\n");
}

int main() {
    int n = 5; // Broj čvorova u grafu
    int graf[MAX_NODES][MAX_NODES] = {0};

    // Konstruišemo primer grafa
    // 0 -> 1, 0 -> 2
    // 1 -> 3
    // 2 -> 4
    graf[0][1] = 1;
    graf[0][2] = 1;
    graf[1][3] = 1;
    graf[2][4] = 1;

    int startni = 0;
    iterativniDFS(graf, n, startni);

    return 0;
}
