// DLL.cpp : Define as funções exportadas para o aplicativo DLL.
//
#include"stdafx.h"
#include"DLL.h"


void escreveJogo(DadosCtrl * cDados, Jogo * jogo) {
	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	CopyMemory(cDados->jogo, jogo, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexJogo);
	SetEvent(cDados->hEventJogo);
}


void leJogo(DadosCtrl * cDados, Jogo * jogo) {
	WaitForSingleObject(cDados->hEventJogo, INFINITE);
	ResetEvent(cDados->hEventJogo);
	WaitForSingleObject(cDados->hMutexJogo, INFINITE);
	CopyMemory(jogo, cDados->jogo, sizeof(Jogo));
	ReleaseMutex(cDados->hMutexJogo);
}

void leMsg(DadosCtrl * cDados, Mensagem * msg){
	int pos;

	WaitForSingleObject(cDados->hSemPodeLer, INFINITE);
	WaitForSingleObject(cDados->hMutexIndiceMsgOut, INFINITE);

	pos = cDados->msg->out;
	cDados->msg->out++;

	ReleaseMutex(cDados->hMutexIndiceMsgOut);
	
	CopyMemory(msg, &cDados->msg->msgs[pos], sizeof(Mensagem));

	ReleaseSemaphore(cDados->hSemPodeLer,1,NULL);


}

void escreveMsg(DadosCtrl * cDados, Mensagem * msg){

	int pos;

	WaitForSingleObject(cDados->hSemPodeEscrever, INFINITE);
	WaitForSingleObject(cDados->hMutexIndiceMsgIn, INFINITE);

	pos = cDados->msg->in;
	cDados->msg->in++;

	ReleaseMutex(cDados->hMutexIndiceMsgIn);

	CopyMemory(&cDados->msg->msgs[pos], msg, sizeof(Mensagem));

	ReleaseSemaphore(cDados->hSemPodeEscrever, 1, NULL);

}
