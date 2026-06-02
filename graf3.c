#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 100

// Struktura za stek
typedef struct {
    int data[MAX_NODES * MAX_NODES];
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
    if (s->top < MAX_NODES * MAX_NODES - 1) {
        s->data[++(s->top)] = value;
    }
}

int pop(Stack *s) {
    if (!isEmpty(s)) {
        return s->data[(s->top)--];
    }
    return -1;
}

int peek(Stack *s) {
    if (!isEmpty(s)) {
        return s->data[s->top];
    }
    return -1;
}

// Funkcija koja pronalazi i štampa Ojlerovu putanju/ciklus
void pronadjiOjlerovuPutanju(int graf[MAX_NODES][MAX_NODES], int n) {

    // Računanje stepena čvorova
    int stepen[MAX_NODES] = {0};

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (graf[i][j] == 1) {
                stepen[i]++;
            }
        }
    }

    // Provera uslova za postojanje Ojlerove putanje
    int brojNeparnih = 0;

    for (int i = 0; i < n; i++) {
        if (stepen[i] % 2 != 0) {
            brojNeparnih++;
        }
    }

    if (brojNeparnih != 0 && brojNeparnih != 2) {
        printf("Ojlerova putanja ne postoji.\n");
        return;
    }

    // Određivanje početnog čvora
    int startniCvor = 0;

    if (brojNeparnih == 2) {
        // Počinje se iz jednog od neparnih čvorova
        for (int i = 0; i < n; i++) {
            if (stepen[i] % 2 != 0) {
                startniCvor = i;
                break;
            }
        }
    } else {
        // Za ciklus uzimamo prvi čvor koji ima granu
        for (int i = 0; i < n; i++) {
            if (stepen[i] > 0) {
                startniCvor = i;
                break;
            }
        }
    }

    Stack trenutnaPutanja;
    Stack krajnjaPutanja;

    initStack(&trenutnaPutanja);
    initStack(&krajnjaPutanja);

    push(&trenutnaPutanja, startniCvor);

    // Kopija grafa jer brišemo iskorišćene grane
    int preostaleGrane[MAX_NODES][MAX_NODES];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            preostaleGrane[i][j] = graf[i][j];
        }
    }

    // Hierholzerov algoritam
    while (!isEmpty(&trenutnaPutanja)) {

        int u = peek(&trenutnaPutanja);

        int v = -1;

        for (int i = 0; i < n; i++) {
            if (preostaleGrane[u][i] == 1) {
                v = i;
                break;
            }
        }

        if (v != -1) {
            push(&trenutnaPutanja, v);

            preostaleGrane[u][v] = 0;
            preostaleGrane[v][u] = 0;
        } else {
            pop(&trenutnaPutanja);
            push(&krajnjaPutanja, u);
        }
    }

    printf("Ojlerova putanja/ciklus:\n");

    while (!isEmpty(&krajnjaPutanja)) {
        printf("%d", pop(&krajnjaPutanja));

        if (!isEmpty(&krajnjaPutanja)) {
            printf(" -> ");
        }
    }

    printf("\n");
}

int main() {

    int n = 5;

    int graf[MAX_NODES][MAX_NODES] = {0};

    // Primer grafa
    graf[0][1] = 1; graf[1][0] = 1;
    graf[0][2] = 1; graf[2][0] = 1;
    graf[0][3] = 1; graf[3][0] = 1;
    graf[1][2] = 1; graf[2][1] = 1;
    graf[1][4] = 1; graf[4][1] = 1;
    graf[2][3] = 1; graf[3][2] = 1;
    graf[2][4] = 1; graf[4][2] = 1;
    graf[3][4] = 1; graf[4][3] = 1;

    pronadjiOjlerovuPutanju(graf, n);

    return 0;
}