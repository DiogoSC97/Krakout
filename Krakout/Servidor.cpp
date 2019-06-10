#include "pch.h"
#include "../DLL/DLL.h"
#include "..\DLL\stdafx.h"
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
DWORD WINAPI trataMensagem(LPVOID * m);
DWORD WINAPI recebeMensagemMem(LPVOID);
void moveJogador(TCHAR * nomeJogador, TCHAR * direcao);
void setupJogo();
void moveBola(int x, int y, int nBola);
int verificaColisaoTijolos(int x, int y);
int verificaColisaoBarreiras(int x, int y);
int verificaPerdeVida(int x, int y);

Jogo j;	//Temp
HANDLE hMutexJogo;
int limX, limY;

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
	HANDLE hT,hTMsg; //HANDLE/ponteiro para a thread a ser criada

	limX = CLIENT_WIDTH - 45;
	limY = CLIENT_HEIGHT - 85;

	hMutexJogo = CreateMutex(NULL, FALSE, TEXT("mutexJogoGlobal"));
	if (hMutexJogo == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	setTop10(top10set);
	_tprintf(TEXT("Top 10: %s\n\n"), getTop10());


	//Demo bola
	_tprintf(TEXT("Lancar thread (S/N)?"));
	_tscanf_s(TEXT("%c"), &resp, 1);
	if (resp == 'S' || resp == 's') {

		setupJogo();

		hT = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, NULL, 0, &threadId);
		hTMsg = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recebeMensagemMem, NULL, 0, &threadId);

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
	int i, x, y;
	DadosCtrl cDados;
	Jogo jogo;
	int flagX, flagY;

	iniciaMemJogo(&cDados);

	srand((int)time(NULL));

	x = rand() % limX;
	y = rand() % limY;

	flagX = 0; flagY = 0;
	while (1) {
		if (flagX == 0 && flagY == 0) { x++; y++; }												//			\. 
		else if (flagX == 0 && flagY == 1) { x++; y--; }		//    /'
		else if (flagX == 1 && flagY == 0) { x--; y++; }		//			./
		else if (flagX == 1 && flagY == 1) { x--; y--; }		//    '\

		if (x > limX) flagX = 1;
		if (x < 0)flagX = 0;
		if (y > limY - 2 || verificaColisaoBarreiras(y, x)) flagY = 1;
		if (y < 0 || verificaColisaoTijolos(y, x)) flagY = 0;

		j.bolas[0].x = y;
		j.bolas[0].y = x;
		
		escreveJogo(&cDados, &j);
		Sleep(4.09);
	}

	return 0;

}

DWORD WINAPI recebeMensagemMem(LPVOID) {
	DadosCtrl cDados;
	Mensagem msg, msg_aux;
	BOOL ret;
	DWORD n;
	HANDLE hT;

	iniciaMemMsg(&cDados);

	wcscpy_s(msg_aux.msg, TEXT("login teste"));
	wcscpy_s(msg_aux.nomeJogador, TEXT("teste"));
	escreveMsg(&cDados, &msg_aux);

	wcscpy_s(msg_aux.msg, TEXT("move direita"));
	wcscpy_s(msg_aux.nomeJogador, TEXT("teste"));
	escreveMsg(&cDados, &msg_aux);

	while (1) {

		leMsg(&cDados, &msg);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)trataMensagem, &msg, 0, NULL);
	}
}

DWORD WINAPI trataMensagem(LPVOID * m) {

	Mensagem * msg = (Mensagem*)m;
	TCHAR * token;
	const char delimiter[2] = " ";
	TCHAR pedido[2][24];
	int n = 0, i, pos=0;

	for (int i = 0; i < (sizeof(msg->msg)/sizeof(TCHAR)); i++)	{
		if (msg->msg[i] == ' ') {
			pedido[n][i] = '\0';
			n = 1;
			pos = i + 1;
			continue;
		}
		if (msg->msg[i] == '\0') {
			pedido[n][i-pos] = msg->msg[i];
			break;
		}
			
		pedido[n][i-pos] = msg->msg[i];
	}

	WaitForSingleObject(hMutexJogo, INFINITE);

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
	ReleaseMutex(hMutexJogo);
}


void moveBola(int x, int y, int nBola) {

	Bola b = j.bolas[nBola];

	if (x > 0 && x < limX && y > 0 && y < limY) {			//Verifica limites do campo de jogo
		j.bolas[nBola].x = x;
		j.bolas[nBola].y = y;
	}


}

int verificaColisaoTijolos(int x, int y) {	// 1 - muda direcção	0 - não muda de direcção
	
	for (int i = 0; i < (sizeof(j.tijolos)/sizeof(Tijolo)); i++)				// Verifica colisão com tijolos
	{
		if (x < (j.tijolos[i].x + 35) && (x + 25) > j.tijolos[i].x && y < j.tijolos[i].y && j.tijolos[i].colisoes > 0) {
			j.tijolos[i].colisoes--;
			return 1;
		}
	}
	return 0;
}

int verificaColisaoBarreiras(int x, int y) {// 1 - muda direcção 	0 - não muda de direcção

	for (int i = 0; i < (sizeof(j.barreiras) / sizeof(Barreira)); i++)				// Verifica colisão com barreiras
	{
		if (x < (j.barreiras[i].x + BARREIRA_BIG_WIDTH) && (x + 25) > j.barreiras[i].x && (y + 25) > j.barreiras[i].y) {
			return 1;
		}
	}
	return 0;
}

int verificaPerdeVida(int x, int y) {

	if ((y + 25) > limY) {
		return 1;
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
			j.barreiras[i].y += 10;									//troquei

		else if (_tcscmp(direcao, TEXT("esquerda")) == 0)
			j.barreiras[i].y -= 10;									//troquei
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
			if (bAtual.x < limX && (bAtual.x + BARREIRA_BIG_WIDTH + 1) < bNaoAtual.x)
				j.barreiras[i].x += 1;

			else if (_tcscmp(direcao, TEXT("esquerda")) == 0)
				if (bAtual.x > 0 && bAtual.x > (bNaoAtual.x + BARREIRA_BIG_WIDTH + 1))
					j.barreiras[i].x -= 1;
	}

}

void setupJogo() {
	
	for (int i = 0; i < 2; i++){
		wcscpy_s(j.jogadores[i].nome, TEXT(""));
		j.barreiras[i].vel = 10;
		j.barreiras[i].tam = BARREIRA_BIG_WIDTH;
		j.barreiras[i].x = limY - BARREIRA_BIG_HEIGHT;							//troquei
		j.barreiras[i].y = limX - BARREIRA_BIG_WIDTH;			//troquei
	}
	j.barreiras[1].vel = -1;
	for (int i = 0; i < 1; i++){
		for (int k = 0; k < (limX / TIJOLO_BIG_WIDTH); k++){
			j.tijolos[k].x = i * TIJOLO_NORMAL_HEIGHT;				//troquei
			j.tijolos[k].y = k * TIJOLO_NORMAL_WIDTH;				//troquei
			j.tijolos[k].colisoes = 1;
		}
	}
}