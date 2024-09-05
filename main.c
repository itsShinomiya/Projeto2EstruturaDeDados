#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// Definindo constantes
#define MAX_CINTO 5
#define MAX_NOME 50
#define NUM_CAIXAS 10
#define NUM_INIMIGOS_POSSIVEIS 3
#define NUM_ELEMENTOS_POSSIVEIS 5
#define DANO_MAOS 5

// Estruturas
typedef struct Item {
    char nome[MAX_NOME];
    int peso;
    int ataque;
    int cura;
    struct Item *prox;
} Item;

typedef struct Inimigo {
    char nome[MAX_NOME];
    int ataque;
    int vida;
    struct Inimigo *prox;
} Inimigo;

typedef struct Caixa {
    int numero; // Campo para armazenar o número da caixa
    Item *elemento;
    Inimigo *inimigo;
    struct Caixa *prox;
    struct Caixa *ant;
} Caixa;

typedef struct Mapa {
    Caixa *inicio;
    int tamanho;
} Mapa;

typedef struct Heroi {
    char nome[MAX_NOME];
    int vida;
    Item *cinto[MAX_CINTO];
    Item *mochila;
    Caixa *posicaoAtual;
} Heroi;

// Funções de inicialização
Heroi *criarHeroi(char nome[]) {
    Heroi *heroi = (Heroi *)malloc(sizeof(Heroi));
    strcpy(heroi->nome, nome);
    heroi->vida = 100;
    heroi->mochila = NULL;
    heroi->posicaoAtual = NULL;
    for (int i = 0; i < MAX_CINTO; i++) {
        heroi->cinto[i] = NULL;
    }
    return heroi;
}

Item *criarItem(char nome[], int peso, int ataque, int cura) {
    Item *item = (Item *)malloc(sizeof(Item));
    strcpy(item->nome, nome);
    item->peso = peso;
    item->ataque = ataque;
    item->cura = cura;
    item->prox = NULL;
    return item;
}

Inimigo *criarInimigo(char nome[], int ataque, int vida) {
    Inimigo *inimigo = (Inimigo *)malloc(sizeof(Inimigo));
    strcpy(inimigo->nome, nome);
    inimigo->ataque = ataque;
    inimigo->vida = vida;
    inimigo->prox = NULL;
    return inimigo;
}

Caixa *criarCaixa(int numero, Item *elemento, Inimigo *inimigo) {
    Caixa *caixa = (Caixa *)malloc(sizeof(Caixa));
    caixa->numero = numero; // Define o número da caixa
    caixa->elemento = elemento;
    caixa->inimigo = inimigo;
    caixa->prox = NULL;
    caixa->ant = NULL;
    return caixa;
}

Mapa *criarMapa(int tamanho) {
    Mapa *mapa = (Mapa *)malloc(sizeof(Mapa));
    mapa->inicio = NULL;
    mapa->tamanho = tamanho;
    return mapa;
}

// Funções de gerenciamento
void adicionarItemMochila(Heroi *heroi, Item *item) {
    item->prox = heroi->mochila;
    heroi->mochila = item;
}

void adicionarItemCinto(Heroi *heroi, Item *item) {
    for (int i = 0; i < MAX_CINTO; i++) {
        if (heroi->cinto[i] == NULL) {
            heroi->cinto[i] = item;
            return;
        }
    }
    printf("Cinto cheio!\n");
}

void exibirEstadoHeroi(Heroi *heroi) {
    printf("Herói: %s | Vida: %d\n", heroi->nome, heroi->vida);
    printf("Cinto:\n");
    for (int i = 0; i < MAX_CINTO; i++) {
        if (heroi->cinto[i] != NULL) {
            printf("  [%d] %s (Peso: %d, Ataque: %d, Cura: %d)\n", i, heroi->cinto[i]->nome, heroi->cinto[i]->peso, heroi->cinto[i]->ataque, heroi->cinto[i]->cura);
        } else {
            printf("  [%d] Vazio\n", i);
        }
    }
    if (heroi->mochila) {
        printf("Mochila: %s (Peso: %d, Ataque: %d, Cura: %d)\n", heroi->mochila->nome, heroi->mochila->peso, heroi->mochila->ataque, heroi->mochila->cura);
    } else {
        printf("Mochila está vazia\n");
    }
}

// Funções de interação
void interagirElemento(Heroi *heroi, Caixa *caixa) {
    if (caixa->elemento) {
        Item *item = caixa->elemento;
        printf("Você encontrou um item: %s (Peso: %d, Ataque: %d, Cura: %d)\n", item->nome, item->peso, item->ataque, item->cura);
        printf("Deseja pegar o item? (1 - Sim, 0 - Não): ");
        int escolha;
        scanf("%d", &escolha);
        if (escolha == 1) {
            printf("Deseja colocar o item na mochila (1) ou no cinto (2)?: ");
            int opcao;
            scanf("%d", &opcao);
            if (opcao == 1) {
                adicionarItemMochila(heroi, item);
            } else if (opcao == 2) {
                adicionarItemCinto(heroi, item);
            }
            // Remover o item da caixinha após ser recolhido
            caixa->elemento = NULL;
        }
    }
}
void usarPocao(Heroi *heroi) {
    // Verifica se há poção no cinto
    for (int i = 0; i < MAX_CINTO; i++) {
        if (heroi->cinto[i] && heroi->cinto[i]->cura > 0) {
            printf("Você usou uma %s! Cura: %d\n", heroi->cinto[i]->nome, heroi->cinto[i]->cura);
            heroi->vida += heroi->cinto[i]->cura;
            free(heroi->cinto[i]);
            heroi->cinto[i] = NULL;
            return;
        }
    }

    // Verifica se há poção na mochila (no topo)
    if (heroi->mochila && heroi->mochila->cura > 0) {
        printf("Você usou uma %s! Cura: %d\n", heroi->mochila->nome, heroi->mochila->cura);
        heroi->vida += heroi->mochila->cura;
        Item *temp = heroi->mochila;
        heroi->mochila = heroi->mochila->prox;
        free(temp);
    } else {
        printf("Você não tem poções disponíveis.\n");
    }
}

void batalhar(Heroi *heroi, Inimigo *inimigo, Caixa *caixa) {
    int vidaInicialHeroi = heroi->vida; // Armazena a vida inicial do herói para calcular a poção após a vitória

    printf("Você encontrou um inimigo: %s (Ataque: %d, Vida: %d)\n", inimigo->nome, inimigo->ataque, inimigo->vida);
    while (heroi->vida > 0 && inimigo->vida > 0) {

        printf("Vida atual do inimigo: %d\n", inimigo->vida);

        exibirEstadoHeroi(heroi);
        printf("Escolha uma ação:\n");
        printf("1 - Atacar com arma do cinto\n");
        printf("2 - Atacar com arma da mochila\n");
        printf("3 - Bater com as mãos (Dano: %d)\n", DANO_MAOS);
        printf("4 - Fugir para uma caixa vizinha\n");

        // Exibe a opção de usar poção se houver alguma disponível
        int pocaoDisponivel = 0;
        for (int i = 0; i < MAX_CINTO; i++) {
            if (heroi->cinto[i] && heroi->cinto[i]->cura > 0) {
                pocaoDisponivel = 1;
                break;
            }
        }
        if (heroi->mochila && heroi->mochila->cura > 0) {
            pocaoDisponivel = 1;
        }
        if (pocaoDisponivel) {
            printf("5 - Usar poção\n");
        }

        int escolha;
        scanf("%d", &escolha);
        Item *arma = NULL;

        if (escolha == 1) {
            printf("Escolha uma arma do cinto (0 a %d): ", MAX_CINTO - 1);
            int posicao;
            scanf("%d", &posicao);
            if (posicao >= 0 && posicao < MAX_CINTO && heroi->cinto[posicao]) {
                arma = heroi->cinto[posicao];
            } else {
                printf("Escolha inválida!\n");
                continue;
            }
        } else if (escolha == 2) {
            if (heroi->mochila) {
                arma = heroi->mochila;
            } else {
                printf("Mochila está vazia!\n");
                continue;
            }
        } else if (escolha == 3) {
            printf("Você bate com as mãos! Dano: %d\n", DANO_MAOS);
            inimigo->vida -= DANO_MAOS;
        } else if (escolha == 4) {
            return;
        } else if (escolha == 5 && pocaoDisponivel) {
            usarPocao(heroi);
            continue;
        } else {
            printf("Escolha inválida!\n");
            continue;
        }

        // Se o jogador escolheu uma arma válida
        if (arma) {
            printf("Você ataca com %s! Dano: %d\n", arma->nome, arma->ataque);
            inimigo->vida -= arma->ataque;
        }

        if (inimigo->vida > 0) {
            printf("%s ataca! Dano: %d\n", inimigo->nome, inimigo->ataque);
            heroi->vida -= inimigo->ataque;
        }

        // Traçando uma linha para indicar o fim de um turno
        printf("--------------------------\n");
    }

    if (heroi->vida <= 0) {
        printf("Você foi derrotado!\n");
        exit(0);
    } else {
        printf("Você derrotou %s!\n", inimigo->nome);
        // Remover o inimigo da caixinha após ser derrotado
        caixa->inimigo = NULL;

        // Gerar uma poção como recompensa
        int curaPocao = vidaInicialHeroi / 2;
        Item *pocao = criarItem("Poção de Cura", 1, 0, curaPocao);

        printf("Você recebeu uma poção de cura que restaura %d de vida!\n", curaPocao);
        printf("Deseja colocar a poção na mochila (1) ou no cinto (2)?: ");
        int opcao;
        scanf("%d", &opcao);
        if (opcao == 1) {
            adicionarItemMochila(heroi, pocao);
        } else if (opcao == 2) {
            adicionarItemCinto(heroi, pocao);
        }
    }
}



// Função para gerar o mapa com caixinhas
Mapa *gerarMapa(int tamanho, int dificuldade) {
    Mapa *mapa = criarMapa(tamanho);
    Caixa *caixaAtual = NULL;

    // Listas de elementos e inimigos
    char *elementos[] = {"Espada", "Poção", "Escudo", "Arco", "Flechas"};
    char *inimigos[] = {"Goblin", "Orc", "Esqueleto"};

    for (int i = 0; i < tamanho; i++) {
        Item *item = NULL;
        Inimigo *inimigo = NULL;

        // Lógica para definir a chance de gerar itens conforme a dificuldade
        int chanceItem = 0;
        int chanceInimigo = 0;

        if (dificuldade == 1) { // Fácil
            chanceItem = 70; // Maior chance de itens aparecerem
            chanceInimigo = 30; // Menor chance de inimigos aparecerem
        } else if (dificuldade == 2) { // Médio
            chanceItem = 50; // Chances balanceadas
            chanceInimigo = 50;
        } else if (dificuldade == 3) { // Difícil
            chanceItem = 30; // Menor chance de itens aparecerem
            chanceInimigo = 70; // Maior chance de inimigos aparecerem
        }

        // Gerando itens aleatórios conforme a dificuldade
        if (rand() % 100 < chanceItem) {
            char *elemento = elementos[rand() % NUM_ELEMENTOS_POSSIVEIS];
            int ataque = 5 + (rand() % 5) * (dificuldade == 1 ? 2 : 1); // Itens mais poderosos no fácil
            int defesa = 10 * (rand() % 5);
            int cura = (strcmp(elemento, "Poção") == 0) ? 20 * (rand() % 2 + dificuldade) : 0;
            item = criarItem(elemento, ataque, defesa, cura);
        }

        // Gerando inimigos aleatórios conforme a dificuldade
        if (rand() % 100 < chanceInimigo) {
            char *nomeInimigo = inimigos[rand() % NUM_INIMIGOS_POSSIVEIS];
            int ataqueInimigo = 10 * (dificuldade + 1) + (rand() % 5) * (dificuldade == 3 ? 2 : 1); // Inimigos mais fortes no difícil
            int vidaInimigo = 30 * (dificuldade + 1) + (rand() % 20);
            inimigo = criarInimigo(nomeInimigo, ataqueInimigo, vidaInimigo);
        }

        // Criando a caixa
        Caixa *novaCaixa = criarCaixa(i, item, inimigo);
        if (mapa->inicio == NULL) {
            mapa->inicio = novaCaixa;
        } else {
            caixaAtual->prox = novaCaixa;
            novaCaixa->ant = caixaAtual;
        }
        caixaAtual = novaCaixa;
    }

    return mapa;
}
// Função para verificar se o mapa foi completado
int mapaCompleto(Mapa *mapa) {
    Caixa *caixaAtual = mapa->inicio;
    while (caixaAtual) {
        if (caixaAtual->inimigo) {
            return 0; // Ainda há inimigos
        }
        caixaAtual = caixaAtual->prox;
    }
    return 1; // Todos os inimigos foram derrotados
}

// Função principal do jogo
void jogar() {
    srand(time(NULL));

    // Criando o herói
    Heroi *heroi = criarHeroi("Aventureiro");

    // Criando os mapas
    Mapa *mapas[3];
    mapas[0] = gerarMapa(NUM_CAIXAS, 1); // Mapa fácil
    mapas[1] = gerarMapa(NUM_CAIXAS, 2); // Mapa médio
    mapas[2] = gerarMapa(NUM_CAIXAS, 3); // Mapa difícil

    int mapaAtual = 0;
    heroi->posicaoAtual = mapas[mapaAtual]->inicio;

    while (mapaAtual < 3) {
        printf("\nMapa %d\n", mapaAtual + 1);
        while (heroi->posicaoAtual) {
            printf("Você está na caixinha %d\n", heroi->posicaoAtual->numero);

            if (heroi->posicaoAtual->elemento) {
                interagirElemento(heroi, heroi->posicaoAtual);
            }
            if (heroi->posicaoAtual->inimigo) {
                batalhar(heroi, heroi->posicaoAtual->inimigo, heroi->posicaoAtual);
            }

            if (mapaCompleto(mapas[mapaAtual])) {
                printf("Você completou o mapa %d!\n", mapaAtual + 1);
                break;
            }

            // Pergunta para onde o jogador deseja ir
            printf("Escolha uma ação:\n");
            printf("1 - Avançar para a próxima caixinha\n");
            printf("2 - Voltar para a caixinha anterior\n");
            int escolha;
            scanf("%d", &escolha);

            if (escolha == 1 && heroi->posicaoAtual->prox) {
                heroi->posicaoAtual = heroi->posicaoAtual->prox;
            } else if (escolha == 2 && heroi->posicaoAtual->ant) {
                heroi->posicaoAtual = heroi->posicaoAtual->ant;
            } else {
                printf("Movimento inválido!\n");
            }
        }

        // Se o jogador completou o mapa atual, avançar para o próximo
        mapaAtual++;
        if (mapaAtual < 3) {
            heroi->posicaoAtual = mapas[mapaAtual]->inicio;
        } else {
            printf("Parabéns! Você completou todos os mapas!\n");
        }
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    jogar();
    return 0;
}
