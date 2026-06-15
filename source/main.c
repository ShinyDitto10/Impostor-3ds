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
int jugadorComienza = 0;
u64 tiempoTranscurrido;
u64 tickInicial = 0;

typedef struct{
	int minutos;
	int segundos;
} Tiempo;

C2D_TextBuf g_staticBuf;
C2D_TextBuf g_dynamicBuf;
C2D_Text g_dynamicText[17];
C2D_Text g_staticText[23];

Tiempo contadorTiempo;

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

	
	romfsInit();
    srand(time(NULL));

	abrirPalabrasTXT();
	int numPalabrasArchivo = getNumPalabrasArchivo();
	palabrasPartida palabrasRonda = escogerPalabrasPartida(numPalabrasArchivo);

	// Preparar pantallas
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Crear colores
	u32 clrBlue  = C2D_Color32(0x30, 0x30, 0xFF, 0xFF);
	//u32 clrBlack  = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
	u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
	u32 clrBg = C2D_Color32(0x00, 0x00, 0x80, 0xFF);
	u32 clrLight = C2D_Color32(0x80, 0x80, 0xFF, 0xFF);
    u32 clrTrans = C2D_Color32(0x00, 0x00, 0x00, 0x00);
	u32 clrTarjeta = C2D_Color32(0xFF, 0x4B, 0x33, 0xFF);
	
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
	C2D_TextParse(&g_staticText[18], g_staticBuf, "Escoge tu nombre de la lista:");
	C2D_TextParse(&g_staticText[19], g_staticBuf, "Pulsa para ver\ntu palabra");
	C2D_TextParse(&g_staticText[20], g_staticBuf, "Siguiente jugador");
	C2D_TextParse(&g_staticText[21], g_staticBuf, "Terminar partida");
	C2D_TextParse(&g_staticText[22], g_staticBuf, "Volver al menú");
	for(int i = 0; i < 23; i++){
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
        
		//Comprobar pantalla táctil
		if(pulsandoPantalla) pulsandoPantallaUltimoFrame = true;
		else pulsandoPantallaUltimoFrame = false;
		touchPosition touch;
		hidTouchRead(&touch);
		if(touch.px == 0 && touch.py == 0) pulsandoPantalla = false;
		else pulsandoPantalla = true;

		//Teclado
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
		C2D_TextParse(&g_dynamicText[13], g_dynamicBuf, textoTarjeta);
		C2D_TextOptimize(&g_dynamicText[13]);
		char jugadorComienzaOutput[31];
		sprintf(jugadorComienzaOutput, "Empieza %s", nombreJug[jugadorComienza]);
		C2D_TextParse(&g_dynamicText[14], g_dynamicBuf, jugadorComienzaOutput);
		C2D_TextOptimize(&g_dynamicText[14]);

		char tiempoTranscurridoOutput[31];
		tiempoTranscurrido = (svcGetSystemTick() - tickInicial) / SYSCLOCK_ARM11;
        contadorTiempo.minutos = tiempoTranscurrido / 60;
		contadorTiempo.segundos = tiempoTranscurrido % 60;
		sprintf(tiempoTranscurridoOutput, "Tiempo: %d:%02d", contadorTiempo.minutos, contadorTiempo.segundos);
		C2D_TextParse(&g_dynamicText[15], g_dynamicBuf, tiempoTranscurridoOutput);
		C2D_TextOptimize(&g_dynamicText[15]);

		char palabraRevelada[60];
		sprintf(palabraRevelada, "La palabra era\n%s", palabrasRonda.palabraJugadores);
		palabraRevelada[strlen(palabraRevelada) - 2] = '\0';
		C2D_TextParse(&g_dynamicText[16], g_dynamicBuf, palabraRevelada);
		C2D_TextOptimize(&g_dynamicText[16]);
		
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
		    if (tocandoRectangulo(touch, 166, 20, 139, 65) && numJugadores > 3 && !pulsandoPantallaUltimoFrame) {
				numJugadores--;
				if(numImpostores + 1 == numJugadores) numImpostores--;
			}
			if (tocandoRectangulo(touch, 235, 95, 15, 25) && numImpostores > 1 && !pulsandoPantallaUltimoFrame) numImpostores--;
			if (tocandoRectangulo(touch, 285, 95, 15, 25) && numImpostores < numJugadores - 2 && numImpostores < 6 && !pulsandoPantallaUltimoFrame) numImpostores++;

			if (tocandoRectangulo(touch, 100, 200, 120, 35)){
				menuActivo = MENU_PARTIDA;
				abrirPalabrasTXT();
	            numPalabrasArchivo = getNumPalabrasArchivo();
	            palabrasRonda = escogerPalabrasPartida(numPalabrasArchivo);
				crearImpostores(numJugadores, numImpostores);
			}
	        else if (kDown & KEY_B){
	            menuActivo = MENU_PRINCIPAL;
			}
		}

		else if (menuActivo == MENU_PARTIDA){
			C2D_SceneBegin(bottom);
			C2D_DrawRectSolid(40, 40, 0.0f, 240, 150, (viendoPalabra == -1) ? clrBg : clrBlue);
			C2D_DrawText(&g_dynamicText[13], C2D_WithColor | C2D_AlignCenter, 160, 80, 0.0f, 1, 1, clrTarjeta);
			C2D_DrawRectSolid(70, 200, 0.0f, 180, 35, (viendoPalabra == -1) ? clrBg : clrBlue);
			C2D_DrawText(&g_staticText[20], C2D_WithColor | C2D_AlignCenter, 160, 210, 0.0f, 0.6f, 0.6f, (viendoPalabra == -1) ? clrTrans : clrWhite);
			if(viendoPalabra != -1 && !palabraVistaJugador[viendoPalabra]){
				C2D_DrawText(&g_staticText[19], C2D_WithColor | C2D_AlignCenter, 160, 80, 0.0f, 1, 1, clrWhite);
			}

			C2D_SceneBegin(top);
			crearMenuGameplayArriba(numJugadores);
			C2D_DrawText(&g_staticText[18], C2D_WithColor | C2D_AlignCenter, 200, 10, 0.0f, 0.75f, 0.75f, clrWhite);
			C2D_DrawText(&g_dynamicText[0], C2D_WithColor | C2D_AlignCenter, 73, 50, 0.0f, 0.65f, 0.65f, clrWhite);
			C2D_DrawText(&g_dynamicText[1], C2D_WithColor | C2D_AlignCenter, 198, 50, 0.0f, 0.65f, 0.65f, clrWhite);
			C2D_DrawText(&g_dynamicText[2], C2D_WithColor | C2D_AlignCenter, 323, 50, 0.0f, 0.65f, 0.65f, clrWhite);
			C2D_DrawText(&g_dynamicText[3], C2D_WithColor | C2D_AlignCenter, 73, 94, 0.0f, 0.65f, 0.65f, (numJugadores < 4) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[4], C2D_WithColor | C2D_AlignCenter, 198, 94, 0.0f, 0.65f, 0.65f, (numJugadores < 5) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[5], C2D_WithColor | C2D_AlignCenter, 323, 94, 0.0f, 0.65f, 0.65f, (numJugadores < 6) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[6], C2D_WithColor | C2D_AlignCenter, 73, 138, 0.0f, 0.65f, 0.65f, (numJugadores < 7) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[7], C2D_WithColor | C2D_AlignCenter, 198, 138, 0.0f, 0.65f, 0.65f, (numJugadores < 8) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[8], C2D_WithColor | C2D_AlignCenter, 323, 138, 0.0f, 0.65f, 0.65f, (numJugadores < 9) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[9], C2D_WithColor | C2D_AlignCenter, 73, 182, 0.0f, 0.65f, 0.65f, (numJugadores < 10) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[10], C2D_WithColor | C2D_AlignCenter, 198, 182, 0.0f, 0.65f, 0.65f, (numJugadores < 11) ? clrTrans : clrWhite);
			C2D_DrawText(&g_dynamicText[11], C2D_WithColor | C2D_AlignCenter, 323, 182, 0.0f, 0.65f, 0.65f, (numJugadores < 12) ? clrTrans : clrWhite);

			if(viendoPalabra == -1){
			    if(kDown & KEY_DDOWN && botonSeleccionado.y < 3) botonSeleccionado.y++;
			    if(kDown & KEY_DUP && botonSeleccionado.y > 0) botonSeleccionado.y--;
			    if(kDown & KEY_DRIGHT && botonSeleccionado.x < 2) botonSeleccionado.x++;
			    if(kDown & KEY_DLEFT && botonSeleccionado.x > 0) botonSeleccionado.x--;
				id = posicionAid(botonSeleccionado);

				if(kDown & KEY_A && id < numJugadores && !palabraVistaJugador[id]){
					viendoPalabra = id;
				}
			} else if(!palabraVistaJugador[viendoPalabra]){
				if(tocandoRectangulo(touch, 40, 40, 240, 150)){
					palabraVistaJugador[viendoPalabra] = true;
					clrTarjeta = colorTextoTarjeta(viendoPalabra, numImpostores);
					crearTextoTarjeta(viendoPalabra, numImpostores, palabrasRonda);
				}
			} else{
				if(tocandoRectangulo(touch, 120, 200, 100, 35)){
					borrarTextoTarjeta();
					viendoPalabra = -1;
					numPalabrasVistas++;
				}
			}

			if(numPalabrasVistas == numJugadores){
				jugadorComienza = primerJugador(numJugadores);
				tickInicial = svcGetSystemTick();
				menuActivo = MENU_RESULTADOS;
			}
		}

		else if(menuActivo == MENU_RESULTADOS){
			C2D_SceneBegin(bottom);
			if(!partidaTerminada){
			    C2D_DrawText(&g_dynamicText[14], C2D_WithColor | C2D_AlignCenter, 160, 70, 0.0f, 0.8f, 0.8f, clrWhite);
			    C2D_DrawText(&g_dynamicText[15], C2D_WithColor | C2D_AlignCenter, 160, 95, 0.0f, 0.8f, 0.8f, clrWhite);
			    C2D_DrawRectSolid(70, 130, 0.0f, 180, 35, clrBlue);
			    C2D_DrawText(&g_staticText[21], C2D_WithColor | C2D_AlignCenter, 160, 140, 0.0f, 0.6f, 0.6f, clrWhite);
				if(tocandoRectangulo(touch, 70, 130, 180, 35) && !pulsandoPantallaUltimoFrame) partidaTerminada = true;
			} else{
				C2D_DrawText(&g_dynamicText[16], C2D_WithColor | C2D_AlignCenter, 160, 70, 0.0f, 0.8f, 0.8f, clrWhite);
				C2D_DrawRectSolid(70, 130, 0.0f, 180, 35, clrBlue);
			    C2D_DrawText(&g_staticText[22], C2D_WithColor | C2D_AlignCenter, 160, 140, 0.0f, 0.6f, 0.6f, clrWhite);
				if(tocandoRectangulo(touch, 70, 130, 180, 35) && !pulsandoPantallaUltimoFrame){
					numPalabrasVistas = 0;
					viendoPalabra = -1;
					for(int i = 0; i < 6; i++){
						IDimpostores[i] = -1;
					}
					for(int i = 0; i < 12; i++){
						palabraVistaJugador[i] = false;
					}
					partidaTerminada = false;
					fclose(palabrasTXT);

					menuActivo = MENU_PRINCIPAL;
				}
			}
			C2D_SceneBegin(top);
		}
		
	    C3D_FrameEnd(0);
	}

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
