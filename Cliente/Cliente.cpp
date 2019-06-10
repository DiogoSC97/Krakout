
#include "pch.h"
#include "..\DLL\DLL.h"
#include "..\DLL\stdafx.h"
#include "resource.h"
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <wingdi.h>
#include <stdio.h>
#include <cstring>
#include <atlstr.h>
#include <wchar.h>
#include <string.h>


#pragma region GLOBALVARIABLES

DadosCtrl cDados;
Jogo jogo;
HWND hWndBkp;

// UI
HBITMAP hInitBG;
HDC hDeviceC, hDeviceCM;
HDC hBufferDC, hdc, bufferDC, hWindowDC;
HBITMAP hBufferBitmap;
PAINTSTRUCT paintstruct;


//Recursos
BITMAP bitmaps[NUM_BITMAPS];
HBITMAP hBitmaps[NUM_BITMAPS];
HDC hDeviceContexts[NUM_BITMAPS];


// Vari�veis de controlo
BOOL continuar;										// Variável de controlo das threads
int estado;											// Estado do cliente (inicial, a aguardar jogo, em jogo)
int KEY_LEFT, KEY_RIGHT;							// Teclas para jogo


// Dados THREADS
HANDLE hT_GetDadosJogo;
#pragma endregion

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
bool iniciaMemJogo(DadosCtrl* cDados);
bool iniciaMemMsg(DadosCtrl* cDados);
void iniciaVariaveis(); 
void iniciaThreads();
void iniciaDoubleBuffer(HWND hWnd);
void loadUIResources(HWND hWnd);
void buildImageOnBuffer(HWND hWnd);
void buildMapOnBuffer(HWND hWnd);
DWORD WINAPI t_GetDadosJogo(LPVOID param);


// Nome da classe da janela (para programas de uma só janela, normalmente este nome é
// igual ao do próprio programa) "szprogName" é usado mais abaixo na definição das
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Base");

// Parâmetros:
// hInst: Gerado pelo Windows, é o handle (número) da instância deste programa
// hPrevInst: Gerado pelo Windows, é sempre NULL para o NT (era usado no Windows 3.1)
// lpCmdLine: Gerado pelo Windows, é um ponteiro para uma string terminada por 0
// destinada a conter parâmetros para o programa
// nCmdShow: Parâmetro que especifica o modo de exibição da janela (usado em
// ShowWindow()


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd; // hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg; // MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp; // WNDCLASSEX é uma estrutura cujos membros servem para definir as características da classe da janela


	// ============================================================================
	// 1. Definição das características da janela "wcApp"
	// (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);									// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;											// Instância da janela actualmente exibida ("hInst" é parâmetro de WinMain e vem inicializada daí)
	wcApp.lpszClassName = szProgName;									// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;									// Endereço da função de processamento da janela // ("TrataEventos" foi declarada no início e // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;								// Estilo da janela: Fazer o redraw se for // modificada horizontal ou verticalmente
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));			// "hIcon" = handler do ícon normal //"NULL" = Icon definido no Windows // "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));		// "hIconSm" = handler do ícon pequeno //"NULL" = Icon definido no Windows // "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR1));	// "hCursor" = handler do cursor (rato) // "NULL" = Forma definida no Windows // "IDC_ARROW" Aspecto "seta"
	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);					// Classe do menu que a janela pode ter (NULL = não tem menu)
	wcApp.cbClsExtra = 0;												// Livre, para uso particular
	wcApp.cbWndExtra = 0;												// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);			// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por "GetStockObject".Neste caso o fundo será branco

																
	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);


	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName,				// Nome da janela (programa) definido acima
		TEXT("KRAKOUT"),		// Texto que figura na barra do título
		WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		initWindowPosLeft,			// Posição x pixels (default=à direita da última)
		initWindowPosTop,			// Posição y pixels (default=abaixo da última)
		CLIENT_WIDTH,					// Largura da janela (em pixels)
		CLIENT_HEIGHT,					// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,		// handle da janela pai (se se criar uma a partir de outra) ou HWND_DESKTOP se a janela for a primeira, criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da instância do programa actual ("hInst" é passado num dos parâmetros de WinMain()
		0);						// Não há parâmetros adicionais para a janela


	// ============================================================================
	// 4. Mostrar a janela
	// ============================================================================
	ShowWindow(hWnd, nCmdShow);		// "hWnd"= handler da janela, devolvido por "CreateWindow"; "nCmdShow"= modo de exibição (p.e. normal/modal); é passado como parâmetro de WinMain()
	UpdateWindow(hWnd);				// Refrescar a janela (Windows envia à janela uma mensagem para pintar, mostrar dados, (refrescar)…

	iniciaMemJogo(&cDados);
	iniciaMemMsg(&cDados); 
	iniciaVariaveis();
	iniciaThreads();

	hWndBkp = hWnd;
	

	// ============================================================================
	// 5. Loop de Mensagens
	// ============================================================================
	// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa fila de
	// espera até que GetMessage(...) possa ler "a mensagem seguinte"
	// Parâmetros de "getMessage":
	// 1)"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no início de WinMain()):
		// HWND hwnd handler da janela a que se destina a mensagem
		// UINT message Identificador da mensagem
		// WPARAM wParam Parâmetro, p.e. código da tecla premida
		// LPARAM lParam Parâmetro, p.e. se ALT também estava premida
		// DWORD time Hora a que a mensagem foi enviada pelo Windows
		// POINT pt Localização do mouse (x, y)
	// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem // receber as mensagens para todas as janelas pertencentes à thread actual)
	// 3)Código limite inferior das mensagens que se pretendem receber
	// 4)Código limite superior das mensagens que se pretendem receber
	// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
	// terminando então o loop de recepção de mensagens, e o programa
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg); // Pré-processamento da mensagem (p.e. obter código ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que aguarda até que a possa reenviar à função de tratamento da janela, CALLBACK TrataEventos (abaixo)
	}
	
	
	// ============================================================================
	// 6. Fim do programa
	//==================================================================
	return((int)lpMsg.wParam); // Retorna sempre o parâmetro wParam da estrutura lpMsg
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC hdc; static TCHAR c;
	static int x = 10, y = 10, xi = 0, yi = 0, xf = 0, yf = 0;
	static HBITMAP hbmp;
	static HDC memdc; //Device context que guarda tudo o que se imprime no fundo da janela
	HBITMAP fundo;
	
	switch (messg) { 

	case WM_CREATE:{

		hbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_TAMARELO));
		hdc = GetDC(hWnd);
		memdc = CreateCompatibleDC(hdc);
		fundo = CreateCompatibleBitmap(hdc, CLIENT_WIDTH, CLIENT_HEIGHT);
		SelectObject(memdc, fundo);
		/*SelectObject(memdc, GetStockObject(BLACK_BRUSH));
		PatBlt(memdc, 0, 0, 800, 600, PATCOPY);*/
		ReleaseDC(hWnd, hdc);

		iniciaDoubleBuffer(hWnd);
		loadUIResources(hWnd);

	}break;

	case WM_PAINT:{

		// Constrói imagem no buffer
		buildImageOnBuffer(hWnd);

		// Copia buffer para o ecrã
		BitBlt(hWindowDC, 0, 0, 1000, 700, hBufferDC, 0, 0, SRCCOPY);

		//hdc = BeginPaint(hWnd, &ps);

		// Isto não é aqui,  criar função para resolver em todos os casos. Ver tp miguel
		//BitBlt(hWindowDC, 0, 0, 1000, 700, hDeviceContexts[posBgLv1], 0, 0, SRCCOPY);
		EndPaint(hWnd, &paintstruct);
	}break;
	
	case WM_COMMAND: {
		/*switch (LOWORD(wParam))
		case ID_SERVIDOR_LOGIN:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1),
				hWnd, (DLGPROC)FuncaoCaixa1);
			break;
		case ID_JOGO_TOP10:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2),
				NULL, (DLGPROC)FuncaoCaixa2);
			break;
		case ID_SOBRE:
			MessageBox(hWnd, TEXT("Sobre"), TEXT("Opção do Menu"), MB_OK);
			break;*/
	}break;

	case WM_LBUTTONDOWN:{
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		hdc = GetDC(hWnd);
		TextOut(hdc, x, y, &c, 1);
		ReleaseDC(hWnd, hdc);
	}break;

	case  WM_MBUTTONUP:{

		InvalidateRect(hWnd, NULL, 1);
	}break;

	case WM_RBUTTONDOWN: {
		HDC auxdc;
		hdc = GetDC(hWnd);
		auxdc = CreateCompatibleDC(hdc);
		//Ativar um elemento dos tipos: Fonte, Fundo, Linha, Bitmap no device context em questão
		SelectObject(auxdc, hbmp);
		BitBlt(hdc, LOWORD(lParam), HIWORD(lParam), 66/*largura*/, 66, auxdc, 0, 0, SRCCOPY);
		//Tudo o que se escreve no fundo da janela principal se escreve no DC da memória
		BitBlt(memdc, LOWORD(lParam), HIWORD(lParam), 66/*largura*/, 66, auxdc, 0, 0, SRCCOPY);
		DeleteDC(auxdc);
		ReleaseDC(hWnd, hdc);
	}break;

	case WM_CHAR: {
		//Premiu uma tecla com representação ASCII
		c = wParam;
		hdc = GetDC(hWnd);
		TextOut(hdc, x, y, &c, 1);
		ReleaseDC(hWnd, hdc);
	}break;

	case WM_KEYDOWN: {
		if (estado == ESTADO_INICIAL) {
			if (wParam == VK_RETURN) {
				//FAZER: efetuar as connecções necessárias   -----   ver tp miguel
				estado = ESTADO_EM_JOGO;
				InvalidateRect(hWnd, NULL, TRUE);
					
				//resume de uma thread
			}
		}
		else if (estado == ESTADO_EM_JOGO) {
			Mensagem msg;

			if (wParam == VK_LEFT) { 
				//	fazer isto com inteiros para ser mais simples
				wcscpy_s(msg.msg, TEXT("move esquerda"));
				wcscpy_s(msg.nomeJogador, TEXT("teste"));
				escreveMsg(&cDados, &msg);
					
				//PlaySound(TEXT("nave.wav"), NULL, SND_ASYNC);
			}

			else if (wParam == VK_RIGHT){
					
				wcscpy_s(msg.msg, TEXT("move direita"));
				wcscpy_s(msg.nomeJogador, TEXT("teste"));
				escreveMsg(&cDados, &msg);
			}
			
		}
	}break;

	case WM_DESTROY: {// Destruir a janela e terminar o programa// "PostQuitMessage(Exit Status)"
		PostQuitMessage(0);
		DeleteDC(memdc); //Só no fim
	}break;

	//Descomentar para entregar

	//case WM_CLOSE: {
	//	if (MessageBox(hWnd, TEXT("	Quer mesmo sair?"), TEXT("Fechar Cliente KRAKOUT"), MB_YESNO | MB_ICONASTERISK) == IDYES) {
	//		// envia mensagem
	//		continuar = false;			//controlo de threads (falta implementar)
	//		Sleep(5);
	//		PostQuitMessage(0);
	//	}
	//}break;

	default: {
		return DefWindowProc(hWnd, messg, wParam, lParam);
	}break;
	}
	return(0);
}



/* Inicializa vari�veis do cliente */
void iniciaVariaveis() {

	estado = ESTADO_EM_JOGO;
	continuar = true;

	//Teclas
	KEY_RIGHT = VK_RIGHT;
	KEY_LEFT = VK_LEFT;

	// UI
	hdc = NULL;
	bufferDC = NULL;

	//Jogador
}

/* Prepara double buffer */
void iniciaDoubleBuffer(HWND hWnd) {
	hWindowDC = GetDC(hWnd);
	hBufferDC = CreateCompatibleDC(hWindowDC);
	hBufferBitmap = CreateCompatibleBitmap(hWindowDC, CLIENT_WIDTH, CLIENT_HEIGHT);
	SelectObject(hBufferDC, hBufferBitmap);
	ReleaseDC(hWnd, hWindowDC);
}


void iniciaThreads() {
	
	hT_GetDadosJogo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)t_GetDadosJogo, NULL, 0, NULL);

	if (hT_GetDadosJogo == NULL) {

		if (MessageBox(hWndBkp, TEXT("Erro a criar threads!"), TEXT("ERRO"), MB_OK | MB_ICONERROR) == IDYES) {
			PostQuitMessage(1);
		}
	}
}


DWORD WINAPI t_GetDadosJogo(LPVOID param) {
	do {

		leJogo(&cDados, &jogo);
		estado = ESTADO_EM_JOGO;

		// Atualiza mapa
		InvalidateRect(hWndBkp, NULL, FALSE);

	} while (continuar);

	return 0;
}

/* PREPARA RECURSOS DA UI DO CLIENTE_GUI */
void loadUIResources(HWND hWnd) {
	int i;

	// Carrega bitmaps
	hBitmaps[posBarreira] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BARREIRA), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBitmaps[posBola] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BOLA), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBitmaps[posTijAmarelo] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_TAMARELO), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBitmaps[posTijAzul] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_TAZUL), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBitmaps[posTijCinza] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_TCINZA), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBitmaps[posTijVerde] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_TVERDE), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBitmaps[posTijVermelho] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_TVERMELHO), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBitmaps[posBgLv1] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BG_LV1), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	hBitmaps[posBgLv2] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_BG_LV2), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);


	// Cria DCs dos bitmaps compat�veis com o buffer/ecr�
	// Coloca cada bitmap no respetivo DC compat�vel com o buffer/ecr�
	for (i = 0; i < NUM_BITMAPS; i++) {
		hDeviceContexts[i] = CreateCompatibleDC(hBufferDC);
		SelectObject(hDeviceContexts[i], hBitmaps[i]);
	}

}

bool iniciaMemJogo(DadosCtrl* cDados) {

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

bool iniciaMemMsg(DadosCtrl* cDados) {

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

void buildImageOnBuffer(HWND hWnd) {

	//int i;

	// Desenha no buffer, dependendo do estado
	switch (estado) {

	case ESTADO_INICIAL:
	{
		hWindowDC = BeginPaint(hWnd, &paintstruct);
		BitBlt(hBufferDC, 0, 0, 1000, 700, hDeviceContexts[posBgLv2], 0, 0, SRCCOPY);
	}
	break;

	case ESTADO_ESPERA_JOGO:
	{
		hWindowDC = BeginPaint(hWnd, &paintstruct);
		BitBlt(hdc, 0, 0, 1000, 700, hDeviceContexts[posBgLv1], 0, 0, SRCCOPY);
	}
	break;

	case ESTADO_EM_JOGO:
	{
		//BitBlt(hdc, 0, 0, 1000, 700, hDeviceContexts[posBgLv1], 0, 0, SRCCOPY);
		// Constrói mapa do jogo
		buildMapOnBuffer(hWnd);
		//buildInfoBarOnBuffer(hWnd);
	}
	break;

	case ESTADO_JOGO_TERMINADO_VITORIA:
	{
		hWindowDC = BeginPaint(hWnd, &paintstruct);
		//BitBlt(hBufferDC, 0, 0, 1000, 700, hDeviceContexts[posGameWon], 0, 0, SRCCOPY);
		// TODO: Imprime tabela de classificações (função para ser também chamada em ESTADO_JOGO_TERMIANDO_PERDIDO)
	}
	break;

	case ESTADO_JOGO_TERMIANDO_PERDIDO:
	{
		hWindowDC = BeginPaint(hWnd, &paintstruct);
		//BitBlt(hBufferDC, 0, 0, 1000, 700, hDeviceContexts[posGameLost], 0, 0, SRCCOPY);
		// TODO: Imprime tabela de classificações (função para ser também chamada em ESTADO_JOGO_TERMINADO_VITORIA)
	}
	break;
	}
}

/* Constrói mapa no buffer */
void buildMapOnBuffer(HWND hWnd) {

	int i;

	//Background
	hWindowDC = BeginPaint(hWnd, &paintstruct);
	BitBlt(hBufferDC, 0, 0, 1000, 700, hDeviceContexts[posBgLv2], 0, 0, SRCCOPY);

	//Tijolos
	for (i = 0; i < NUM_MAX_TIJOLOS; i++) {
		if (jogo.tijolos[i].colisoes == 1) {
			TransparentBlt(hBufferDC, jogo.tijolos[i].y, jogo.tijolos[i].x, TIJOLO_NORMAL_WIDTH, TIJOLO_NORMAL_HEIGHT, hDeviceContexts[posTijAmarelo], 0, 0, TIJOLO_NORMAL_WIDTH, TIJOLO_NORMAL_HEIGHT, RGB(255, 255, 255));
		}
	}

	//Bola
	for (i = 0; i < NUM_MAX_BOLAS; i++) {		
		TransparentBlt(hBufferDC, jogo.bolas[i].y, jogo.bolas[i].x, 25, 25, hDeviceContexts[posBola], 0, 0, BOLA_WIDTH_HEIGHT, BOLA_WIDTH_HEIGHT, RGB(255, 255, 255));
	}

	//Barreira
	TransparentBlt(hBufferDC, jogo.barreiras[0].y, jogo.barreiras[0].x, BARREIRA_BIG_WIDTH, BARREIRA_BIG_HEIGHT, hDeviceContexts[posBarreira], 0, 0, BARREIRA_WIDTH, BARREIRA_HEIGHT, RGB(255, 255, 255));

}
