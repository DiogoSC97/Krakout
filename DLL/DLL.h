#pragma once
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <conio.h>

typedef struct {
	int x, y;
}Bola;

typedef struct {
	HANDLE hMapFileTeste, hMutexTeste;
	Bola * bola;
}DadosCtrl;

#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif
extern "C"
{
	//Funções a serem exportadas/importadas
	DLL_IMP_API void escreveBola(DadosCtrl * cDados, Bola * bola);
	DLL_IMP_API void leBola(DadosCtrl * cDados, Bola * bola);
}