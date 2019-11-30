#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <locale.h>

// Mesma struct do nosso programa Target
#define N_maxitems 3

typedef struct {
	int n_items;
	char items[N_maxitems][15]; // 3 * 15 * 1 
	int wins;
	int pontos;
} session;


HANDLE hProc; /* Handle é basicamente um pointer, aponta para algo, neste caso,
				 esta var irá apontar para o nosso processo (jogo) e guardá-lo  */
DWORD pID; // retorna um long int. Neste caso, irá ser o nosso process ID

// Com esta função, iremos conseguir o endereço base do nosso programa. (diferente de process ID)
uintptr_t getModuleBaseAddress(DWORD procId, const char* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId); // Tira uma "snapshot" aos módulos

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (strstr(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}

			} while (Module32Next(hSnap, &modEntry));
		}
	}

	CloseHandle(hSnap);

	return modBaseAddr;
}

void aprocessar() // visuais...
{
	printf("\nA processar...."); Sleep(2000);
	printf("\tConcluido!");		 Sleep(1500);
}


void getProcess(char* processName) // Basicamente, vamos usar esta função para encontrar o nosso processo (apartir do nome) e daí, conseguir o processID.
{
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);

	HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Tira um "snapshot" aos processos

	if (hProcSnap == INVALID_HANDLE_VALUE) {
		printf("Erro! Não foi possivel receber os processos através do snapshot.\n");
		return 0;
	}

	while (Process32Next(hProcSnap, &ProcEntry)) // Enquanto houver um processo... (loop á lista dos processos)
	{
		//printf("\n%s\n", ProcEntry.szExeFile); // Printa o nome do processo // Usado para testes

		if (!strcmp(processName, ProcEntry.szExeFile))
		{
			printf("\nProcesso %s encontrado! ID: %d", ProcEntry.szExeFile, ProcEntry.th32ProcessID);
			hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcEntry.th32ProcessID); // Ganhar acesso ao process ( endereços de memória, etc... )
			pID = ProcEntry.th32ProcessID; // Process ID

			if (hProc == NULL)
				printf("\nErro! Não foi possivel receber o ID do processo %s\n", ProcEntry.szExeFile);

			CloseHandle(hProcSnap); // Parar a snapshot
			Sleep(2500);
			return 1; // Sai da função getProcess com sucesso.
		}
	}

	// Caso dê erro
	printf("Erro! Não foi possivel encontrar o processo %s através do snapshot.", processName);
	CloseHandle(hProcSnap);
	return 0;
}

int main()
{
	setlocale(LC_ALL, "Portuguese");

	getProcess((char*)"Target.exe");

	uintptr_t processBaseAddress = getModuleBaseAddress(pID, "Target.exe");
	// Basicamente, vai nos dar o número base do nosso processo (diferente de process ID)

	uintptr_t guess = processBaseAddress + 0x1B174; // Temos que fazer esta soma, pois cada vez que compilamos o nosso Target, ele irá mudar os endereços das variaveis e o endereço base também
	// guess = n									// Porém, com a nossa função getModuleBaseAddress(), conseguimos sempre ter o endereço base atualizado
													// Neste caso, 0x1B174 é o nosso "Offset", a soma deles irá dar o endereço da variavel que queremos.

	int myguess, newguess; 							// Iremos usar estas variáveis para armazenar valores
	

	uintptr_t points = processBaseAddress + 0x1B518;
	int mypoints;

	uintptr_t wins = processBaseAddress + 0x1B514;
	int mywins;

	uintptr_t max = processBaseAddress + 0x1B000;
	int oldmax, mymax;

	session player;
	uintptr_t dwPlayerAddress = processBaseAddress + 0x1B4E0; // Aqui começa a struct session, por exemplo, se int n_items tiver a pos 996 -> 1000 (int = 4 bytes), 
															  // char items[3][15] vai ser de 1000 a 1045 (char = 1 byte) 
															  // 

	system("cls");

	int opc;
	do {

		ReadProcessMemory(hProc, (LPVOID)dwPlayerAddress, &player, sizeof(player), 0); // Vai ler todos os valores da nossa struct armazena-los em player.

		// Informaçoes, onde usamos também a struct, e onde mostra que endereço base =\= process id
		printf("Pontos: %i ||| Vitórias: %i ||| N_items: %i ||| Endereço base: (%p) ||| Process ID: (%d) \n", player.pontos, player.wins, player.n_items, processBaseAddress, pID);
		
		printf("\nO que deseja fazer?\n\n");
		printf("1. Ver qual é o número\n");
		printf("2. Alterar o número\n");
		printf("3. Mudar os pontos\n");
		printf("4. Alterar o número de vitórias\n");
		printf("5. Alterar o número máximo\n");
		printf("6. Refresh\n");
		printf("0. Sair\n\n\t>");
		scanf_s("%d", &opc);

		switch (opc)
		{
			case 1:
				ReadProcessMemory(hProc, (LPVOID)guess, &myguess, sizeof(myguess), 0); // Neste caso, estamos a ler no programa Target (hProc), a variável guess (n), e armazenamos em myguess
																					// Por exemplo, se n = 6, guess = 6 -> myguess = 6
				printf("\nO número é %d\n", myguess);
				Sleep(2500);
				break;

			case 2:
				ReadProcessMemory(hProc, (LPVOID)guess, &myguess, sizeof(myguess), 0); // Fazemos o mesmo de á bocado para sabermos qual é o numero
				printf("\nO número atual é %d", myguess);

				printf("\nDigite o novo número que deseja > ");
				scanf_s("%d", &newguess);
				WriteProcessMemory(hProc, (PVOID)guess, &newguess, sizeof(newguess), 0); // Aqui iremos mudar o valor da variavel n, para newguess

				aprocessar(); break; // Manter o código "clean"

			case 3:
				printf("\nDigite a nova quantia de pontos que deseja > ");
				scanf_s("%d", &mypoints);
				WriteProcessMemory(hProc, (PVOID)points, &mypoints, sizeof(mypoints), 0); // O processo repete-se, pedimos o novo valor e substituimos com WriteProcessMemory.

				aprocessar(); break;

			case 4:
				printf("\nDigite quantas vitórias deseja > ");
				scanf_s("%d", &mywins);
				WriteProcessMemory(hProc, (PVOID)wins, &mywins, sizeof(mywins), 0);

				aprocessar(); break;

			case 5:
				ReadProcessMemory(hProc, (LPVOID)max, &oldmax, sizeof(oldmax), 0); // Aqui lemos no programa target o valor máximo
				printf("\nNúmero máximo atual: %d\n", oldmax);		

				printf("Digite o novo número máximo que deseja > ");			   // Pedimos o novo valor
				scanf_s("%d", &mymax);
				WriteProcessMemory(hProc, (PVOID)max, &mymax, sizeof(mymax), 0);	// E substituimos

				aprocessar(); break;

			case 6:
				system("cls");
				break;

			case 0: 
				return 0;

			default:
				printf("Erro!"); Sleep(2500); break;
		}

		system("cls");

	} while (opc != 0);

	return 0;
}

// ReadProcessMemory(hProc, (LPVOID)memoryAdress, &valor, sizeof(valor), 0);
// WriteProcessMemory(hProc, (PVOID)memoryAdress, &valor, sizeof(valor), 0);
