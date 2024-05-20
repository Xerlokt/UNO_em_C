#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

// Definições das cores e símbolos das cartas
typedef enum { VERMELHO, VERDE, AMARELO, AZUL, SEM_COR } Cor;
typedef enum { ZERO, UM, DOIS, TRES, QUATRO, CINCO, SEIS, SETE, OITO, NOVE, INVERTE, BLOQUEIO, MAIS_DOIS, MAIS_QUATRO, MUDAR_COR } Simbolo;

typedef struct {
    Cor cor;
    Simbolo simbolo;
} Carta;

#define TOTAL_CARTAS 108

typedef struct {
    Carta cartas[TOTAL_CARTAS];
    int topo;
} Monte;

Monte monte;

Carta mao_jogador[108];
Carta mao_computador[108];
int num_cartas_mao = 0;
int num_cartas_mao_computador = 0;

Carta carta_atual;
int vez_jogador = 1; // 1 = jogador, 2 = computador

// Funções de inicialização e embaralhamento
void inicializar_monte() {
    int index = 0;
    int cor;
	int simbolo;
	int i;
    for (cor = VERMELHO; cor <= AZUL; cor++) {
        for (simbolo = ZERO; simbolo <= NOVE; simbolo++) {
            monte.cartas[index].cor = (Cor)cor;
            monte.cartas[index++].simbolo = (Simbolo)simbolo;
            if (simbolo != ZERO) {
                monte.cartas[index].cor = (Cor)cor;
                monte.cartas[index++].simbolo = (Simbolo)simbolo;
            }
        }
        for (simbolo = INVERTE; simbolo <= MAIS_DOIS; simbolo++) {
            monte.cartas[index].cor = (Cor)cor;
            monte.cartas[index++].simbolo = (Simbolo)simbolo;
            monte.cartas[index].cor = (Cor)cor;
            monte.cartas[index++].simbolo = (Simbolo)simbolo;
        }
    }
    for (i = 0; i < 4; i++) {
        monte.cartas[index].cor = SEM_COR;
        monte.cartas[index++].simbolo = MAIS_QUATRO;
        monte.cartas[index].cor = SEM_COR;
        monte.cartas[index++].simbolo = MUDAR_COR;
    }
    monte.topo = TOTAL_CARTAS; // Inicializa o topo da pilha
}

void embaralhar_monte() {
	int i;
    for (i = TOTAL_CARTAS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carta temp = monte.cartas[i];
        monte.cartas[i] = monte.cartas[j];
        monte.cartas[j] = temp;
    }
}

void inicializar_mao_jogador() {
    num_cartas_mao = 0;
    num_cartas_mao_computador = 0;
    int i;
    for (i = 0; i < 7; i++) {
        mao_jogador[num_cartas_mao++] = monte.cartas[--monte.topo]; // Jogador compra uma carta
        mao_computador[num_cartas_mao_computador++] = monte.cartas[--monte.topo]; // Computador compra uma carta
    }
}

// Funções para exibir as cartas e estados do jogo
void imprimir_carta(Carta c) {
    char *cores[] = { "Vermelho", "Verde", "Amarelo", "Azul", "Sem Cor" };
    char *simbolos[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Inverte", "Bloqueio", "+2", "+4 Mudar a Cor", "Mudar Cor" };
    printf("%s %s\n", cores[c.cor], simbolos[c.simbolo]);
}

void imprimir_mao_jogador() {
    printf("Suas cartas:\n");
    int i;
    for (i = 0; i < num_cartas_mao; i++) {
        printf("%d. ", i + 1);
        imprimir_carta(mao_jogador[i]);
    }
}

void imprimir_mao_computador() {
    printf("Cartas do computador: %d\n", num_cartas_mao_computador);
}

void imprimir_carta_atual() {
    printf("Carta atual:\n");
    imprimir_carta(carta_atual);
}

void limpar_tela() {
    system(CLEAR); 
}

void pausar() {
    printf("Pressione Enter para continuar...");
    getchar();
}

// Funções de jogo
void jogar() {
    int escolha;
    int i;
    int cor;
    printf("Escolha uma carta para jogar (1-%d) ou 0 para comprar: ", num_cartas_mao);
    scanf("%d", &escolha);
    getchar(); // Limpar buffer
    if (escolha > 0 && escolha <= num_cartas_mao) {
        // Verifica se a carta escolhida é "Mudar Cor" ou "+4 Mudar a Cor" ou "Bloqueio"
        if (mao_jogador[escolha - 1].simbolo == MUDAR_COR || 
            (mao_jogador[escolha - 1].simbolo == MAIS_QUATRO && mao_jogador[escolha - 1].cor == SEM_COR) ||
            mao_jogador[escolha - 1].simbolo == BLOQUEIO) {
            if (mao_jogador[escolha - 1].simbolo == MAIS_QUATRO) {
                printf("Escolha uma nova cor (1. Vermelho, 2. Verde, 3. Amarelo, 4. Azul): ");
                scanf("%d", &cor);
                getchar(); // Limpar buffer
                carta_atual.cor = (Cor)(cor - 1);
                // O oponente compra quatro cartas adicionais
                for (i = 0; i < 4; i++) {
                    mao_computador[num_cartas_mao_computador++] = monte.cartas[--monte.topo];
                }
            } else if (mao_jogador[escolha - 1].simbolo == BLOQUEIO) {
                // Pula a vez do próximo jogador
                vez_jogador = vez_jogador == 1 ? 2 : 1;
            } else {
                // Se for "Mudar Cor", a cor pode ser escolhida livremente
                printf("Escolha uma nova cor (1. Vermelho, 2. Verde, 3. Amarelo, 4. Azul): ");
                scanf("%d", &cor);
                getchar(); // Limpar buffer
                carta_atual.cor = (Cor)(cor - 1);
            }
            // Remove a carta da mão do jogador
            for (i = escolha - 1; i < num_cartas_mao - 1; i++) {
                mao_jogador[i] = mao_jogador[i + 1];
            }
            num_cartas_mao--;
        } else {
            // Se não for "Mudar Cor" ou "+4 Mudar a Cor" ou "Bloqueio", verifica se a carta pode ser jogada normalmente
            if (mao_jogador[escolha - 1].cor == carta_atual.cor || mao_jogador[escolha - 1].simbolo == carta_atual.simbolo) {
                carta_atual = mao_jogador[escolha - 1];
                // Remove a carta da mão do jogador
                for (i = escolha - 1; i < num_cartas_mao - 1; i++) {
                    mao_jogador[i] = mao_jogador[i + 1];
                }
                num_cartas_mao--;
            } else {
                printf("Você só pode jogar uma carta com a mesma cor ou o mesmo número, ou a carta 'Mudar Cor', '+4 Mudar a Cor' ou 'Bloqueio'!\n");
                pausar();
            }
        }
    } else {
        // Compra uma carta
        mao_jogador[num_cartas_mao++] = monte.cartas[--monte.topo];
    }
}


void jogar_computador() {
    // Verifica se alguma carta na mão do computador pode ser jogada
    int i;
    int j;
    for (i = 0; i < num_cartas_mao_computador; i++) {
        if (mao_computador[i].cor == carta_atual.cor || mao_computador[i].simbolo == carta_atual.simbolo) {
            // Se encontrar uma carta que pode ser jogada, joga ela
            carta_atual = mao_computador[i];
            // Remove a carta da mão do computador
            for (j = i; j < num_cartas_mao_computador - 1; j++) {
                mao_computador[j] = mao_computador[j + 1];
            }
            num_cartas_mao_computador--;
            return;
        }
    }
    // Se não encontrar nenhuma carta jogável, compra uma carta
    mao_computador[num_cartas_mao_computador++] = monte.cartas[--monte.topo];
}



int verificar_fim_jogo() {
    return num_cartas_mao == 0 || num_cartas_mao_computador == 0;
}

void reiniciar_jogo() {
    // Reinicializa as variáveis do jogo
    num_cartas_mao = 0;
    num_cartas_mao_computador = 0;
    vez_jogador = 1;
    inicializar_monte();
    embaralhar_monte();
    inicializar_mao_jogador();
}

void menu_principal() {
    int opcao;
    int direcao_jogo = 1;
    do {
        limpar_tela();
        printf("=== Menu Principal ===\n");
        printf("1. Começar Jogo\n");
        printf("2. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); // Limpar buffer

        switch (opcao) {
            case 1:
                srand(time(NULL)); // Semente para geração de números aleatórios
                reiniciar_jogo(); // Inicializa e embaralha o monte, e distribui as cartas para os jogadores

                // A primeira carta do monte é a carta atual em jogo
                carta_atual = monte.cartas[--monte.topo]; // Compra a carta do topo do monte

                while (!verificar_fim_jogo()) {
                    limpar_tela();
                    imprimir_carta_atual();
                    imprimir_mao_jogador();
                    imprimir_mao_computador();
                    if (carta_atual.simbolo == INVERTE) {
                        direcao_jogo *= -1; // Inverte a direção do jogo
                    }

                    if (vez_jogador == 1) {
                        jogar();
                    } else {
                        jogar_computador();
                    }

                    if (direcao_jogo == 1) {
                        vez_jogador = vez_jogador == 1 ? 2 : 1; // Alterna entre jogador e computador
                    } else {
                        vez_jogador = vez_jogador == 1 ? 1 : 2; // Inverte a ordem de alternância entre jogador e computador
                    }

                    pausar();
                }

                if (num_cartas_mao == 0) {
                    printf("Parabéns! Você venceu!\n");
                } else {
                    printf("O computador venceu!\n");
                }
                pausar();
                break;
            case 2:
                printf("Saindo do jogo...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                pausar();
                break;
        }
    } while (opcao != 2);
}


int main() {
    menu_principal();
    return 0;
}
