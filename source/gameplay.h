#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

FILE *palabrasTXT;

typedef struct {
    char palabraJugadores[30];
    char pistaImpostor[30];
} palabrasPartida;

int IDimpostores[6] = {-1, -1, -1, -1, -1, -1};

bool abrirPalabrasTXT() {
    palabrasTXT = fopen("romfs:/palabras_impostor.txt", "r");
    if(palabrasTXT == NULL) return false;
    else return true;
}

int getNumPalabrasArchivo() {
    char buffer[10];
    int numPalabrasArchivo;
    fgets(buffer, 10, palabrasTXT);
    sscanf(buffer, "%d", &numPalabrasArchivo);
    if(numPalabrasArchivo > 0) return numPalabrasArchivo;
    else return -1;
}

palabrasPartida escogerPalabrasPartida(int numPalabrasArchivo) {
    char buffer[30];

    int palabraID = rand() % numPalabrasArchivo;
    for(int i = 0; i < (1 + palabraID * 5); i++){
        fgets(buffer, 30, palabrasTXT);
    }
    fgets(buffer, 30, palabrasTXT);
    char palabraJugadores[30];
    strncpy(palabraJugadores, buffer, 29);

    int pistaID = rand() % 3;
    for(int i = 0; i < pistaID - 1; i++){
        fgets(buffer, 30, palabrasTXT);
    }
    fgets(buffer, 30, palabrasTXT);
    char pistaImpostor[30];
    strncpy(pistaImpostor, buffer, 29);
    
    palabrasPartida palabrasPartida;
    strncpy(palabrasPartida.palabraJugadores, palabraJugadores, 29);
    strncpy(palabrasPartida.pistaImpostor, pistaImpostor, 29);
    return palabrasPartida;
}

bool cerrarPalabrasTXT() {
    if(fclose(palabrasTXT) == NULL) return false;
    else return true;
}

bool comprobarImpostoresRepetidos(int numJugadores, int numImpostores){
    for(int i = 0; i < numImpostores - 1; i++){
        for(int j = i + 1; j < numImpostores; j++){
            if(IDimpostores[i] == IDimpostores[j]) return true;
        }
    }
    return false;
}

void crearImpostores(int numJugadores, int numImpostores){
    do{
        for(int i = 0; i < numImpostores; i++){
            IDimpostores[i] = rand() % numJugadores;
        }
    } while(comprobarImpostoresRepetidos(numJugadores, numImpostores));
}