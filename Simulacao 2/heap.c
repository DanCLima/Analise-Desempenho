#include <stdio.h>

//---------------------
#define MAXN 1000100

int heap[MAXN];
int tamanho_heap;
//---------------------

int pai(int i) {
    return i / 2;
}

int esquerda(int i) {
    return 2 * i;
}

int direita(int i) {
    return 2 * i + 1;
}

void heapify_up(int v) {
    if (v == 1) return;

    int p = pai(v);
    if (heap[v] < heap[p]) {
        int temp = heap[v];
        heap[v] = heap[p];
        heap[p] = temp;

        heapify_up(p);
    }
}

void heapify_down(int v) {
    int d = direita(v);
    int e = esquerda(v);

    int menor = v;

    if (d <= tamanho_heap && heap[d] < heap[menor]) menor = d;
    if (e <= tamanho_heap && heap[e] < heap[menor]) menor = e;

    if (menor != v) {
        int temp = heap[v];
        heap[v] = heap[menor];
        heap[menor] = temp;

        heapify_down(menor);
    }
}

void insere(int valor) {
    heap[++tamanho_heap] = valor;

    heapify_up(tamanho_heap);
}

void deleta(int posicao) {
    int temp = heap[posicao];
    heap[posicao] = heap[tamanho_heap];
    heap[tamanho_heap] = temp;

    tamanho_heap--;

    heapify_down(posicao);
}

int main() {
    int n;
    scanf("%d", &n);

    for (int i = 1; i <= n; i++) {
        char operacao;
        scanf(" %c", &operacao);

        if (operacao == 'I') { // inserir um número
            int valor;
            scanf("%d", &valor);

            insere(valor);
        }

        if (operacao == 'D') {
            printf("%d\n", heap[1]); // imprime o valor do topo
            deleta(1);
        }
    }

    return 0;
}
