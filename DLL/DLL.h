#pragma once
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <conio.h>

typedef struct {
	int x, y;
	float vel;
	HBITMAP bm1, bm2;
}Bola;

typedef struct {
	int x, y, tam;
	float vel;
	HBITMAP bm1, bm2;
}Barreira;

typedef struct {
	TCHAR tipo[12];	//Speed-up	-ou-	Slow-down	-ou-	Vida-extra	-ou-	Triple
	int x, y, tam, duracao;
	float vel;
	HBITMAP bm1, bm2;
}Brinde;

typedef struct {
	TCHAR tipo[12];	//normal	-ou-	resistente	-ou-	m�gico
	int x, y, tam, colisoes;
	//float vel;
	HBITMAP bm1, bm2;
	Brinde brinde;
}Tijolo;



typedef struct {
	TCHAR nome[24];
	int pontuacao;
}Jogador;

typedef struct {
	Barreira barreiras[2]; //Max 2 jogadores. Pode sofrer altera��es.	Jogador[0] -> Barreira[0] e Jogador[0] -> Barreira[0]
	Tijolo tijolos[24];	//Max 24 tijolos. Pode sofrer altera��es.
	Brinde brindes[6]; //Max 6 brindes a cair. Pode sofrer altera��es.
	Bola bolas[3]; //Max 3 bolas em jogo(1 normal, 3 com powerup). Pode sofrer altera��es (por causa do multiplayer).
	Jogador jogadores[2]; //Max 3 bolas em jogo(1 normal, 3 com powerup). Pode sofrer altera��es.
}Jogo;

typedef struct {
	char nomeJogador[24];
	char msg[24];				//Alterar depois se necess�rio
}Mensagem;

typedef struct {
	char msgs[12];				//Alterar depois se necess�rio
	int in, out;
}MSG_PARTILHADA;

typedef struct {
	HANDLE hMapFileJogo, hMutexJogo, hEventJogo;	//Mudar nome do Mutex depois da demonstra��o
	HANDLE hMapFileMsg, hSemPodeEscrever, hSemPodeLer, hMutexIndiceMsgIn, hMutexIndiceMsgOut;
	Jogo * jogo;
	MSG_PARTILHADA * msg;
}DadosCtrl;

#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif
extern "C"
{
	//Fun��es a serem exportadas/importadas
	DLL_IMP_API void escreveJogo(DadosCtrl * cDados, Jogo * jogo);
	DLL_IMP_API void leJogo(DadosCtrl * cDados, Jogo * jogo);
	
	DLL_IMP_API void leMsg(DadosCtrl * cDados, Mensagem * msg);
	DLL_IMP_API void escreveMsg(DadosCtrl * cDados, Mensagem * msg);
}