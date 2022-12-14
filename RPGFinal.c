// inclus?o das bibliotecas
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<locale.h>

// tamanhos m?ximos pr?-definidos
#define MAX 10
#define SIZE 20

// arquivos
#define NOME_ARQUIVO_ENTRADA "entrada.txt"
#define NOME_ARQUIVO_SAIDA "saida.txt"

// tipos de n?s que existem
typedef enum _tipoNo
{
    raiz,
    naoTerminal,
    terminal
} tipoNo;

// itens essenciais
typedef enum _itensEssenciais
{
    sacoImpermeavel,
    lanterna,
    caboSolteiro,
    caderno,
    itemEssencial // item gen?rico para inicializar o invent?rio

} itensEssenciais;

// itens raros
typedef enum _itensRaros
{
    malvinao,
    barrinhaDeCereal,
    faca,
    itemRaro // item gen?rico para inicializar o invent?rio
} itensRaros;

// vari?vel que indica a op??o que eu selecionei em um n? e para qual n? essa op??o leva
typedef struct _opcao
{
    char opcaoSelecionada;
    int indiceProximoNo;
} opcao;

// n? propriamente dito e suas caracter?sticas
typedef struct _no
{
    int indice;
    tipoNo tipo;
    opcao opcoes[MAX];
    int nOpcoes;
    char texto[1001];
    struct _no *anterior;
    struct _no *proximo;
} no;

// ponteiros de n?s que apontam para o n? raiz e o ponteiro que estamos atualmente
no *ptrInicio;
no *ptrAtual;

// arquivo de sa?da
FILE *arquivoSaida;

// vari?veis globais
int moedas;
int counterFO = 0;
int counterAtividades = 0;
int counterItensRaros = 0;

// invent?rios
itensEssenciais inventarioEssencial[6] = {itemEssencial, itemEssencial, itemEssencial, itemEssencial, itemEssencial, itemEssencial};
itensRaros inventarioRaro[3] = {itemRaro, itemRaro, itemRaro};

// prot?tipos
char *lerNomeJogador(char *nomeArquivo);
void cadastrarNo(int indice, tipoNo tipo, opcao *opcoes, int nOpcoes, char *texto);
void cadastrarNos();
no *buscarNo(int indice);
int lerIndiceProximoNo(char opcao);
int isCompra(int indice);
int compra(int indice, int moedas, char opcao, itensEssenciais *inventarioEssencial, itensRaros *inventarioRaro);
int isValendoFO(int indice);
int isValendoItem(int indice);
int verificarItemEssencial(int indice, itensEssenciais *inventarioEssencial);
int verificarItemRaro(int indice, itensRaros *inventarioRaro);
int isFoundable(int indice);
void addItemRaro(itensRaros *inventarioRaro, int indice);
void addItemEssencial(itensEssenciais *inventarioEssencial);
void limparTela();
void readItensRaros(itensRaros *inventarioRaro, int *counterItensRaros, int *counterFO);
void printInventarioEssencial(itensEssenciais *inventarioEssencial);
void printInventarioRaro(itensRaros *inventarioRaro);

// main
int main()
{
    setlocale(LC_ALL, "Portuguese"); // setar linguagem para portugu?s

    // abertura do arquivo de sa?da
	if (!(arquivoSaida = fopen(NOME_ARQUIVO_SAIDA, "w"))) {
		printf("ERRO AO ABRIR O ARQUIVO DE SAIDA!");
		exit(1);
	}

	//mensagem de boas-vindas
	char nomeJogador[100];
	strcpy(nomeJogador, lerNomeJogador(NOME_ARQUIVO_ENTRADA));
	printf("Bem-vindo ao Acampamento B?sico, %s!\n", nomeJogador);
	fprintf(arquivoSaida, "Bem-vindo ao Acampamento B?sico, %s!\n", nomeJogador);

	// come?ar a contagem do tempo
    clock_t ticks[3];

    ticks[0] = clock();

    cadastrarNos(); // montagem dos n?s em uma lista encadeada

    ptrAtual = buscarNo(0); // carregar o n? raiz

    // la?o de controle do jogo
    while(1) {

        // se o n? n?o ? terminal, o programa imprime seu texto e l? a op??o
        if(ptrAtual->tipo != terminal) {
            int indiceProximoNo = -1;
            while(indiceProximoNo == -1) {
                char opcao;
                printf("%s", ptrAtual->texto);
                fprintf(arquivoSaida, "%s", ptrAtual->texto);
                scanf(" %c", &opcao);
                fprintf(arquivoSaida, "%c\n", opcao);

                // se o jogador deseja ver suas moedas
                while(opcao == 'T' && ptrAtual->indice == 6) {

                        if(counterAtividades == 0) {

                            ticks[1] = clock();
                            moedas = (ticks[1] - ticks[0]) / CLOCKS_PER_SEC;
                            printf("\nVoc? tem %d moedas\n", moedas);
                            fprintf(arquivoSaida, "\nVoc? tem %d moedas\n", moedas);

                            printf("%s", ptrAtual->texto);
                            fprintf(arquivoSaida, "%s", ptrAtual->texto);
                            scanf(" %c", &opcao);
                            fprintf(arquivoSaida, "%c\n", opcao);
                        }

                        else {

                            while(opcao == 'T') {
                                ticks[1] = clock();
                                moedas = ((ticks[1] - ticks[0]) / CLOCKS_PER_SEC) + 70;
                                printf("Voc? tem %d moedas\n", moedas);
                                fprintf(arquivoSaida, "Voc? tem %d moedas\n", moedas);

                                printf("%s", ptrAtual->texto);
                                fprintf(arquivoSaida, "%s", ptrAtual->texto);
                                scanf(" %c", &opcao);
                                fprintf(arquivoSaida, "%c\n", opcao);
                            }
                        }
                    }

                // leitura do pr?ximo n? pela op??o selecionada
                indiceProximoNo = lerIndiceProximoNo(opcao);

                if(indiceProximoNo == -1 && opcao != 'T') {
                    printf("\nOp??o Inv?lida!\n");
                    fprintf(arquivoSaida, "\nOp??o Inv?lida!\n");
                }

                else {

                    // se o n? em quest?o ? um n? de compra
                    if(isCompra(ptrAtual->indice)) {

                        if(counterAtividades == 0) {
                            ticks[1] = clock();
                            moedas = (ticks[1] - ticks[0]) / CLOCKS_PER_SEC;

                            if(compra(ptrAtual->indice, moedas, opcao, inventarioEssencial, inventarioRaro)) {
                                printf("\nCompra efetuada!\n\n");
                                fprintf(arquivoSaida, "\nCompra efetuada!\n\n");
                                no *ptrProximoNo = buscarNo(indiceProximoNo);
                                ptrAtual = ptrProximoNo;
                            }
                        }

                        else if(counterAtividades == 1) {
                            ticks[1] = clock();
                            moedas = (ticks[1] - ticks[0]) / CLOCKS_PER_SEC + 70;

                            if(compra(ptrAtual->indice, moedas, opcao, inventarioEssencial, inventarioRaro)) {
                                printf("\nCompra efetuada!\n\n");
                                fprintf(arquivoSaida, "\nCompra efetuada!\n\n");
                                no *ptrProximoNo = buscarNo(indiceProximoNo);
                                ptrAtual = ptrProximoNo;
                            }
                        }

                        else if(counterAtividades == 2) {
                            ticks[1] = clock();
                            moedas = (ticks[1] - ticks[0]) / CLOCKS_PER_SEC + 70;

                            if(compra(ptrAtual->indice, moedas, opcao, inventarioEssencial, inventarioRaro)) {
                                printf("\nCompra efetuada!\n\n");
                                fprintf(arquivoSaida, "\nCompra efetuada!\n\n");
                                no *ptrProximoNo = buscarNo(200);
                                ptrAtual = ptrProximoNo;
                            }
                        }
                    }

                    // se o n? em quest?o est? valendo FO-
                    else if(isValendoFO(ptrAtual->indice)) {
                        counterFO++;
                        no *ptrProximoNo = buscarNo(indiceProximoNo);
                        ptrAtual = ptrProximoNo;
                        limparTela();
                    }

                    // se o n? ? um n? que finaliza uma atividade, o programa conta uma atividade feita
                    else if(ptrAtual->indice == 35 || ptrAtual->indice == 58 || ptrAtual->indice == 33) {
                        counterAtividades++;
                        ticks[0] = clock();
                        no *ptrProximoNo = buscarNo(indiceProximoNo);
                        ptrAtual = ptrProximoNo;
                        limparTela();
                    }

                    // se ? um n? em que o jogador acha e ganha um item raro
                    else if(isFoundable(ptrAtual->indice)) {
                        if(ptrAtual-> indice == 29) {
                            addItemEssencial(inventarioEssencial);
                            no *ptrProximoNo = buscarNo(indiceProximoNo);
                            ptrAtual = ptrProximoNo;
                            limparTela();
                        }

                        else {
                            addItemRaro(inventarioRaro, ptrAtual->indice);
                            no *ptrProximoNo = buscarNo(indiceProximoNo);
                            ptrAtual = ptrProximoNo;
                            limparTela();
                        }
                    }


                    // verifica se existe alguma condi??o de item raro para continuar, e se n?o for satisfeita, encaminha para um n? terminal
                    else if(verificarItemRaro(ptrAtual->indice, inventarioRaro) == 0 && ptrAtual->indice == 23) {
                        no *ptrProximoNo = buscarNo(100);
                        ptrAtual = ptrProximoNo;
                        limparTela();
                    }

                    // verifica se existe alguma condi??o de item essencial para continuar, e se n?o for satisfeita, encaminha para um n? terminal
                    else if(verificarItemEssencial(ptrAtual->indice, inventarioEssencial) == 0 && ptrAtual->indice == 24) {
                        no *ptrProximoNo = buscarNo(101);
                        ptrAtual = ptrProximoNo;
                        limparTela();
                    }

                    // verifica se existe alguma condi??o de item essencial para continuar, e se n?o for satisfeita, encaminha para um n? terminal
                    else if(verificarItemEssencial(ptrAtual->indice, inventarioEssencial) == 0 && ptrAtual->indice == 47) {
                        no *ptrProximoNo = buscarNo(103);
                        ptrAtual = ptrProximoNo;
                        limparTela();
                    }

                    // verifica se existe alguma condi??o de item raro para continuar, e se for satisfeita, continua
                    else if(verificarItemRaro(ptrAtual->indice, inventarioRaro) == 1 && counterAtividades < 2) {
                        no *ptrProximoNo = buscarNo(indiceProximoNo);
                        ptrAtual = ptrProximoNo;
                        limparTela();
                    }

                    // verifica se existe alguma condi??o de item essencial para continuar, e se for satisfeita, continua
                    else if(verificarItemEssencial(ptrAtual->indice, inventarioEssencial) == 1 && counterAtividades < 2) {
                        no *ptrProximoNo = buscarNo(indiceProximoNo);
                        ptrAtual = ptrProximoNo;
                        limparTela();
                    }
                }
            }
        }

        // se o n? ? terminal
        else {

            if(ptrAtual->indice == 200) {
                printf("%s", ptrAtual->texto);
                fprintf(arquivoSaida, "%s", ptrAtual->texto);

                if(inventarioRaro[1] == barrinhaDeCereal) {
                    counterFO++;
                    printf("\n\nVoc? foi plotado com uma barrinha de cereal... Torrado, FO-!\n\n");
                }

                for(int i = 0; i < 3; i++) {
                    if(inventarioRaro[i] != itemRaro) {
                        counterItensRaros++;
                    }
                }

                printf("\n\nFO-: %d\n\n", counterFO);
                fprintf(arquivoSaida, "\nFO-: %d\n\n", counterFO);
                printf("Itens Essenciais:\n");
                fprintf(arquivoSaida, "Itens Essenciais:\n");
                printInventarioEssencial(inventarioEssencial);
                printf("\n\n");
                printf("Itens Raros: \n");
                fprintf(arquivoSaida, "Itens Raros:\n");
                printInventarioRaro(inventarioRaro);
                printf("\n\nPotua??o: ");
                fprintf(arquivoSaida, "\n\nPotua??o: ");
                printf("%d", 100 * counterItensRaros - 50 * counterFO);
                fprintf(arquivoSaida, "%d", 100 * counterItensRaros - 50 * counterFO);
                break;
            }

            else {
                printf("%s", ptrAtual->texto);
                break;
            }
        }
    }
}

// l? o nome do jogador a partir do arquivo
char *lerNomeJogador(char *nomeArquivo) {
	FILE *arquivoEntrada;
	char *nome = NULL, buffer[100];

	//abrir o arquivo texto para leitura
	arquivoEntrada = fopen(nomeArquivo, "r");
	if (arquivoEntrada == NULL) {
		printf("\nERRO AO ABRIR O ARQUIVO DE ENTRADA!");
		fprintf(arquivoSaida, "\nERRO AO ABRIR O ARQUIVO DE ENTRADA!");
		exit(1);
	}
	//ler nome do jogador no arquivo
	if(!feof(arquivoEntrada)) {
		nome = fgets(buffer, 100, arquivoEntrada);
	}
	//fechar o arquivo de entrada
	fclose(arquivoEntrada);

	//retornar o nome lido
	return nome;
}

void cadastrarNo(int indice, tipoNo tipo, opcao *opcoes, int nOpcoes, char *texto)
{
    no *ptr = (no *) malloc(sizeof(no));

	if(ptr == NULL) {
		printf("\nErro na aloca??o de mem?ria!");
		return;
    }

    ptr->indice = indice;
    strcpy(ptr->texto, texto);
    ptr->tipo = tipo;
    ptr->nOpcoes = nOpcoes;
    for(int i = 0; i < nOpcoes; i++) {
        ptr->opcoes[i] = opcoes[i];
    }
    ptr->anterior = NULL;
    ptr->proximo = NULL;

    no *ptrAux = ptrInicio;
    if(ptrAux != NULL) {
        while(ptrAux->proximo != NULL) {
            ptrAux = ptrAux->proximo;
        }

        ptrAux->proximo = ptr;
        ptr->anterior = ptrAux;
    }

    else {
        ptrInicio = ptr;
    }
}

void cadastrarNos()
{
    opcao opcoes0[1] = {{'*', 1}};
    cadastrarNo(
    0,
    raiz,
    opcoes0,
    1,
    "Neste jogo, voc? ? um aluno do IME que, pela primeira vez, est? indo para o tem?vel Campo. \nAssim, voc? deve se preparar da melhor forma, fazendo as decis?es de forma a concluir todas as atividades.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes1[1] = {{'*', 2}};
    cadastrarNo(
    1,
    naoTerminal,
    opcoes1,
    1,
    "Durante o jogo, voc? ter? constantemente um invent?rio e moedas.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes2[1] = {{'*', 3}};
    cadastrarNo(
    2,
    naoTerminal,
    opcoes2,
    1,
    "As moedas ser?o ganhadas automaticamente, sem nenhum esfor?o.\nCom as moedas, o jogador poder? comprar itens ?teis ?s atividades e desafios.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes3[1] = {{'*', 4}};
    cadastrarNo(
    3,
    naoTerminal,
    opcoes3,
    1,
    "J? no invent?rio, haver? os itens essenciais e raros que o jogador possui.\nEsses itens ser?o, ora comprados, ora encontrados em alguns caminhos.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes4[1] = {{'*', 5}};
    cadastrarNo(
    4,
    naoTerminal,
    opcoes4,
    1,
    "Finalmente, os seus itens, as moedas e os seus poss?veis FOs durante a atividade influenciar?o na sua pontua??o final.\nEnt?o, tome cuidado nas escolhas, pois nem todas as vantagens s?o o que parecem ser!\n\nAperte qualquer tecla + <ENTER> para iniciar o jogo\n"
    );

    opcao opcoes5[1] = {{'*', 6}};
    cadastrarNo(
    5,
    naoTerminal,
    opcoes5,
    1,
    "O CAMPO\n\nVoc? acaba de chegar no local de Campo, e desce facilmente da viatura com sua leve mochila.\nVoc? estranha o peso da sua mochila e, ao abri-la, percebe que esqueceu TUDO no IME!\nRapidamente, um aluno safo tenta te ajudar, te vendendo um item essencial para o campo.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes6[3] = {{'A', 7}, {'B', 7}, {'C', 7}};
    cadastrarNo(
    6,
    naoTerminal,
    opcoes6,
    3,
    "Qual item comprar? \n\nA- Saco Imperme?vel: 110 Moedas\nB- Lanterna: 100 Moedas\nC- Cabo Solteiro: 80 Moedas\nT- Ver Moedas\n\nOp??o escolhida: "
    );

    opcao opcoes7[2] = {{'A', 8}, {'B', 9}};
    cadastrarNo(
    7,
    naoTerminal,
    opcoes7,
    2,
    "O Tenente separa o primeiro ano em 2 pelot?es para fazer as atividades. \nQual atividade fazer? \n\nA- Progress?o\nB- Marcha\n\nOp??o escolhida: "
    );

    opcao opcoes8[1] = {{'*', 11}};
    cadastrarNo(
    8,
    naoTerminal,
    opcoes8,
    1,
    "Agora, enquanto est? claro, o Tenente instrui seu pelot?o acerca da progress?o diurna.\nVoc? adentra na pista e...\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes11[3] = {{'A', 12}, {'B', 13}, {'C', 14}};
    cadastrarNo(
    11,
    naoTerminal,
    opcoes11,
    3,
    "Na primeira parte voc? deve encarar uma pista de rastejo e lembra que na instru??o haviam 3 op??es. \nComo prosseguir? \n\nA- Engatinhar\nB- Rastejo alto\nC- Rastejo baixo\n\nOp??o escolhida:  "
    );

    opcao opcoes12[1] = {{'*', 11}};
    cadastrarNo(
    12,
    naoTerminal,
    opcoes12,
    1,
    "O Tenente te acusou de acochambra??o e, por isso, deu um FO- e pediu para repetir a pista. \n\nAperte qualquer tecla + <ENTER> para voltar ao in?cio\n"
    );

    opcao opcoes13[1] = {{'*', 11}};
    cadastrarNo(
    13,
    naoTerminal,
    opcoes13,
    1,
    "O Tenente achou que podia ser melhor, ent?o deu outra chance. \n\nAperte qualquer tecla + <ENTER> para voltar ao in?cio\n"
    );

    opcao opcoes14[1] = {{'*', 15}};
    cadastrarNo(
    14,
    naoTerminal,
    opcoes14,
    1,
    "O Tenente gostou do seu desempenho, mas a progress?o diurna ainda n?o acabou. \n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes15[2] = {{'A', 16}, {'B', 19}};
    cadastrarNo(
    15,
    naoTerminal,
    opcoes15,
    2,
    "Voc? segue correndo, exausto, e depara-se com uma bifurca??o:\n\n? esquerda, voc? ouve um som long?nquo de ?gua corrente;\n? direita, a mata aparenta ser mais fechada. \nQual caminho tomar? \n\nA- Esquerda\nB- Direita\n\nOp??o escolhida:  "
    );

    opcao opcoes16[2] = {{'A', 17}, {'B', 18}};
    cadastrarNo(
    16,
    naoTerminal,
    opcoes16,
    2,
    "Ap?s andar muito, voc? chega em um rio desconhecido, e vai seguir por suas margens.\nComo um aluno safo, v? nisso a oportunidade de beber a ?gua do rio, economizando a do cantil.\n\nA- Beber a ?gua\nB- N?o beber e prosseguir\n\nOp??o escolhida:  "
    );

    opcao opcoes17[1] = {{'*', 20}};
    cadastrarNo(
    17,
    naoTerminal,
    opcoes17,
    1,
    "J? cansado, mas se aproximando do final, encontra uma pista de cordas.\n\nAperte qualquer tecla + <ENTER> para tentar a pista\n"
    );

    opcao opcoes20[1] = {{'*', 0}};
    cadastrarNo(
    20,
    terminal,
    opcoes20,
    1,
    "Tentando a pista, voc? come?ou a sentir-se mal...\n\nO Tenente adverte que foi por causa da ?gua estranha que voc? consumiu.\nInfelizmente, voc? precisar? ir na visita m?dica, e n?o concluiu o campo. \nFim de Jogo\n"
    );

    opcao opcoes18[1] = {{'*', 21}};
    cadastrarNo(
    18,
    naoTerminal,
    opcoes18,
    1,
    "J? cansado, mas se aproximando do final, encontra uma pista de cordas.\n\nAperte qualquer tecla + <ENTER> para tentar a pista\n"
    );

    opcao opcoes21[1] = {{'*', 24}};
    cadastrarNo(
    21,
    naoTerminal,
    opcoes21,
    1,
    "Ap?s muito esfor?o, a pista e a progress?o diurna foram conclu?das.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes19[2] = {{'A', 15}, {'B', 23}};
    cadastrarNo(
    19,
    naoTerminal,
    opcoes19,
    2,
    "A mata come?a a fechar e a temperatura come?a a cair... Voc? deve escolher sabiamente: \n\nA- Voltar\nB- Prosseguir\n\nOp??o escolhida: "
    );

    opcao opcoes22[1] = {{'*', 18}};
    cadastrarNo(
    22,
    naoTerminal,
    opcoes22,
    1,
    "Voc? utilizou a faca e conseguiu encontrar um malvin?o.\n\nAperte qualquer tecla + <ENTER> para adicionar o malvin?o ao invent?rio\n"
    );

    opcao opcoes23[1] = {{'*', 22}};
    cadastrarNo(
    23,
    naoTerminal,
    opcoes23,
    1,
    "A mata ficou fechada e voc? precisa de uma faca.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes24[1] = {{'*', 25}};
    cadastrarNo(
    24,
    naoTerminal,
    opcoes24,
    1,
    "O tempo escureceu e o Tenente est? prestes a inciar a progress?o noturna. Voc? precisa de uma lanterna.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes25[1] = {{'*', 26}};
    cadastrarNo(
    25,
    naoTerminal,
    opcoes25,
    1,
    "Usando sua lanterna, voc? identificou um aliado infiltrado nas linhas inimigas.\nEsse aliado te passa uma mensagem, que deve ser cuidadosamente gravada.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes26[1] = {{'*', 27}};
    cadastrarNo(
    26,
    naoTerminal,
    opcoes26,
    1,
    "A mensagem ?:\n\n\"Haver? um ataque cibern?tico ao IME e todos os ifs ser?o trocados por elses, no dia D + 3, come?ando na SE9!\"\n\nAperte qualquer tecla + <ENTER> para continuar. A mensagem ser? apagada.\n"
    );

    opcao opcoes27[2] = {{'A', 28}, {'B', 29}};
    cadastrarNo(
    27,
    naoTerminal,
    opcoes27,
    2,
    "Continuando a andar em sil?ncio, encontra uma ?rvore ca?da.\nVoc? pode passar por baixo ou por cima do obst?culo: \n\nA- Cima\nB- Baixo\n\nOp??o escolhida: "
    );

    opcao opcoes28[1] = {{'*', 31}};
    cadastrarNo(
    28,
    naoTerminal,
    opcoes28,
    1,
    "Seguindo por cima, voc? faz um barulho ao pisar na casca da ?rvore.\nO inimigo, assim, te plotou e voc? ouviu um disparo.\nRapidamente, voc? saiu correndo para o final da pista.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes29[1] = {{'*', 30}};
    cadastrarNo(
    29,
    naoTerminal,
    opcoes29,
    1,
    "Seguindo por baixo, voc? encontra um caderno e uma caneta para anotar.\nRapidamente, anota a mensagem, ainda fresca ma sua mem?ria.\n\nAperte qualquer tecla + <ENTER> para continuar.\n"
    );

    opcao opcoes30[1] = {{'*', 32}};
    cadastrarNo(
    30,
    naoTerminal,
    opcoes30,
    1,
    "Continuando abaixado, voc? percebe uma aranha em seu bra?o e se assusta.\nLogo, sai correndo para o final da pista.\n\nAperte qualquer tecla + <ENTER> para continuar.\n"
    );

    opcao opcoes31[1] = {{'*', 34}};
    cadastrarNo(
    31,
    naoTerminal,
    opcoes31,
    1,
    "Finalmente, voc? chegou ao final da pista, e encontra o Capit?o.\nO Capit?o pergunta: \"Aluno, qual e mensagem?\".\n\nAperte qualquer tecla + <ENTER> para continuar.\n"
    );

    opcao opcoes32[1] = {{'*', 33}};
    cadastrarNo(
    32,
    naoTerminal,
    opcoes32,
    1,
    "Finalmente, voc? chegou ao final da pista, e encontra o Capit?o.\nO Capit?o pergunta: \"Aluno, qual e mensagem?\".\n\nAperte qualquer tecla + <ENTER> para continuar.\n"
    );

    opcao opcoes33[1] = {{'*', 6}};
    cadastrarNo(
    33,
    naoTerminal,
    opcoes33,
    1,
    "Voc? n?o se lembra exatamente, mas lembra que anotou no caderno. Voc? l? a mensagem para ele e ? liberado.\n\nAperte qualquer tecla + <ENTER> para continuar..\n"
    );

    opcao opcoes34[5] = {{'A', 105}, {'B', 35}, {'C', 105}, {'D', 105}, {'E', 105}};
    cadastrarNo(
    34,
    naoTerminal,
    opcoes34,
    5,
    "Voc? n?o se lembra exatamente, mas tem uma no??o. Qual era a mensagem?\n\nA- Haver? um ataque cibern?tico ao IME e todos os ifs ser?o trocados por elses, no dia D + 3, come?ando na SE6!\nB- Haver? um ataque cibern?tico ao IME e todos os ifs ser?o trocados por elses, no dia D + 3, come?ando na SE9!\nC- Haver? um ataque cibern?tico ao ITA e todos os ifs ser?o trocados por elses, no dia D + 3, come?ando na SE9!\nD- Haver? um ataque cibern?tico ao IME e todos os breaks ser?o trocados por gotos, no dia D + 3, come?ando na SE6!\nE- Haver? um ataque cibern?tico ao IME e todos os breaks ser?o trocados por gotos, no dia D + 2, come?ando na SE9! \n\nDigite a op??o correta para concluir a atividade.\n"
    );

    opcao opcoes105[1] = {{'*', 6}};
    cadastrarNo(
    105,
    terminal,
    opcoes105,
    1,
    "Voc? errou a mensagem. Foi torrado.\nFim de jogo.\n"
    );

    opcao opcoes35[1] = {{'*', 6}};
    cadastrarNo(
    35,
    naoTerminal,
    opcoes35,
    1,
    "Voc? acertou a mensagem e concluiu a progress?o com sucesso.\n\nAperte qualquer tecla + <ENTER> para ir ao pr?ximo dia\n"
    );


    opcao opcoes100[1] = {{'*', 100}};
    cadastrarNo(
    100,
    terminal,
    opcoes100,
    1,
    "Voc? n?o tem uma faca...\nFicou nervoso, se perdeu e foi achado depois pelo Tenente.\nFim de Jogo.\n\n"
    );

    opcao opcoes101[1] = {{'*', 100}};
    cadastrarNo(
    101,
    terminal,
    opcoes101,
    1,
    "Voc? n?o tem uma lanterna...\nFoi torrado pelo tenente.\nFim de Jogo.\n\n"
    );

    opcao opcoes9[1] = {{'*', 36}};
    cadastrarNo(
    9,
    naoTerminal,
    opcoes9,
    1,
    "Agora, n?o tem mais volta: voc? escolheu fazer a marcha, uma intensa caminhada de 8km.\nCalor, cansa?o e fadiga s?o alguma das sensa??es que te acompanham...\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes36[2] = {{'A', 102}, {'B', 37}};
    cadastrarNo(
    36,
    naoTerminal,
    opcoes36,
    2,
    "J? ao iniciar do dia, o comandante de seu pelot?o o promove a xerife.\nA fun??o de xerife exige muita responsabilidade, principalmente na contagem de seu pessoal.\n\nEnt?o, voc? diz:\n\nA- \"Eu desisto, senhor!\"\nB- \"Brasil, Tenente!\"\n\nDigite a op??o selecionada: "
    );

    opcao opcoes102[1] = {{'*', 100}};
    cadastrarNo(
    102,
    terminal,
    opcoes102,
    1,
    "Voc? desistiu...\nFoi torrado pelo tenente.\nFim de Jogo.\n\n"
    );

    opcao opcoes37[3] = {{'A', 38}, {'B', 39}, {'C', 40}};
    cadastrarNo(
    37,
    naoTerminal,
    opcoes37,
    3,
    "Como xerife e com o tempo apertado, voc? s? consegue fazer uma das tr?s a??es abaixo:\n\nA- Pedir um bizu ao xerife antigo\nB- Contar o efetivo\nC- Discursar para seu pelot?o\n\nOp??o escolhida: "
    );

    opcao opcoes38[1] = {{'*', 41}};
    cadastrarNo(
    38,
    naoTerminal,
    opcoes38,
    1,
    "O xerife antigo diz: \n\n\"O Capit?o ordenou que sempre que algu?m baixasse, o canga acompanhasse para ajud?-lo...\nComo agora tem um n?mero par de pessoas, sempre haver? efetivo par\".\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes39[1] = {{'*', 41}};
    cadastrarNo(
    39,
    naoTerminal,
    opcoes39,
    1,
    "O efetivo ? de 35 militares.\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes40[1] = {{'*', 41}};
    cadastrarNo(
    40,
    naoTerminal,
    opcoes40,
    1,
    "Enquanto dava seu discurso, o pelot?o ficou em p? com sua mochila pesada.\nVoc? ficou mal visto pelo seu pelot?o.\n\nAperte qualquer tecla + <ENTER> para continuar\n "
    );

    opcao opcoes41[3] = {{'A', 42}, {'B', 43}, {'C', 44}};
    cadastrarNo(
    41,
    naoTerminal,
    opcoes41,
    3,
    "Agora, ? frente como xerife, voc? deve puxar a velocidade da marcha. Ent?o, inicia:\n\nA- Correndo\nB- Andando r?pido\nC- Caminhando\n\nAperte qualquer tecla + <ENTER> para continuar\n"
    );

    opcao opcoes42[1] = {{'*', 41}};
    cadastrarNo(
    42,
    naoTerminal,
    opcoes42,
    1,
    "Seu pelot?o ficou muito cansado e teve que repetir o percurso.\n\nAperte qualquer tecla + <ENTER> para tentar novamente\n"
    );

    opcao opcoes44[1] = {{'*', 41}};
    cadastrarNo(
    44,
    naoTerminal,
    opcoes44,
    1,
    "O Tenente considerou como acochambra??o sua escolha e te deu um FO-.\n\nAperte qualquer tecla + <ENTER> para tentar novamente\n"
    );

    opcao opcoes43[2] = {{'A', 45}, {'B', 46}};
    cadastrarNo(
    43,
    naoTerminal,
    opcoes43,
    2,
    "O Tenente aceitou e comandou: dire??o ao infinito, sem cad?ncia, marche!\n\nAp?s 2km, chegou a hora do primeiro alto hor?rio, e h? duas ?reas de descan?o:\n\nA- Local ensolarado\nB- Clareira, no meio da mata\n\nEscolha a ?rea de descanso: "
    );

    opcao opcoes45[1] = {{'*', 47}};
    cadastrarNo(
    45,
    naoTerminal,
    opcoes45,
    1,
    "Nessa ?rea, os alunos Amadi, Eccar, Laurinho e Gilson baixaram por insola??o. \nA exposi??o ao sol estava intensa, e eles tiveram que ir emergencialmente na visita m?dica\n\nAperte qualquer tecla + <ENTER> para prosseguir\n"
    );

    opcao opcoes46[1] = {{'*', 47}};
    cadastrarNo(
    46,
    naoTerminal,
    opcoes46,
    1,
    "Nessa ?rea, o aluno Amadi, entusiasta da biologia e experiente com animais, foi picado por uma aranha ao tentar segur?-la.\nRapidamente, a emerg?ncia foi acionada e ele baixou.\nAl?m disso, mais 3 baixaram por cansa?o.\n\nAperte qualquer tecla + <ENTER> para ?prosseguir.\n"
    );

    opcao opcoes47[1] = {{'*', 48}};
    cadastrarNo(
    47,
    naoTerminal,
    opcoes47,
    1,
    "Ap?s 4km de marcha, o pelot?o chega ? travessia do rio. \nPara n?o molhar seu material, voc? precisa impermeabilizar todos seus itens.\n\nAperte qualquer tecla + <ENTER> para prosseguir\n"
    );

    opcao opcoes103[1] = {{'*', 49}};
    cadastrarNo(
    103,
    terminal,
    opcoes103,
    1,
    "Voc? n?o tem um saco imperme?vel. \nDurante a travessia, todo seu material foi encharcado e perdido.\nFim de jogo.\n"
    );

    opcao opcoes48[1] = {{'*', 49}};
    cadastrarNo(
    48,
    naoTerminal,
    opcoes48,
    1,
    "A travessia foi ?rdua, mas ap?s muito esfor?o, conseguiu chegar ao outro lado.\nChegando l?, percebe que os alunos Botenza e Conardo baixaram no meio do caminho.\n\nAperte qualquer tecla + <ENTER> para prosseguir\n"
    );

    opcao opcoes49[2] = {{'A', 50}, {'B', 53}};
    cadastrarNo(
    49,
    naoTerminal,
    opcoes49,
    2,
    "Conclu?da a travessia, o pelot?o segue na marcha.\nNo meio do caminho, foi encontrado um barranco, e voc?, como xerife, tem duas op??es:\n\nA- Passar pelo barranco\nB- Contornar o barranco\n\nEscolha a op??o: "
    );

    opcao opcoes50[2] = {{'A', 51}, {'B', 52}};
    cadastrarNo(
    50,
    naoTerminal,
    opcoes50,
    2,
    "Foi uma escolha desastrosa. No barranco, 13 alunos ca?ram e torceram o p?. Voc?, ainda que cuidadoso, tamb?m trope?ou e caiu junto.\nNo meio da bagun?a, voc? encontra uma barrinha de cereal de algu?m, que caiu no ch?o...\n\nA- Pegar a barrinha\nB- N?o pegar a barrinha\n\nOp??o escolhida: \n"
    );

    opcao opcoes51[2] = {{'A', 52}, {'B', 52}};
    cadastrarNo(
    51,
    naoTerminal,
    opcoes51,
    2,
    "E agora... voc? vai: \n\nA- Comer a barrinha\nB- Guardar a barrinha\n\nOp??o escolhida: "
    );

    opcao opcoes52[1] = {{'*', 54}};
    cadastrarNo(
    52,
    naoTerminal,
    opcoes52,
    1,
    "Atravessado o barranco, tem-se a excelente not?cia que o aluno Amadi, que havia baixado, voltou!\nAinda mais, ele lembrou de voc? e trouxe uma faca para voc?.\n\nAperte qualquer tecla + <ENTER> para pegar a faca\n"
    );

    opcao opcoes53[1] = {{'*', 55}};
    cadastrarNo(
    53,
    naoTerminal,
    opcoes53,
    1,
    "Atravessado o barranco, tem-se a excelente not?cia que o aluno Amadi, que havia baixado, voltou!\n\nAperte qualquer tecla + <ENTER> para pegar a faca\n"
    );

    opcao opcoes55[1] = {{'*', 56}};
    cadastrarNo(
    55,
    naoTerminal,
    opcoes55,
    1,
    "Na ?ltima travessia, de uma ponte, os alunos Henequinho, Batulo e Ivo Lins baixaram junto com seus cangas, por medo de altura...\n\nAperte qualquer tecla + <ENTER> para prosseguir\n"
    );

    opcao opcoes54[1] = {{'*', 57}};
    cadastrarNo(
    54,
    naoTerminal,
    opcoes54,
    1,
    "Na ?ltima travessia, de uma ponte, o aluno Eccar tamb?m voltou!\n\nAperte qualquer tecla + <ENTER> para prosseguir\n"
    );


    opcao opcoes56[5] = {{'A', 105}, {'B', 105}, {'C', 105}, {'D', 105}, {'E', 58}};
    cadastrarNo(
    56,
    naoTerminal,
    opcoes56,
    5,
    "Finalizada a marcha, voc? deve apresentar o pelot?o para o Capit?o.\nQual o efetivo?\n\nA- 26\nB- 17\nC- 21\nD- 27\nE- 24 \n\nDigite a op??o correta para concluir a atividade: \n"
    );

    opcao opcoes57[5] = {{'A', 105}, {'B', 105}, {'C', 105}, {'D', 105}, {'E', 58}};
    cadastrarNo(
    57,
    naoTerminal,
    opcoes57,
    5,
    "Finalizada a marcha, voc? deve apresentar o pelot?o para o Capit?o.\nQual o efetivo?\n\nA- 22\nB- 17\nC- 16\nD- 19\nE- 18 \n\nDigite a op??o correta para concluir a atividade..\n"
    );

    opcao opcoes58[1] = {{'*', 6}};
    cadastrarNo(
    58,
    naoTerminal,
    opcoes58,
    1,
    "Parab?ns, voc? concluiu a atividade!\n\nAperte qualquer tecla + <ENTER> para prosseguir\n"
    );

    opcao opcoes200[1] = {{'*', 6}};
    cadastrarNo(
    200,
    terminal,
    opcoes200,
    1,
    "Voc? chegou ao final do Campo.\n"
    );
}

no *buscarNo(int indice)
{
    no *ptrAux = ptrInicio;

    if(ptrAux == NULL) {
        return NULL;
    }

    else {
        while(ptrAux != NULL) {
            if(ptrAux->indice == indice) {
                return ptrAux;
            }

            ptrAux = ptrAux->proximo;
        }

        return NULL;
    }
}

int lerIndiceProximoNo(char opcao)
{
    if(ptrAtual->opcoes[0].opcaoSelecionada == '*') {
        return ptrAtual->opcoes[0].indiceProximoNo;
    }

    else {
        for(int i = 0; i < ptrAtual->nOpcoes; i++) {
            if(ptrAtual->opcoes[i].opcaoSelecionada == opcao) {
                return ptrAtual->opcoes[i].indiceProximoNo;
            }
        }

        return -1;
    }
}

int isCompra(int indice)
{
    if(indice == 6) {
        return 1;
    }

    else {
        return 0;
    }
}

int isValendoFO(int indice)
{
    if(indice == 12) {
        return 1;
    }

    else if(indice == 44) {
        return 1;
    }

    else {
        return 0;
    }

}

int isValendoItem(int indice)
{
    if(indice == 100) {
        return 1;
    }

    else {
        return 0;
    }
}
int compra(int indice, int moedas, char opcao, itensEssenciais *inventarioEssencial, itensRaros *inventarioRaro)
{
    if(indice == 6) {
        if(opcao == 'A' && moedas >= 110) {
            inventarioEssencial[0] = sacoImpermeavel;
        }

        else if(opcao == 'B' && moedas >= 100) {
            inventarioEssencial[1] = lanterna;
        }

        else if(opcao == 'C' && moedas >= 80) {
            inventarioEssencial[2] = caboSolteiro;
        }

        else {
            printf("\nMoedas Insuficientes!\n");
            fprintf(arquivoSaida, "\nMoedas Insuficientes!\n\n");
            return 0;
        }
    }
}

int verificarItemEssencial(int indice, itensEssenciais *inventarioEssencial)
{
    if(indice == 24) {
        int is24 = 0;
        for(int i = 0; i < 5; i++) {
            if(inventarioEssencial[i] == lanterna) {
                is24++;
            }
        }

        if(is24 == 0) {
            return 0;
        }

        else {
            return 1;
        }
    }

    else if(indice == 47) {
        int is47 = 0;
        for(int i = 0; i < 5; i++) {
            if(inventarioEssencial[i] == sacoImpermeavel) {
                is47++;
            }
        }

        if(is47 == 0) {
            return 0;
        }

        else {
            return 1;
        }
    }

    else {
        return 1;
    }
}

int verificarItemRaro(int indice, itensRaros *inventarioRaro)
{
    if(indice == 23) {
        int is23 = 0;
        for(int i = 0; i < 4; i++) {
            if(inventarioRaro[i] == faca) {
                is23++;
            }
        }

        if(is23 == 0) {
            return 0;
        }

        else {
            return 1;
        }
    }

    else {
        return 1;
    }
}

int isFoundable(int indice)
{
    if(indice == 22) {
        return 1;
    }

    if(indice == 52) {
        return 1;
    }

    if(indice == 51) {
        return 1;
    }

    if(indice == 29) {
        return 1;
    }

    else {
        return 0;
    }
}

void addItemRaro(itensRaros *inventarioRaro, int indice)
{
    if(indice == 22) {
        inventarioRaro[0] = malvinao;
    }

    if(indice == 52) {
        inventarioRaro[2] = faca;
    }

    if(indice == 51) {
        inventarioRaro[1] = barrinhaDeCereal;
    }
}

void addItemEssencial(itensEssenciais *inventarioEssencial)
{
    inventarioEssencial[3] = caderno;
}

void limparTela()
{
    #ifdef LINUX
    system("clear");

    #elif WIN32
    system("cls");

    #else
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    #endif
}

void printInventarioEssencial(itensEssenciais *inventarioEssencial)
{
    for(int i = 0; i < 4; i++) {
        if(inventarioEssencial[i] == sacoImpermeavel) {
            printf("Saco Imperme?vel\n");
            fprintf(arquivoSaida, "Saco Imperme?vel\n");
        }

        if(inventarioEssencial[i] == lanterna) {
            printf("Lanterna\n");
            fprintf(arquivoSaida, "Lanterna\n");
        }

        if(inventarioEssencial[i] == caboSolteiro) {
            printf("Cabo Solteiro\n");
            fprintf(arquivoSaida, "Cabo Solteiro\n");
        }

        if(inventarioEssencial[i] == caderno) {
            printf("Caderno\n");
            fprintf(arquivoSaida, "Caderno\n");
        }
    }
}

void printInventarioRaro(itensRaros *inventarioRaro)
{
    for(int i = 0; i < 3; i++) {
        if(inventarioRaro[i] == malvinao) {
            printf("Malvin?o\n");
            fprintf(arquivoSaida, "Malvin?o\n");
        }

        else if(inventarioRaro[i] == barrinhaDeCereal) {
            printf("Barrinha de cereal\n");
            fprintf(arquivoSaida, "Barrinha de cereal\n");
        }

        else if(inventarioRaro[i] == faca) {
            printf("Faca\n");
            fprintf(arquivoSaida, "Faca\n");
        }
    }
}

void readItensRaros(itensRaros *inventarioRaro, int *counterItensRaros, int *counterFO)
{
    for(int i = 0; i < 3; i++) {
        if(inventarioRaro[i] != itemRaro) {
            counterItensRaros++;
        }
    }

    for(int i = 0; i < 3; i++) {
        if(inventarioRaro[i] == barrinhaDeCereal) {
            counterFO++;
            printf("\nVoc? foi plotado com sua barrinha de cereal guardada. Levou FO-.\n\n");
        }
    }
}
