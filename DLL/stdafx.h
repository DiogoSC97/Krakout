// stdafx.h: arquivo de inclusão para arquivos de inclusão padrão do sistema,
// ou arquivos de inclusão específicos a um projeto que são usados frequentemente, mas
// são modificados raramente
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Excluir itens raramente utilizados dos cabeçalhos do Windows
// Arquivos de Cabeçalho do Windows
#include <windows.h>

#pragma region DEFINES

/* DIMENSÕES DA JANELA DO CLIENTE */
#define CLIENT_WIDTH		1000	// Largura da janela do cliente em píxeis
#define CLIENT_HEIGHT		700		// Altura da janela do cliente em píxeis
#define initWindowPosTop	50		// Posição da janela (deslocamento em relação à direita do ecrã)
#define initWindowPosLeft	250		// Posição da janela (deslocamento em relação à esquerda do ecrã)



/* BITMAPS (POSIÇÔES PARA ARRAYS DE HANDLES E HDC) */
#define posBola				0		// Posição do bitmap da bola
#define posBarreira			1		// Posição do bitmap da barreira
#define posTijAmarelo		2		// Posição do bitmap do tijolo amarelo
#define posTijAzul			3		// Posição do bitmap do tijolo azul
#define posTijCinza			4		// Posição do bitmap do tijolo cinza
#define posTijVerde			5		// Posição do bitmap do tijolo verde
#define posTijVermelho		6		// Posição do bitmap do tijolo vermelho
#define posBgLv1			7		// Posição do bitmap de fundo do lv1
#define posBgLv2			8		// Posição do bitmap de fundo do lv2	

#define NUM_BITMAPS			9


/* DIMENSOES DOS ITENS */
#define TIJOLO_NORMAL_WIDTH			35
#define TIJOLO_NORMAL_HEIGHT		20
#define BARREIRA_WIDTH				48
#define BARREIRA_HEIGHT				9
#define BARREIRA_BIG_WIDTH			120
#define BARREIRA_BIG_HEIGHT			20
#define BOLA_WIDTH_HEIGHT			30


/* ESTADOS */
#define ESTADO_INICIAL					10000	// Estado (início)
#define ESTADO_ESPERA_JOGO				10001	// Estado (à espera do início do jogo)
#define ESTADO_EM_JOGO					10002	// Estado (jogo a decorrer)
#define ESTADO_JOGO_TERMINADO_VITORIA	10003	// Estado (jogo terminado com vitória)
#define ESTADO_JOGO_TERMIANDO_PERDIDO	10004	// Estado (jogo terminado com derrota)


/* NÚMERO MAXIMO DE INSTÂNCIAS */
#define NUM_MAX_TIJOLOS			24
#define NUM_MAX_BOLAS			1

#pragma endregion

// referenciar os cabeçalhos adicionais de que seu programa precisa aqui
