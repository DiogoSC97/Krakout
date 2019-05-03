// Krakout.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h"
#include <iostream>
/*
int main()
{
    std::cout << "Hello World!\n"; 
}
*/
// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln



#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define TAM 200

int _tmain(int argc, TCHAR *argv[]) {
	HKEY chave;
	DWORD resultado, versao, tamanho;
	TCHAR str[TAM], autor[TAM];
	BYTE buf[200];
	int aux_len = 0;

	TCHAR top10[10][40] = {
		TEXT("Paulão,100"),
		TEXT("Bob,150"),
		TEXT("King,200"),
		TEXT("Talocha,1000"),
		TEXT("Paulão,100"),
		TEXT("Paulão,100"),
		TEXT("Paulão,100"),
		TEXT("Paulão,100"),
		TEXT("Paulão,100"),
		TEXT("Paulão,100") };

	memset(buf, 0, 200);

	for (int i = 0; i < 10; i++)
	{
		if (i > 0)
			aux_len = _tcslen(top10[i - 1]);
		memcpy(buf + aux_len + 1, top10[i], _tcslen(top10[i]) + 1);

	}


#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\Krakout
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Krakout"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &resultado) != ERROR_SUCCESS) {
		_tprintf(TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError());
		return -1;
	}
	else {
		//Se a chave foi criada, inicializar os valores
		if (resultado == REG_CREATED_NEW_KEY) {
			_tprintf(TEXT("Chave: HKEY_CURRENT_USER\\Software\\Krakout criada\n"));
			//Criar valor "Autor" = "Meu nome"

			RegSetValueEx(chave, TEXT("Top10"), 0, REG_MULTI_SZ, (LPBYTE)buf, 10 * 20 * sizeof(TCHAR));
			_tprintf(TEXT("Valores Autor e Versão guardados\n"));
		}
		//Se a chave foi aberta, ler os valores lá guardados
		else if (resultado == REG_OPENED_EXISTING_KEY) {
			_tprintf(TEXT("Chave: HKEY_CURRENT_USER\\Software\\Krakout aberta\n"));
			tamanho = 200;
			RegQueryValueEx(chave, TEXT("Top10"), NULL, NULL, (LPBYTE)autor, &tamanho);
			autor[tamanho / sizeof(TCHAR)] = '\0';
			_stprintf_s(str, TAM, TEXT("Top10:%s\n"), autor);
			_tprintf(TEXT("Lido do Registry:%s\n"), str);
		}
		RegCloseKey(chave);
	}
	return 0;
}
