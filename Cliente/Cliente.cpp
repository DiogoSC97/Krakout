// Cliente.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h"
#include "../DLL/DLL.h"
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>


bool iniciaMemTeste(DadosCtrl * cDados);
void gotoxy(int x, int y);

int _tmain(int argc, TCHAR *argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	DadosCtrl cDados;
	Jogo jogo;

	iniciaMemTeste(&cDados);

	
	while (true)
	{
		leJogo(&cDados, &jogo);
		system("cls");
		gotoxy(jogo.bolas[0].x, jogo.bolas[0].y);
		_tprintf(TEXT("O"));
	}
	
	
}

bool iniciaMemTeste(DadosCtrl * cDados) {

	cDados->hMapFileTeste = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), TEXT("fmTeste"));
	if (cDados->hMapFileTeste == NULL) {
		_tprintf(TEXT("Erro ao mapear memória partilhada! (%d)"), GetLastError());
		return FALSE;
	}

	cDados->jogo = (Jogo*)MapViewOfFile(cDados->hMapFileTeste, FILE_MAP_WRITE, 0, 0, sizeof(Jogo));

	cDados->hMutexTeste = CreateMutex(NULL, FALSE, TEXT("mutexTeste"));
	if (cDados->hMutexTeste == NULL) {
		_tprintf(TEXT("Erro ao criar o mutex! (%d)"), GetLastError());
		return FALSE;
	}

	return TRUE;
}

void gotoxy(int x, int y) {
	static HANDLE hStdout = NULL;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	if (!hStdout)
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hStdout, coord);
}
