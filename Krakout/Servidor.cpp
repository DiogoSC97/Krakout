#include "pch.h"
#include "../DLL/DLL.h"
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <atlstr.h>
#include <cstring>

#define TAM 200

int setTop10(TCHAR * top10);
TCHAR * getTop10();
bool iniciaMemJogo(DadosCtrl * cDados);
DWORD WINAPI Thread(LPVOID);
void gotoxy(int x, int y);
int consolaAltura();
int consolaLargura();
DWORD WINAPI trataMensagem(LPVOID * m);
DWORD WINAPI recebeMensagemMem(LPVOID);
void moveJogador(TCHAR * nomeJogador, TCHAR * direcao);
void setupJogo();
void moveBola(int x, int y, int nBola);
int verificaColisaoTijolos(int x, int y);
int verificaColisaoBarreiras(int x, int y);

Jogo j;	//Temp

int _tmain(int argc, TCHAR *argv[]) {
	
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	
	TCHAR top10set[96] = TEXT("Ze-1000,Bob-900,King-800,Manel-700,Carlitos-600,Rafael-500");
	TCHAR * top10get;
	TCHAR resp;
	DWORD threadId; //Id da thread a ser criada
	HANDLE hT; //HANDLE/ponteiro para a thread a ser criada


	setTop10(top10set);
	_tprintf(TEXT("Top 10: %s\n\n"), getTop10());


	//Demo bola
	_tprintf(TEXT("Lancar thread (S/N)?"));
	_tscanf_s(TEXT("%c"), &resp, 1);
	if (resp == 'S' || resp == 's') {

		setupJogo();

		hT = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, NULL, 0, &threadId);
		WaitForSingleObject(hT, INFINITE);
	}

	return 0;
}



int setTop10(TCHAR * top10) {
	HKEY chave;
	DWORD resultado;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Krakout"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &resultado) != ERROR_SUCCESS) {
		return -1;
	}
	else {
		if (resultado == REG_CREATED_NEW_KEY || resultado == REG_OPENED_EXISTING_KEY) {
			RegSetValueEx(chave, TEXT("Top 10"), 0, REG_SZ, (LPBYTE)top10, _tcslen(top10) * sizeof(TCHAR));
		}
		RegCloseKey(chave);
		return 1;
	}
}

TCHAR * getTop10() {
	HKEY chave;
	DWORD resultado, tamanho;
	TCHAR * top10 = new TCHAR[TAM];

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Krakout"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &resultado) != ERROR_SUCCESS) {
		return (TCHAR*)TEXT("Erro ao criar/abrir chave!");
	}
	else {
		if (resultado == REG_OPENED_EXISTING_KEY) {
			tamanho = 200 * sizeof(TCHAR);
			RegQueryValueEx(chave, TEXT("Top 10"), NULL, NULL, (LPBYTE)top10, &tamanho);
			top10[tamanho / sizeof(TCHAR)] = '\0';
			RegCloseKey(chave);
			return top10;
		}
		else if (resultado == REG_CREATED_NEW_KEY) {
			RegCloseKey(chave);
			return (TCHAR*)TEXT("Erro ao criar/abrir chave!");
		}
	}
}



bool iniciaMemJogo(DadosCtrl * cDados) {

	cDados->hMapFileJogo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), TEXT("fmJogo"));
	if (cDados->hMapFileJogo == NULL) {
		_tprintf(TEXT("Erro ao mapear memória partilhada! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->jogo = (Jogo*)MapViewOfFile(cDados->hMapFileJogo, FILE_MAP_WRITE, 0, 0, sizeof(Jogo));
	if (cDados->jogo == NULL) {
		_tprintf(TEXT("Erro ao criar view da memoria!"));
		return 0;
	}

	cDados->hMutexJogo = CreateMutex(NULL, FALSE, TEXT("mutexJogo"));
	if (cDados->hMutexJogo == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hEventJogo = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoJogo"));
	if (cDados->hEventJogo == NULL) {
		_tprintf(TEXT("Erro ao criar o evento relativo ao jogo! (%d)"), GetLastError());
		return FALSE;
	}

	return TRUE;
}


bool iniciaMemMsg(DadosCtrl * cDados) {

	cDados->hMapFileMsg = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), TEXT("fmMsg"));
	if (cDados->hMapFileMsg == NULL) {
		_tprintf(TEXT("Erro ao mapear memória partilhada! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->msg = (MSG_PARTILHADA*)MapViewOfFile(cDados->hMapFileMsg, FILE_MAP_WRITE, 0, 0, sizeof(MSG_PARTILHADA));
	if (cDados->msg == NULL) {
		_tprintf(TEXT("Erro ao criar view da memoria!"));
		return 0;
	}

	cDados->hMutexIndiceMsgIn = CreateMutex(NULL, FALSE, TEXT("mutexMsgIn"));
	if (cDados->hMutexIndiceMsgIn == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hMutexIndiceMsgOut = CreateMutex(NULL, FALSE, TEXT("mutexMsgOut"));
	if (cDados->hMutexIndiceMsgOut == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hSemPodeLer = CreateSemaphore(NULL, 12, 12, TEXT("semaforoMsgPodeLer"));				//Alterar o nº consoante o nº de mensagens
	if (cDados->hSemPodeLer == NULL) {
		_tprintf(TEXT("Erro ao criar o semáforo relativo a ler as mensagens! (%d)"), GetLastError());
		return FALSE;
	}
	
	cDados->hSemPodeEscrever = CreateSemaphore(NULL, 12, 12, TEXT("semaforoMsgPodeEscrever"));		//Alterar o nº consoante o nº de mensagens
	if (cDados->hSemPodeEscrever == NULL) {
		_tprintf(TEXT("Erro ao criar o semáforo relativo a escrever as mensagens! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->hEventMsg = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoMensagem"));
	if (cDados->hEventMsg == NULL) {
		_tprintf(TEXT("Erro ao criar o evento relativo às mensagens! (%d)"), GetLastError());
		return FALSE;
	}

	return TRUE;
}


DWORD WINAPI Thread(LPVOID) {
	int i, x, y, limX, limY;
	DadosCtrl cDados;
	Jogo jogo;
	int flagX, flagY;

	iniciaMemJogo(&cDados);

	srand((int)time(NULL));

	limX = consolaLargura();
	limY = consolaAltura();
	x = rand() % consolaLargura();
	y = rand() % consolaAltura();

	flagX = 0; flagY = 0;
	while (1) {
		if (flagX == 0 && flagY == 0) { x++; y++; }												//			\. 
		else if (flagX == 0 && flagY == 1) { x++; y--; }		//    /'
		else if (flagX == 1 && flagY == 0) { x--; y++; }		//			./
		else if (flagX == 1 && flagY == 1) { x--; y--; }		//    '\

		if (x > limX) flagX = 1;
		if (x < 0)flagX = 0;
		if (y > limY - 2 || verificaColisaoBarreiras(x, y)) flagY = 1;
		if (y < 0 || verificaColisaoTijolos(x, y)) flagY = 0;

		jogo.bolas[0].x = x;
		jogo.bolas[0].y = y;
		
		escreveJogo(&cDados, &jogo);
		Sleep(16.66667);
	}

	return 0;

}

int consolaAltura() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

int consolaLargura() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

DWORD WINAPI recebeMensagemMem(LPVOID) {
	DadosCtrl cDados;

	Mensagem msg;
	BOOL ret;
	DWORD n;
	HANDLE hT;

	while (1) {
		WaitForSingleObject(&cDados.hEventMsg, INFINITE);
		ResetEvent(&cDados.hEventMsg);

		leMsg(&cDados, &msg);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)trataMensagem, &msg, 0, NULL);
	}
}

DWORD WINAPI trataMensagem(LPVOID * m) {

	Mensagem * msg = (Mensagem*)m;
	TCHAR * token;
	const char delimiter[2] = " ";
	TCHAR * pedido[2];
	int n = 0, i;

	for (int i = 0; i < (sizeof(msg->msg)/sizeof(TCHAR)); i++)	{
		if (msg->msg[i] == ' ') {
			pedido[n][i] = '\0';
			n = 1;
		}
		pedido[n][i] = msg->msg[i];
	}

	if (_tcscmp(pedido[0], TEXT("começar"))==0) {
		//iniciar jogo
	}
	else if (_tcscmp(pedido[0], TEXT("move")) == 0) {
		moveJogador(msg->nomeJogador, pedido[1]);
	}
	else if (_tcscmp(pedido[0], TEXT("login")) == 0) {
		for (i = 0; i < 2; i++)	{
			if (_tcscmp(j.jogadores[i].nome, pedido[1]) == 0)
				return 0;		//Envia mensagem a informar que já existe um jogador com este nome
			if (wcscpy_s(j.jogadores[i].nome, TEXT("")) == 0) {
				wcscpy_s(j.jogadores[i].nome, pedido[1]);
				break;
			}
		}
	}

}


void moveBola(int x, int y, int nBola) {

	Bola b = j.bolas[nBola];

	if (x > 0 && x < consolaAltura() && y > 0 && y < consolaLargura()) {			//Verifica limites do campo de jogo
		j.bolas[nBola].x = x;
		j.bolas[nBola].y = y;
	}


}

int verificaColisaoTijolos(int x, int y) {	// 1 - muda direcção	0 - não muda de direcção
	
	for (int i = 0; i < (sizeof(j.tijolos)/sizeof(Tijolo)); i++)				// Verifica colisão com tijolos
	{
		if (x < (j.tijolos[i].x + 35) && x > j.tijolos[i].x && y < (j.tijolos[i].y + 20) && y > j.tijolos[i].y && j.tijolos[i].colisoes > 0) {
			j.tijolos[i].colisoes--;
			return 1;
		}
	}
	return 0;
}

int verificaColisaoBarreiras(int x, int y) {// 1 - muda direcção 	0 - não muda de direcção

	for (int i = 0; i < (sizeof(j.barreiras) / sizeof(Barreira)); i++)				// Verifica colisão com barreiras
	{
		if (y < (j.barreiras[i].y + 9) && y > j.barreiras[i].y && x < (j.barreiras[i].x + j.barreiras[i].tam) && x > j.barreiras[i].x) {
			return 1;
		}
	}
	return 0;
}

void moveJogador(TCHAR * nomeJogador, TCHAR * direcao) {

	int i;
	Barreira bAtual, bNaoAtual;

	for (i = 0; i < (sizeof(j.jogadores) / sizeof(Jogador)); i++)
		if (_tcscmp(j.jogadores[i].nome, nomeJogador) == 0)
			break;


	if (j.barreiras[1].vel == -1) {										// Se só estiver um jogador em campo
		if (_tcscmp(direcao, TEXT("direita")) == 0)
				if (j.barreiras[i].y < consolaLargura())
					j.barreiras[i].y += 1;

		else if (_tcscmp(direcao, TEXT("esquerda")) == 0)
			if (j.barreiras[i].y > 0)
				j.barreiras[i].y -= 1;
	}
	else {
		if (i == 0) {
			bAtual = j.barreiras[i];
			bNaoAtual = j.barreiras[1];
		}
		else {
			bAtual = j.barreiras[i];
			bNaoAtual = j.barreiras[0];
		}

		if (_tcscmp(direcao, TEXT("direita")) == 0)
			if (j.barreiras[i].y < consolaLargura() && (bAtual.y+150+1) < bNaoAtual.y)
				j.barreiras[i].y += 1;

			else if (_tcscmp(direcao, TEXT("esquerda")) == 0)
				if (j.barreiras[i].y > 0 && bAtual.y > (bNaoAtual.y+150+1))
					j.barreiras[i].y -= 1;
	}
}

void setupJogo() {
	
	for (int i = 0; i < 2; i++){
		wcscpy_s(j.jogadores[i].nome, TEXT(""));
		j.barreiras[i].vel = 1;
		j.barreiras[i].tam = 48;
		j.barreiras[i].y = consolaAltura() - 9;
		j.barreiras[i].x = consolaLargura() - j.barreiras[i].tam;
	}

	for (int i = 0; i < 1; i++){
		for (int k = 0; k < (consolaLargura() / 35); k++){
			j.tijolos[i].y = i * 20;
			j.tijolos[i].x = k * 35;
			j.tijolos[i].colisoes = 1;
		}
	}
}