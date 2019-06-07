// Cliente.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h"
#include "../DLL/DLL.h"
#include "resource.h"
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

bool iniciaMemJogo(DadosCtrl * cDados);
//void gotoxy(int x, int y);

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
	WNDCLASSEX wcApp; // WNDCLASSEX é uma estrutura cujos membros servem para
	// definir as características da classe da janela
	// ============================================================================
	// 1. Definição das características da janela "wcApp"
	// (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst; // Instância da janela actualmente exibida
	// ("hInst" é parâmetro de WinMain e vem inicializada daí)
	wcApp.lpszClassName = szProgName; // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos; // Endereço da função de processamento da janela // ("TrataEventos" foi declarada no início e // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for // modificada horizontal ou verticalmente
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));// "hIcon" = handler do ícon normal
	//"NULL" = Icon definido no Windows
	// "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));// "hIconSm" = handler do ícon pequeno
	//"NULL" = Icon definido no Windows
	// "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR1)); // "hCursor" = handler do cursor (rato)
	// "NULL" = Forma definida no Windows
	// "IDC_ARROW" Aspecto "seta"
	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); // Classe do menu que a janela pode ter
	// (NULL = não tem menu)
	wcApp.cbClsExtra = 0; // Livre, para uso particular
	wcApp.cbWndExtra = 0; // Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por // "GetStockObject".Neste caso o fundo será cinza
	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);
	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName, // Nome da janela (programa) definido acima
		TEXT("KRAKOUT"),// Texto que figura na barra do título
		WS_OVERLAPPEDWINDOW, // Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT, // Posição x pixels (default=à direita da última)
		CW_USEDEFAULT, // Posição y pixels (default=abaixo da última)
		800, // Largura da janela (em pixels)
		600, // Altura da janela (em pixels)
		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de
		// outra) ou HWND_DESKTOP se a janela for a primeira,
// criada a partir do "desktop"
(HMENU)NULL, // handle do menu da janela (se tiver menu)
(HINSTANCE)hInst, // handle da instância do programa actual ("hInst" é
// passado num dos parâmetros de WinMain()
0); // Não há parâmetros adicionais para a janela
// ============================================================================
// 4. Mostrar a janela
// ============================================================================
	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por
	// "CreateWindow"; "nCmdShow"= modo de exibição (p.e.
	// normal/modal); é passado como parâmetro de WinMain()
	UpdateWindow(hWnd); // Refrescar a janela (Windows envia à janela uma
	// mensagem para pintar, mostrar dados, (refrescar)…
	// ============================================================================
	// 5. Loop de Mensagens
	// ============================================================================
	// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa fila de
	// espera até que GetMessage(...) possa ler "a mensagem seguinte"
	// Parâmetros de "getMessage":
	// 1)"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no
	// início de WinMain()):
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
		TranslateMessage(&lpMsg); // Pré-processamento da mensagem (p.e. obter código
		// ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que
		// aguarda até que a possa reenviar à função de
		// tratamento da janela, CALLBACK TrataEventos (abaixo)
	}
	// ============================================================================
	// 6. Fim do programa
	//==================================================================
	return((int)lpMsg.wParam); // Retorna sempre o parâmetro wParam da estrutura lpMsg
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC hdc; static TCHAR c;
	static int x = 10, y = 10, xi = 0, yi = 0, xf = 0, yf = 0;
	PAINTSTRUCT ps;
	static HBITMAP hbmp;
	static HDC memdc; //Device context que guarda tudo o que
	//se imprime no fundo da janela
	HBITMAP fundo;
	switch (messg) {
		// outra) ou HWND_DESKTOP se a janela for a primeira,
	case WM_CREATE:
		hbmp = LoadBitmap(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDB_BITMAP4));
		hdc = GetDC(hWnd);
		memdc = CreateCompatibleDC(hdc);
		fundo = CreateCompatibleBitmap(hdc, 800, 600);
		SelectObject(memdc, fundo);
		SelectObject(memdc, GetStockObject(GRAY_BRUSH));
		PatBlt(memdc, 0, 0, 800, 600, PATCOPY);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_DESTROY: // Destruir a janela e terminar o programa
		// "PostQuitMessage(Exit Status)"
		PostQuitMessage(0);
		DeleteDC(memdc); //Só no fim
		break;
	case WM_COMMAND:
		/*switch (LOWORD(wParam)) {
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
			break;
		}
		break;*/
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//TextOut(hdc, x, y, &c, 1);
		//Rectangle(hdc, xi, yi, xf, yf);
		BitBlt(hdc, 0, 0, 800, 600, memdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		hdc = GetDC(hWnd);
		TextOut(hdc, x, y, &c, 1);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_MBUTTONDOWN:
		xi = LOWORD(lParam);
		yi = HIWORD(lParam);
		break;
	case  WM_MBUTTONUP:
		xf = LOWORD(lParam);
		yf = HIWORD(lParam);
		/*hdc = GetDC(hWnd);
		Rectangle(hdc, xi, yi, xf, yf);
		ReleaseDC(hWnd, hdc);*/
		InvalidateRect(hWnd, NULL, 1);
		break;
	case WM_RBUTTONDOWN:
	{
		HDC auxdc;
		hdc = GetDC(hWnd);
		auxdc = CreateCompatibleDC(hdc);
		//Ellipse(hdc, LOWORD(lParam), HIWORD(lParam), 
			//LOWORD(lParam)+100, HIWORD(lParam)+100);
		//Ativar um elemento dos tipos: Fonte, Fundo, Linha, Bitmap
		//no device context em questão
		SelectObject(auxdc, hbmp);
		BitBlt(hdc, LOWORD(lParam), HIWORD(lParam), 66/*largura*/, 66, auxdc, 0, 0, SRCCOPY);
		//Tudo o que se escreve no fundo da janela principal se
		//escreve no DC da memória
		BitBlt(memdc, LOWORD(lParam), HIWORD(lParam), 66/*largura*/,66, auxdc, 0, 0, SRCCOPY);
		DeleteDC(auxdc);
		ReleaseDC(hWnd, hdc);
	}
	break;
	case WM_CHAR:
		//Premiu uma tecla com representação ASCII
		c = wParam;
		hdc = GetDC(hWnd);
		TextOut(hdc, x, y, &c, 1);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_RIGHT) {
			x += 10;
			/*hdc = GetDC(hWnd);
			TextOut(hdc, x, y, &c, 1);
			ReleaseDC(hWnd, hdc);*/
			InvalidateRect(hWnd, NULL, 1);
		}
		break;
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // não é efectuado nenhum processamento, apenas se segue o "default" do Windows
		return DefWindowProc(hWnd, messg, wParam, lParam);
		break;
	}
	return(0);
}

bool iniciaMemJogo(DadosCtrl * cDados);
bool iniciaMemMsg(DadosCtrl * cDados);
void gotoxy(int x, int y);
/*
int _tmain(int argc, TCHAR *argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	DadosCtrl cDados;
	Jogo jogo;

	iniciaMemJogo(&cDados);

	
	while (true)
	{
		leJogo(&cDados, &jogo);
		system("cls");
		gotoxy(jogo.bolas[0].x, jogo.bolas[0].y);
		_tprintf(TEXT("O"));
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
*/