// DLL.cpp : Define as funções exportadas para o aplicativo DLL.
//
#include"stdafx.h"
#include"DLL.h"


void escreveJogo(DadosCtrl * cDados, Jogo * jogo) {
	WaitForSingleObject(cDados->hMutexTeste, INFINITE);
	CopyMemory(cDados->jogo, jogo, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexTeste);
}


void leJogo(DadosCtrl * cDados, Jogo * jogo) {
	WaitForSingleObject(cDados->hMutexTeste, INFINITE);
	CopyMemory(jogo, cDados->jogo, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexTeste);
}