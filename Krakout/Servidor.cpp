#include "pch.h"
#include "../DLL/DLL.h"
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define TAM 200


int setTop10(TCHAR * top10);
TCHAR * getTop10();
bool iniciaMemTeste(DadosCtrl * cDados);
DWORD WINAPI Thread(LPVOID);
void gotoxy(int x, int y);
int consolaAltura();
int consolaLargura();


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
		if (flagX == 0 && flagY == 0) { x++; y++; }
		if (flagX == 0 && flagY == 1) { x++; y--; }
		if (flagX == 1 && flagY == 0) { x--; y++; }
		if (flagX == 1 && flagY == 1) { x--; y--; }

		if (x > limX) flagX = 1;
		if (x < 0) flagX = 0;
		if (y > limY - 2) flagY = 1;
		if (y < 0) flagY = 0;

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