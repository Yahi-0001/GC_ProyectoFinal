#ifndef ROSTRO_H
#define ROSTRO_H

#include "grafo.h"

extern NODO mallaFacial[CAPACIDAD];
extern int totalNodos;

extern int conexiones[][3];
extern int totalConexiones;

extern int estadoExpresion;

int lecturaArchivo(NODO *catalogo, int *total);
void obtenerTraslacion(int id, float *tx, float *ty, float *tz);

#endif