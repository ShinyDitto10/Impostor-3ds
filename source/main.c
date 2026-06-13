/*IMPOSTOR v0.0.0
Creado por ShinyDitto10*/

#include <citro2d.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gameplay.h"
#include "menu_gameplay.h"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

enum MenuActivo {
	MENU_PRINCIPAL,
	MENU_OPCIONES,
	MENU_TUTORIAL,
	MENU_CREDITOS,
	MENU_GAMEPLAY,
	MENU_PARTIDA,
	MENU_RESULTADOS
};

int menuActivo = MENU_PRINCIPAL;
int numJugadores = 3;
int numImpostores = 1;

C2D_TextBuf g_staticBuf;
C2D_TextBuf g_dynamicBuf;
C2D_Text g_dynamicText[13];
C2D_Text g_staticText[18];

bool tocandoRectangulo(touchPosition touch, float x, float y, float w, float h){
	if(touch.px < x + w && touch.px > x && touch.py < y + h && touch.py > y) return true;
	else return false;
}


//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	// Preparar librerías
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	//Preparar teclado
	

	romfsInit();
    srand(time(NULL));

	abrirPalabrasTXT();
	int numPalabrasArchivo = getNumPalabrasArchivo();
	palabrasPartida palabrasRonda = escogerPalabrasPartida(numPalabrasArchivo);
	char palabraJugadores[50];
	char pistaImpostor[50];
	sprintf(palabraJugadores, "La palabra es %s", palabrasRonda.palabraJugadores);
	sprintf(pistaImpostor, "La pista es %s", palabrasRonda.pistaImpostor);

	// Preparar pantallas
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Create colors
	u32 clrBlue  = C2D_Color32(0x30, 0x30, 0xFF, 0xFF);
	//u32 clrBlack  = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
	u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
	u32 clrBg = C2D_Color32(0x00, 0x00, 0x80, 0xFF);
	u32 clrLight = C2D_Color32(0x80, 0x80, 0xFF, 0xFF);
    u32 clrTrans = C2D_Color32(0x00, 0x00, 0x00, 0x00);
	

	// Preparar texto estático
	g_staticBuf = C2D_TextBufNew(4096);
	g_dynamicBuf = C2D_TextBufNew(4096);
	C2D_TextParse(&g_staticText[0], g_staticBuf, "IMPOSTOR");
	C2D_TextParse(&g_staticText[1], g_staticBuf, "ver 0.0.0");
	C2D_TextParse(&g_staticText[2], g_staticBuf, "Creado por ShinyDitto10");
	C2D_TextParse(&g_staticText[3], g_staticBuf, "Pulsa START para salir");
	C2D_TextParse(&g_staticText[4], g_staticBuf, "Jugar");
	C2D_TextParse(&g_staticText[5], g_staticBuf, "Opciones");
	C2D_TextParse(&g_staticText[6], g_staticBuf, "Cómo\njugar");
	C2D_TextParse(&g_staticText[7], g_staticBuf, "Créditos");
	C2D_TextParse(&g_staticText[8], g_staticBuf, "Añadir\nJugador");
	C2D_TextParse(&g_staticText[9], g_staticBuf, "Quitar\nJugador");
	C2D_TextParse(&g_staticText[10], g_staticBuf, "Cruceta: Navegar  A: Cambiar nombre");
	C2D_TextParse(&g_staticText[11], g_staticBuf, "Número de impostores");
	C2D_TextParse(&g_staticText[12], g_staticBuf, "Pista para los impostores");
	C2D_TextParse(&g_staticText[13], g_staticBuf, "Los impostores se conocen");
	C2D_TextParse(&g_staticText[14], g_staticBuf, "Jugadores");
	C2D_TextParse(&g_staticText[15], g_staticBuf, "Empezar partida");
	C2D_TextParse(&g_staticText[16], g_staticBuf, "-");
	C2D_TextParse(&g_staticText[17], g_staticBuf, "+");
	for(int i = 0; i < 18; i++){
	    C2D_TextOptimize(&g_staticText[i]);
	}

	bool pulsandoPantalla = false;
	bool pulsandoPantallaUltimoFrame;

	char nombreJug[12][20] = {
		"Jugador 1",
		"Jugador 2",
		"Jugador 3",
		"Jugador 4",
		"Jugador 5",
		"Jugador 6",
		"Jugador 7",
		"Jugador 8",
		"Jugador 9",
		"Jugador 10",
		"Jugador 11",
		"Jugador 12",
	};
	SwkbdState swkbd;

	// Bucle de la aplicación
	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START && menuActivo == MENU_PRINCIPAL) break;
        
		if(pulsandoPantalla) pulsandoPantallaUltimoFrame = true;
		else pulsandoPantallaUltimoFrame = false;
		touchPosition touch;
		hidTouchRead(&touch);
		if(touch.px == 0 && touch.py == 0) pulsandoPantalla = false;
		else pulsandoPantalla = true;

		if (menuActivo == MENU_GAMEPLAY && kDown & KEY_A && posicionAid(botonSeleccionado) < numJugadores){
			char buffer[16];
			swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, 15);
			swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "Cancelar", false);
			swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "Confirmar", true);
	        swkbdSetHintText(&swkbd, "Escribe tu nombre");
	        SwkbdButton button = swkbdInputText(&swkbd, buffer, sizeof(buffer));
			if(button == SWKBD_BUTTON_RIGHT){
				strncpy(nombreJug[posicionAid(botonSeleccionado)], buffer, 16);
			}
		}

		C2D_TextBufClear(g_dynamicBuf);
		for(int i = 0; i < numJugadores; i++){
			C2D_TextParse(&g_dynamicText[i], g_dynamicBuf, nombreJug[i]);
			C2D_TextOptimize(&g_dynamicText[i]);
		}
		char numImpostoresMenu[3];
		sprintf(numImpostoresMenu, "%d", numImpostores);
		C2D_TextParse(&g_dynamicText[12], g_dynamicBuf, numImpostoresMenu);
		C2D_TextOptimize(&g_dynamicText[12]);
		
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(bottom, clrBg);
		C2D_TargetClear(top, clrBg);

		if (menuActivo == MENU_PRINCIPAL){

			C2D_SceneBegin(bottom);
			C2D_DrawRectSolid(22, 20, 0.0f, 276, 80, tocandoRectangulo(touch, 22, 20, 276, 80) ? clrLight : clrBlue);
		    C2D_DrawRectSolid(22, 115, 0.0f, 82, 80, tocandoRectangulo(touch, 22, 115, 82, 80) ? clrLight : clrBlue);
		    C2D_DrawRectSolid(119, 115, 0.0f, 82, 80, tocandoRectangulo(touch, 119, 115, 82, 80) ? clrLight : clrBlue);
		    C2D_DrawRectSolid(216, 115, 0.0f, 82, 80, tocandoRectangulo(touch, 216, 115, 82, 80) ? clrLight : clrBlue);
		    C2D_DrawText(&g_staticText[3], C2D_WithColor | C2D_AlignCenter, 160, 220, 1, 0.5f, 0.5f, clrWhite);
		    C2D_DrawText(&g_staticText[4], C2D_WithColor | C2D_AlignCenter, 160, 45, 0.0f, 1.0f, 1.0f, clrWhite);
		    C2D_DrawText(&g_staticText[5], C2D_WithColor | C2D_AlignCenter, 63, 135, 0.0f, 0.75f, 0.75f, clrWhite);
		    C2D_DrawText(&g_staticText[6], C2D_WithColor | C2D_AlignCenter, 160, 125, 0.0f, 0.75f, 0.75f, clrWhite);
		    C2D_DrawText(&g_staticText[7], C2D_WithColor | C2D_AlignCenter, 257, 135, 0.0f, 0.75f, 0.75f, clrWhite);

			C2D_SceneBegin(top);
			C2D_DrawText(&g_staticText[0], C2D_WithColor | C2D_AlignCenter, 200, 50, 1, 1.5f, 1.5f, clrWhite);
		    C2D_DrawText(&g_staticText[1], C2D_WithColor, 10, 220, 1, 0.5f, 0.5f, clrWhite);
		    C2D_DrawText(&g_staticText[2], C2D_WithColor | C2D_AlignRight, 390, 220, 1, 0.5f, 0.5f, clrWhite);

			if (tocandoRectangulo(touch, 22, 20, 276, 80)) menuActivo = MENU_GAMEPLAY;
		}

		else if(menuActivo == MENU_GAMEPLAY){

			C2D_SceneBegin(bottom);
			C2D_DrawRectSolid(15, 20, 0.0f, 139, 65, clrBlue);
			C2D_DrawRectSolid(166, 20, 0.0f, 139, 65, clrBlue);
			C2D_DrawRectSolid(15, 95, 0.0f, 290, 25, clrBlue);
			C2D_DrawRectSolid(15, 130, 0.0f, 290, 25, clrBlue);
			C2D_DrawRectSolid(15, 165, 0.0f, 290, 25, clrBlue);
			C2D_DrawRectSolid(100, 200, 0.0f, 120, 35, clrBlue);
			C2D_DrawText(&g_staticText[8], C2D_WithColor | C2D_AlignCenter, 84.5f, 30, 0.0f, 0.75f, 0.75f, clrWhite);
			C2D_DrawText(&g_staticText[9], C2D_WithColor | C2D_AlignCenter, 235.5f, 30, 0.0f, 0.75f, 0.75f, clrWhite);
			C2D_DrawText(&g_staticText[11], C2D_WithColor, 20, 100, 0.0f, 0.6f, 0.6f, clrWhite);
			C2D_DrawText(&g_dynamicText[12], C2D_WithColor | C2D_AlignCenter, 265, 100, 0.0f, 0.6f, 0.6f, clrWhite);
			C2D_DrawText(&g_staticText[16], C2D_WithColor | C2D_AlignCenter, 240, 100, 0.0f, 0.6f, 0.6f, clrWhite);
			C2D_DrawText(&g_staticText[17], C2D_WithColor | C2D_AlignCenter, 290, 100, 0.0f, 0.6f, 0.6f, clrWhite);
			C2D_DrawText(&g_staticText[12], C2D_WithColor, 20, 135, 0.0f, 0.6f, 0.6f, clrWhite);
			C2D_DrawText(&g_staticText[13], C2D_WithColor, 20, 170, 0.0f, 0.6f, 0.6f, clrWhite);
			C2D_DrawText(&g_staticText[15], C2D_WithColor | C2D_AlignCenter, 160, 205, 0.0f, 0.6f, 0.6f, clrWhite);

			C2D_SceneBegin(top);
			crearMenuGameplayArriba(numJugadores);
			C2D_DrawText(&g_staticText[14], C2D_WithColor | C2D_AlignCenter, 200, 6, 0.0f, 1, 1, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
			C2D_DrawText(&g_staticText[10], C2D_WithColor | C2D_AlignCenter, 200, 220, 0.0f, 0.5f, 0.5f, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));

	        C2D_DrawText(&g_dynamicText[0], C2D_WithColor | C2D_AlignCenter, 73, 50, 0.0f, 0.65f, 0.65f, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
			C2D_DrawText(&g_dynamicText[1], C2D_WithColor | C2D_AlignCenter, 198, 50, 0.0f, 0.65f, 0.65f, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
			C2D_DrawText(&g_dynamicText[2], C2D_WithColor | C2D_AlignCenter, 323, 50, 0.0f, 0.65f, 0.65f, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
			C2D_DrawText(&g_dynamicText[3], C2D_WithColor | C2D_AlignCenter, 73, 94, 0.0f, 0.65f, 0.65f, (numJugadores < 4) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[4], C2D_WithColor | C2D_AlignCenter, 198, 94, 0.0f, 0.65f, 0.65f, (numJugadores < 5) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[5], C2D_WithColor | C2D_AlignCenter, 323, 94, 0.0f, 0.65f, 0.65f, (numJugadores < 6) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[6], C2D_WithColor | C2D_AlignCenter, 73, 138, 0.0f, 0.65f, 0.65f, (numJugadores < 7) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[7], C2D_WithColor | C2D_AlignCenter, 198, 138, 0.0f, 0.65f, 0.65f, (numJugadores < 8) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[8], C2D_WithColor | C2D_AlignCenter, 323, 138, 0.0f, 0.65f, 0.65f, (numJugadores < 9) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[9], C2D_WithColor | C2D_AlignCenter, 73, 182, 0.0f, 0.65f, 0.65f, (numJugadores < 10) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[10], C2D_WithColor | C2D_AlignCenter, 198, 182, 0.0f, 0.65f, 0.65f, (numJugadores < 11) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[11], C2D_WithColor | C2D_AlignCenter, 323, 182, 0.0f, 0.65f, 0.65f, (numJugadores < 12) ? clrTrans : clrWhite);

			if(kDown & KEY_DDOWN && botonSeleccionado.y < 3) botonSeleccionado.y++;
			if(kDown & KEY_DUP && botonSeleccionado.y > 0) botonSeleccionado.y--;
			if(kDown & KEY_DRIGHT && botonSeleccionado.x < 2) botonSeleccionado.x++;
			if(kDown & KEY_DLEFT && botonSeleccionado.x > 0) botonSeleccionado.x--;

			if (tocandoRectangulo(touch, 15, 20, 139, 65) && numJugadores < 12 && !pulsandoPantallaUltimoFrame) numJugadores++;
		    if (tocandoRectangulo(touch, 166, 20, 139, 65) && numJugadores > 3 && !pulsandoPantallaUltimoFrame) numJugadores--;
			if (tocandoRectangulo(touch, 100, 200, 120, 35)) menuActivo = MENU_PARTIDA;
	        else if (kDown & KEY_B){
	            menuActivo = MENU_PRINCIPAL;
			}
		}

		else if (menuActivo == MENU_PARTIDA){
			C2D_SceneBegin(bottom);
			C2D_SceneBegin(top);
			if(kDown & KEY_B) menuActivo = MENU_PRINCIPAL;
		}
		
	    C3D_FrameEnd(0);
	}

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
