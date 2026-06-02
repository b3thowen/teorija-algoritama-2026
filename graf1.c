#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 100

// Struktura za red (Queue) koja je neophodna za BFS
typedef struct {
    int data[MAX_NODES];
    int front;
    int rear;
} Queue;

// Funkcije za rad sa redom
void initQueue(Queue *q) {
    q->front = 0;
    q->rear = 0;
}

bool isEmpty(Queue *q) {
    return q->front == q->rear;
}

void enqueue(Queue *q, int value) {
    if (q->rear < MAX_NODES) {
        q->data[q->rear++] = value;
    }
}

int dequeue(Queue *q) {
    if (!isEmpty(q)) {
        return q->data[q->front++];
    }
    return -1;
}

// Glavna funkcija za pronalaženje najkraćeg puta
int najkraciPut(int graf[MAX_NODES][MAX_NODES], int n, int s, int t) {
    // Ako su početni i krajnji čvor isti, dužina puta je 0
    if (s == t) return 0;

    // Niz koji prati posvećene čvorove
    bool posecen[MAX_NODES] = {false};
    
    // Niz koji čuva udaljenost od početnog čvora `s` do svakog čvora
    int udaljenost[MAX_NODES] = {0};

    Queue q;
    initQueue(&q);

    // Inicijalizacija početnog čvora
    enqueue(&q, s);
    posecen[s] = true;
    udaljenost[s] = 0;

    while (!isEmpty(&q)) {
        int trenutni = dequeue(&q);

        // Prolazimo kroz sve potencijalne susede trenutnog čvora
        for (int i = 0; i < n; i++) {
            // Ako postoji usmerena grana i čvor i nije posećen
            if (graf[trenutni][i] == 1 && !posecen[i]) {
                posecen[i] = true;
                udaljenost[i] = udaljenost[trenutni] + 1;
                enqueue(&q, i);

                // Ako smo stigli do ciljnog čvora `t`, vraćamo udaljenost
                if (i == t) {
                    return udaljenost[i];
                }
            }
        }
    }

    // Ako se petlja završi, a nismo stigli do `t`, put ne postoji
    return -1;
}

int main() {
    int n = 5; // Broj čvorova u grafu (indeksi od 0 do 4)
    
    // Inicijalizacija matrice susedstva (0 znači nema grane, 1 znači ima grane)
    int graf[MAX_NODES][MAX_NODES] = {0};

    graf[0][1] = 1;
    graf[0][2] = 1;
    graf[1][3] = 1;
    graf[2][3] = 1;
    graf[2][4] = 1;
    graf[3][4] = 1;

    int s = 0; // Početni čvor
    int t = 4; // Krajnji čvor

    int rezultat = najkraciPut(graf, n, s, t);

    if (rezultat != -1) {
        printf("Najkraca putanja od cvora %d do cvora %d ima %d grana.\n", s, t, rezultat);
    } else {
        printf("Putanja izmedju cvorova %d i %d ne postoji (Vrednost: %d).\n", s, t, rezultat);
    }

    return 0;
}
