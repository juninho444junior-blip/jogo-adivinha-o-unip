#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TENTATIVAS 10

/* Dados de cada nivel de dificuldade */
typedef struct {
    const char *nome;
    int maximo;      /* maior numero possivel */
    int limite;      /* limite de tentativas */
    int pontosBase;  /* pontos por acertar */
} Nivel;

/* Estatisticas acumuladas do jogador */
typedef struct {
    int partidas;
    int vitorias;
    int somaTentativas;  /* tentativas gastas nas vitorias */
    int pontos;
    int recorde;         /* maior pontuacao em uma unica partida */
} Estatisticas;

void limparBuffer(void){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

/* Le um inteiro e repete ate o usuario digitar um numero valido */
int lerInteiro(const char *msg){
    int valor;
    printf("%s", msg);
    while(scanf("%d", &valor) != 1){
        limparBuffer();
        printf("Entrada invalida! %s", msg);
    }
    limparBuffer();
    return valor;
}

/* Menu de dificuldade: devolve a struct do nivel escolhido */
Nivel escolherNivel(void){
    Nivel facil   = {"Facil",   5,   3, 10};
    Nivel medio   = {"Medio",   20,  5, 30};
    Nivel dificil = {"Dificil", 100, 7, 100};
    int op;

    printf("\n===== NIVEL DE DIFICULDADE =====\n");
    printf("1 - Facil   (1 a 5,   %d tentativas, %d pontos)\n", facil.limite, facil.pontosBase);
    printf("2 - Medio   (1 a 20,  %d tentativas, %d pontos)\n", medio.limite, medio.pontosBase);
    printf("3 - Dificil (1 a 100, %d tentativas, %d pontos)\n", dificil.limite, dificil.pontosBase);

    do{
        op = lerInteiro("Escolha (1-3): ");
    }while(op < 1 || op > 3);

    if(op == 1) return facil;
    if(op == 2) return medio;
    return dificil;
}

/* Mostra se o palpite esta perto ou longe do numero secreto */
void darDica(int palpite, int secreto, int maximo){
    int distancia = abs(palpite - secreto);

    if(palpite < secreto){
        printf("O numero sorteado e MAIOR. ");
    }else{
        printf("O numero sorteado e MENOR. ");
    }

    if(distancia == 1 || distancia * 10 <= maximo){
        printf("(Muito quente!)\n");
    }else if(distancia * 4 <= maximo){
        printf("(Morno)\n");
    }else{
        printf("(Frio)\n");
    }
}

/* Mostra todos os palpites da partida */
void mostrarHistorico(int historico[], int n){
    int i;
    printf("Seus palpites: ");
    for(i = 0; i < n; i++){
        printf("%d", historico[i]);
        if(i < n - 1) printf(" -> ");
    }
    printf("\n");
}

/* Joga uma partida completa e atualiza as estatisticas */
void jogarPartida(Nivel nivel, Estatisticas *est){
    int secreto = (rand() % nivel.maximo) + 1;
    int historico[MAX_TENTATIVAS];
    int tentativas = 0;
    int acertou = 0;
    int palpite, ganhos;

    printf("\n--- Nivel %s ---\n", nivel.nome);
    printf("Pensei em um numero de 1 a %d. Voce tem %d tentativas.\n", nivel.maximo, nivel.limite);

    while(tentativas < nivel.limite && acertou == 0){
        printf("\nTentativa %d de %d (restam %d)\n", tentativas + 1, nivel.limite, nivel.limite - tentativas);
        palpite = lerInteiro("Seu palpite: ");

        /* palpite fora do intervalo nao gasta tentativa */
        if(palpite < 1 || palpite > nivel.maximo){
            printf("Digite um numero de 1 a %d! (nao conta tentativa)\n", nivel.maximo);
            continue;
        }

        historico[tentativas] = palpite;
        tentativas++;

        if(palpite == secreto){
            acertou = 1;
        }else{
            darDica(palpite, secreto, nivel.maximo);
        }
    }

    est->partidas++;

    if(acertou){
        /* pontos base + 5 por cada tentativa que sobrou */
        ganhos = nivel.pontosBase + (nivel.limite - tentativas) * 5;
        est->vitorias++;
        est->somaTentativas += tentativas;
        est->pontos += ganhos;
        if(ganhos > est->recorde){
            est->recorde = ganhos;
            printf("\nNOVO RECORDE!\n");
        }
        printf("\nACERTOU em %d tentativa(s)! +%d pontos\n", tentativas, ganhos);
    }else{
        printf("\nAs tentativas acabaram! O numero era %d.\n", secreto);
    }

    mostrarHistorico(historico, tentativas);
}

void mostrarEstatisticas(Estatisticas est){
    printf("\n===== ESTATISTICAS =====\n");
    printf("Partidas jogadas : %d\n", est.partidas);
    printf("Vitorias         : %d\n", est.vitorias);
    printf("Derrotas         : %d\n", est.partidas - est.vitorias);

    if(est.partidas > 0){
        printf("Taxa de acerto   : %.1f%%\n", 100.0 * est.vitorias / est.partidas);
    }
    if(est.vitorias > 0){
        printf("Media de tentativas nas vitorias: %.1f\n", (float) est.somaTentativas / est.vitorias);
    }
    printf("Pontuacao total  : %d\n", est.pontos);
    printf("Melhor partida   : %d pontos\n", est.recorde);
}

int main(void){
    Estatisticas est = {0, 0, 0, 0, 0};
    Nivel nivel;
    int opcao;

    srand((unsigned int) time(NULL));

    printf("===== JOGO DE ADIVINHACAO =====\n");
    nivel = escolherNivel();

    do{
        jogarPartida(nivel, &est);
        printf("\nPontuacao total: %d\n", est.pontos);

        printf("\n===== MENU =====\n");
        printf("1 - Jogar novamente (nivel %s)\n", nivel.nome);
        printf("2 - Mudar nivel de dificuldade\n");
        printf("3 - Ver estatisticas\n");
        printf("0 - Sair\n");

        do{
            opcao = lerInteiro("Opcao: ");
        }while(opcao < 0 || opcao > 3);

        if(opcao == 2){
            nivel = escolherNivel();
        }else if(opcao == 3){
            mostrarEstatisticas(est);
            printf("\nPressione ENTER para continuar...");
            getchar();
            opcao = 1;  /* depois das estatisticas, joga de novo */
        }

    }while(opcao != 0);

    mostrarEstatisticas(est);
    printf("\nObrigado por jogar!\n");
    return 0;
}

