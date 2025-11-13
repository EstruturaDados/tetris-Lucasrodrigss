#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TAM_FILA 5
#define TAM_PILHA 3

// -------------------------------
// STRUCT DA PEÇA
// -------------------------------
typedef struct {
    int id;
    char tipo;
} Peca;

// -------------------------------
// FILA CIRCULAR
// -------------------------------
typedef struct {
    Peca itens[TAM_FILA];
    int inicio;
    int fim;
    int qtd;
} Fila;

// -------------------------------
// PILHA
// -------------------------------
typedef struct {
    Peca itens[TAM_PILHA];
    int topo;
} Pilha;

// -------------------------------
// PARA DESFAZER (UNDO)
// -------------------------------
typedef struct {
    int houveJogada;
    Peca ultimaPecaJugada;
} Historico;


// -------------------------------
// GERAR PEÇA AUTOMATICAMENTE
// -------------------------------
char tipos[] = {'I','O','T','L','J','S','Z'};
int nextID = 1;

Peca gerarPeca() {
    Peca p;
    p.id = nextID++;
    p.tipo = tipos[rand() % 7];
    return p;
}

// -------------------------------
// FILA: INICIALIZAÇÃO
// -------------------------------
void inicializarFila(Fila *f) {
    f->inicio = 0;
    f->fim = 0;
    f->qtd = 0;

    for(int i = 0; i < TAM_FILA; i++) {
        f->itens[f->fim] = gerarPeca();
        f->fim = (f->fim + 1) % TAM_FILA;
        f->qtd++;
    }
}

// -------------------------------
// FILA: ENQUEUE
// -------------------------------
void inserirFila(Fila *f, Peca p) {
    if(f->qtd == TAM_FILA) return;
    f->itens[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->qtd++;
}

// -------------------------------
// FILA: DEQUEUE
// -------------------------------
Peca removerFila(Fila *f) {
    Peca removida = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->qtd--;
    return removida;
}

// -------------------------------
// PILHA: INICIALIZAÇÃO
// -------------------------------
void inicializarPilha(Pilha *p) {
    p->topo = -1;
}

// -------------------------------
// PILHA: PUSH
// -------------------------------
void push(Pilha *p, Peca x) {
    if(p->topo == TAM_PILHA - 1) {
        printf("Pilha cheia! Não é possível reservar.\n");
        return;
    }
    p->itens[++p->topo] = x;
}

// -------------------------------
// PILHA: POP
// -------------------------------
Peca pop(Pilha *p) {
    if(p->topo == -1) {
        Peca vazia = {-1,'-'};
        return vazia;
    }
    return p->itens[p->topo--];
}

// -------------------------------
// MOSTRAR FILA
// -------------------------------
void mostrarFila(Fila f) {
    printf("\nFILA: ");
    int idx = f.inicio;
    for(int i = 0; i < f.qtd; i++) {
        printf("[%d:%c] ", f.itens[idx].id, f.itens[idx].tipo);
        idx = (idx + 1) % TAM_FILA;
    }
    printf("\n");
}

// -------------------------------
// MOSTRAR PILHA
// -------------------------------
void mostrarPilha(Pilha p) {
    printf("PILHA: ");
    if(p.topo == -1){
        printf("(vazia)\n");
        return;
    }
    for(int i = 0; i <= p.topo; i++){
        printf("[%d:%c] ", p.itens[i].id, p.itens[i].tipo);
    }
    printf("\n");
}

// -------------------------------
// TROCAR TOPO DA PILHA COM FRENTE DA FILA
// -------------------------------
void trocar(Pilha *p, Fila *f) {
    if(p->topo == -1) {
        printf("Pilha vazia! Não há o que trocar.\n");
        return;
    }

    Peca temp = p->itens[p->topo];
    p->itens[p->topo] = f->itens[f->inicio];
    f->itens[f->inicio] = temp;

    printf("Peças trocadas!\n");
}

// -------------------------------
// DESFAZER JOGADA (UNDO)
// -------------------------------
void desfazer(Historico *h, Fila *f) {
    if(!h->houveJogada) {
        printf("Nenhuma jogada para desfazer!\n");
        return;
    }

    if(f->qtd == TAM_FILA) {
        printf("Fila cheia! Não é possível desfazer.\n");
        return;
    }

    // Reinsere a última peça jogada
    f->inicio = (f->inicio == 0 ? TAM_FILA - 1 : f->inicio - 1);
    f->itens[f->inicio] = h->ultimaPecaJugada;
    f->qtd++;

    h->houveJogada = 0;

    printf("Última jogada desfeita!\n");
}

// -------------------------------
// INVERTER FILA COM PILHA
// -------------------------------
void inverterEstruturas(Fila *f, Pilha *p) {
    // passa fila pra pilha
    Pilha tempPilha;
    tempPilha.topo = -1;

    while(f->qtd > 0) {
        push(&tempPilha, removerFila(f));
    }

    // passa pilha antiga pra fila
    Fila tempFila;
    tempFila.inicio = 0;
    tempFila.fim = 0;
    tempFila.qtd = 0;

    while(p->topo != -1) {
        inserirFila(&tempFila, pop(p));
    }

    // copiar resultados
    *f = tempFila;
    *p = tempPilha;

    printf("Fila e pilha foram invertidas!\n");
}


// -------------------------------
// MAIN
// -------------------------------
int main() {
    srand(time(NULL));

    Fila fila;
    Pilha pilha;
    Historico h = {0};

    inicializarFila(&fila);
    inicializarPilha(&pilha);

    int opcao;

    do {
        printf("\n===== MENU =====\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar peça (push)\n");
        printf("3 - Usar peça reservada (pop)\n");
        printf("4 - Trocar frente da fila com topo da pilha\n");
        printf("5 - Desfazer última jogada\n");
        printf("6 - Inverter fila com pilha\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao) {

        case 1: {
            if(fila.qtd == 0) {
                printf("Fila vazia! Impossível jogar.\n");
                break;
            }
            Peca jogada = removerFila(&fila);
            h.houveJogada = 1;
            h.ultimaPecaJugada = jogada;

            printf("Peça jogada: [%d:%c]\n", jogada.id, jogada.tipo);
            inserirFila(&fila, gerarPeca());
        } break;

        case 2: {
            if(fila.qtd == 0) {
                printf("Fila vazia!\n");
                break;
            }
            Peca p = removerFila(&fila);
            push(&pilha, p);
            inserirFila(&fila, gerarPeca());
        } break;

        case 3: {
            Peca p = pop(&pilha);
            if(p.id == -1) {
                printf("Pilha vazia!\n");
                break;
            }
            printf("Peça usada: [%d:%c]\n", p.id, p.tipo);
        } break;

        case 4:
            trocar(&pilha, &fila);
            break;

        case 5:
            desfazer(&h, &fila);
            break;

        case 6:
            inverterEstruturas(&fila, &pilha);
            break;

        case 0:
            printf("Saindo...\n");
            break;

        default:
            printf("Opção inválida!\n");
        }

        mostrarFila(fila);
        mostrarPilha(pilha);

    } while(opcao != 0);

    return 0;
}

