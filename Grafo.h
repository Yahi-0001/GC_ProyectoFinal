#ifndef GRAFO_H
#define GRAFO_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// definir constantes
#define CAPACIDAD 100
#define MEDIDA_ORIGEN 32


//estructura de las coordenadas
typedef struct punto
{
	float x;
	float y;
	float z; //nuevo eje

}PUNTO;


//estructura del grafo
typedef struct nodo
{
	int key;
	PUNTO coordenada;// dato del grafo, las coordenadas
	struct hash *aristas; //catalogo de arista
	char nombreAnatomico[40];// para los nombres de los puntos de la cara
	
}NODO;

typedef struct arista
{
	NODO *destino;
	int costo;

}ARISTA;


typedef struct hash
{ 
	ARISTA **destinos;// puntero a donde queremos ir 
	int *disponible;
	unsigned int *radar; // es para darnos cuenta en que casillas estan las colisiones 
	int capacidadActual;
}HASH;


// prototipos de funciones
NODO *crearNodo(PUNTO coordenada, HASH *aristas, int key, const char *nombreAnatomico);
ARISTA *crearArista(NODO *destino, int costo);
HASH *crearHash(int capacidadInicial);
int hashPorDivision(int key, int capacidad);
int insertarArista(NODO *destino, int costo, HASH *tabla);
int insertarNodo(NODO **nodos, int posicion, PUNTO coordenada, int key, const char *nombreAnatomico);
int juegoRayuela(HASH *tabla, int origen, NODO *destino, int costo, int capacidad);
HASH *rehashing(HASH *tablaVieja);
int unirNodos(NODO *catalogo, int idOrigen, int idDestino, int costo);
void liberarGrafo(NODO *catalogo, int totalNodos, int capacidadActual);

#endif