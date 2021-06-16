#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <stdbool.h>
#include <locale.h>

#define N_maxitems 3

// funcoes
int menu();
void jogo();
void opcoes();
void titulo_dados(char*);
int item_indisponivel(int);
reset();


typedef struct {
	int n_items;
	char items[N_maxitems][15]; // 3 * 15 * 1 
	int wins;
	int pontos;
} session;
session player;

int max = 10;
int items_vendidos[5] = { 0,0,0,0,0 }; 
char utilizador[25] = "user"; 
char novo_utilizador[25];
bool utilizador_inserido;

void titulo_dados(char* title)
{
	system("cls");
	int x;
	for (x = 0; x < 15; x++)
		printf("-*-*");

	printf("\n\n\t\t\t%s\n\n", title);

	for (x = 0; x < 15; x++)
		printf("-*-*");

	printf("\n\n\t\t\t\t\t\tNome: %s", utilizador_inserido ? novo_utilizador : utilizador);
	printf("\n\t\t\t\t\t\tPontos: %d", player.pontos);
	printf("\n\t\t\t\t\t\tVitorias: %d", player.wins);
	printf("\n\t\t\t\t\t\tItems: [%d / %d]", player.n_items, N_maxitems);
	printf("\n\t\t\t\t\t\tModo: [%d - %d]", 0, max);
}

int n; // var global para conseguir ler no cheat engine.. 
void jogo()
{
	srand(time(NULL));
	n = rand() % max;
	int correct = 0, tentativas = 0;
	int guess;
	int wPontos = 0;

	titulo_dados("Jogo do adivinha");

	printf("\nAdivinha o número (%d - %d) > ", 0, max);

	do {
		scanf_s("%d", &guess);
		if (guess == n) {
			tentativas++;

			if (tentativas == 1)
				wPontos = 50;
			else if (tentativas > 1 && tentativas < 5)
				wPontos = 25;
			else
				wPontos = 10;

			player.pontos += wPontos;
			player.wins++;
			titulo_dados("Jogo do adivinha");

			printf("\n\nParabéns! Adivinhaste em %d tentativas. Ganhaste %d pontos!\n", tentativas, wPontos);

			correct++;
		}

		if (guess < n) {
			tentativas++;
			printf("\nTenta um número maior > ");
		}

		if (guess > n) {
			tentativas++;
			printf("\nTenta um número menor > ");
		}

	} while (correct == 0);

	printf("\n\n");
	printf("Voltando ao menu em ");
	int x;
	for (x = 5; x >= 0; x--)
	{
		printf("%d...", x);
		Sleep(1000);
	}

	menu();
}

loja()
{
	titulo_dados("Loja");
	char weapons[5][40] = {
									"Faca",
									"Chapeu",
									"Casaco",
									"Arma",
									"Carro"
	};

	char(*wpPtr)[40] = weapons;

	if (player.n_items >= 3)
	{
		printf("\n\nJA TEM 3 ITEMS, NĂO PODE COMPRAR MAIS, A NĂO SER QUE MUDE O NOME DE UTILIZADOR.\n");
		Sleep(3500);
		menu();
	}

	printf("\n");
	int x;
	for (x = 0; x < 5; x++)
		printf("%d. %s (%s)\n", x + 1, *(wpPtr + x), items_vendidos[x] ? "VENDIDO" : "DISPONIVEL");

	printf("\n0. Sair da loja\n\n\t> ");

	int opc_loja;
	scanf_s("%d", &opc_loja);

	switch (opc_loja)
	{
	case 0:
		menu();
		system("pause");
		break;
	case 1:
		item_indisponivel(0);
		break;
	case 2:
		item_indisponivel(1);
		break;
	case 3:
		item_indisponivel(2);
		break;
	case 4:
		item_indisponivel(3);
		break;
	case 5:
		item_indisponivel(4);
		break;
	}

	loja();
}

item_indisponivel(int num)
{
	if (items_vendidos[num] == 1)
	{
		printf("\nErro!! Item indisponivel\n");
		Sleep(2000);
		loja();
	}
	else

	items_vendidos[num] = 1;
	player.n_items++;
}

menu()
{
	titulo_dados("Jogo do adivinha");
	int opc;
	printf("\n\n\n1. Jogar\t\t2. Regras & Opcőes\t\t3. Loja\t\t0. Sair\n\n\t> ");
	scanf("%d", &opc);

	switch (opc)
	{
	case 0:
		return 0;
	case 1:
		jogo();
		break;
	case 2:
		opcoes();
		break;
	case 3:
		loja();
		break;
	default:
		printf("\n\nErro!\n");
		menu();
		break;

	}
}

void opcoes()
{
	titulo_dados("Regras & Opcoes");
	int opc;
	printf("\nREGRAS: \n");
	printf("\nO jogo consiste em adivinhar um número.");
	printf("\nSe adivinhar o número, ganha pontos, dependendo do número de tentativas.");
	printf("\nSe mudar de nome, os pontos dăo reset.");
	printf("\nO número maximo de items é 3 !!!.");
	printf("\nCada vez que fecha o jogo, os pontos e a quantidade de items resetam.");
	printf("\nOs items não fazem nada\n");

	printf("\nOPCOES: \n");
	printf("\n1. Aumentar a quantidade de números.\n");
	printf("2. Mudar o nome de utilizador.\n");
	printf("3. Dar reset a tudo.\n");

	printf("0. Voltar ao menu.\n\n>");
	scanf("%d", &opc);

	if (opc == 1) {
		printf("Digite o novo número máximo: ");
		scanf_s("%d", &max);
	}

	if (opc == 2) {
		printf("\nDigite o novo novo de utilizador: ");
		scanf("%s", novo_utilizador);
		utilizador_inserido = true;
		int x;
		for (x = 0; x < 5; x++)
			items_vendidos[x] = 0;
		player.n_items = 0;
		player.wins = 0;
		player.pontos = 0;
	}

	if (opc == 3)
		reset();

	if (opc == 0)
		menu();

	opcoes();
}

reset()
{
	utilizador_inserido = false;
	int x;
	for (x = 0; x < 5; x++)
		items_vendidos[x] = 0;
	player.n_items = 0;
	player.wins = 0;
	player.pontos = 0;
	max = 11;
}

int main()
{
	setlocale(LC_ALL, "Portuguese");

	menu();

	printf("\n\n");
	system("pause");
}
