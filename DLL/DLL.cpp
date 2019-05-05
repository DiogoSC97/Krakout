// DLL.cpp : Define as funções exportadas para o aplicativo DLL.
//
#include"stdafx.h"
#include"DLL.h"


void escreveBola(DadosCtrl * cDados, Bola * bola) {
	WaitForSingleObject(cDados->hMutexTeste, INFINITE);
	CopyMemory(cDados->bola, bola, sizeof(Bola));
	ReleaseMutex(cDados->hMutexTeste);
}


void leBola(DadosCtrl * cDados, Bola * bola) {
	WaitForSingleObject(cDados->hMutexTeste, INFINITE);
	CopyMemory(bola, cDados->bola, sizeof(Bola));
	ReleaseMutex(cDados->hMutexTeste);
}