#include <citro2d.h>

typedef struct{
    int x;
    int y;
} Vector2;

Vector2 botonSeleccionado = {0, 0};

Vector2 idAPosicion(int id) {
    int fila = id / 3;
    int columna = id % 3;
    Vector2 posicion = { columna, fila };
    return posicion;
}

int posicionAid(Vector2 posicion){
    return 3 * posicion.y + posicion.x;
}

u32 colorBoton(Vector2 posicion, int id, int numJugadores) {
    if(numJugadores - 1 < id) return C2D_Color32(0x00, 0x00, 0x80, 0xFF);
    else if(posicionAid(posicion) == id) return C2D_Color32(0x80, 0x80, 0xFF, 0xFF);
    else return C2D_Color32(0x30, 0x30, 0xFF, 0xFF);
}

void crearMenuGameplayArriba(int numJugadores){
    C2D_DrawRectSolid(14, 45, 0.0f, 118, 37, colorBoton(botonSeleccionado, 0, numJugadores));
	C2D_DrawRectSolid(139, 45, 0.0f, 118, 37, colorBoton(botonSeleccionado, 1, numJugadores));
	C2D_DrawRectSolid(264, 45, 0.0f, 118, 37, colorBoton(botonSeleccionado, 2, numJugadores));

	C2D_DrawRectSolid(14, 89, 0.0f, 118, 37, colorBoton(botonSeleccionado, 3, numJugadores));
	C2D_DrawRectSolid(139, 89, 0.0f, 118, 37, colorBoton(botonSeleccionado, 4, numJugadores));
	C2D_DrawRectSolid(264, 89, 0.0f, 118, 37, colorBoton(botonSeleccionado, 5, numJugadores));

	C2D_DrawRectSolid(14, 133, 0.0f, 118, 37, colorBoton(botonSeleccionado, 6, numJugadores));
	C2D_DrawRectSolid(139, 133, 0.0f, 118, 37, colorBoton(botonSeleccionado, 7, numJugadores));
	C2D_DrawRectSolid(264, 133, 0.0f, 118, 37, colorBoton(botonSeleccionado, 8, numJugadores));

	C2D_DrawRectSolid(14, 177, 0.0f, 118, 37, colorBoton(botonSeleccionado, 9, numJugadores));
	C2D_DrawRectSolid(139, 177, 0.0f, 118, 37, colorBoton(botonSeleccionado, 10, numJugadores));
	C2D_DrawRectSolid(264, 177, 0.0f, 118, 37, colorBoton(botonSeleccionado, 11, numJugadores));
}