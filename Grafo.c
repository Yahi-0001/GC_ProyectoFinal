#include "Grafo.h"

/*
FUNCIONES DE CREACIÓN

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

NODO *crearNodo(PUNTO coordenada, HASH *aristas, int key, const char *nombreAnatomico)
{
	NODO *nuevoNodo = (NODO*)malloc(sizeof(NODO));
	if (nuevoNodo == NULL)
	{
		return NULL;
	}
	nuevoNodo->key = key;
	nuevoNodo->coordenada = coordenada;
	nuevoNodo->aristas = aristas; // si la dejo en null eso significa que no tiene espacio 
	strcpy(nuevoNodo->nombreAnatomico, nombreAnatomico);
	return nuevoNodo;
}

ARISTA *crearArista(NODO *destino, int costo){
	ARISTA *nuevaArista = (ARISTA*)malloc(sizeof(ARISTA));
	if(nuevaArista == NULL)
	{
		return NULL;
	}
	nuevaArista->destino = destino;
	nuevaArista->costo = costo;
	return nuevaArista;
}

/*
 capcacidadInicial = dependiendo el numero de la funcion unirNodos

 se utilizó calloc porque me sirve para Rayuela porque calloc 
 llena los byte de memoria con ceros


 tengo disponible, destinos y radar entonces eso me dice que que estan libres las casillas
 mientras que malloc no rellena con 0 entonces tendría que hacer otro paso para pasar a 0 el espacio libre
 */
HASH *crearHash(int capacidadInicial)
{
	HASH *nuevoHash = (HASH*)malloc(sizeof(HASH));
	if(nuevoHash == NULL)
	{
		return NULL;
	}

	nuevoHash->destinos = (ARISTA**)calloc(capacidadInicial, sizeof(ARISTA*));
	nuevoHash->disponible = (int*)calloc(capacidadInicial, sizeof(int));
	nuevoHash->radar = (unsigned int*)calloc(capacidadInicial, sizeof(unsigned int));
	nuevoHash->capacidadActual = capacidadInicial;
	return nuevoHash;
}

/*
FUNCIÓN MATEMÁTICA

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/
int hashPorDivision(int key, int capacidad)
{


	/*
	 parte 1: la función hash
	 ORIGEN = key_destino % tamaño_tabla
	*/

	int origen;

	origen = key % capacidad;

	return origen;
}


/*
FUNCIONES DE INSERCIÓN

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

int insertarArista(NODO *destino, int costo, HASH *tabla)
{
	//tomar la key y meterla a la función division y guradar el dato
	int dato = hashPorDivision(destino->key, CAPACIDAD);

	if(tabla->disponible[dato] == 0)
	{
		tabla->destinos[dato] = crearArista(destino, costo);
		tabla->disponible[dato] = 1; //ocupado
		tabla->radar[dato] |= 1;//radar encendido
	}
	else
	{
		int exito = juegoRayuela(tabla, dato, destino, costo, CAPACIDAD);

		if(exito == 0)
		{
			printf("Colapso en hopscotch, Iniciando rehashing...\n");
			tabla = rehashing(tabla);
			insertarArista(destino, costo, tabla);
		}
	}
	return 1;
}

int insertarNodo(NODO **nodos, int posicion, PUNTO coordenada, int key, const char *nombreAnatomico)
{
	HASH *tabla = crearHash(CAPACIDAD);
	nodos[posicion] = crearNodo(coordenada, tabla, key, nombreAnatomico) ;
	return 1;
}


/*
JUEGO DE RAYUELA

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

// función para la parte 2 y 3
int juegoRayuela(HASH *tabla, int origen, NODO *destino, int costo, int capacidad)
{
	/*
	 algoritmo revisa la casilla
	 if la casilla esta disponible = libreee
	 guardamos el puntero destino ahí mismo
	 cambiamos a estado ocupado
	 encendemos el bit correspondiente en mi radar
	*/
	int actual = origen;

	while(tabla->disponible[actual] == 1)
	{
		actual = (actual + 1) % capacidad;

	}

	/*
   if casilla esta ocupada
   caminamos hacia adelante para encontrar el primer espacio vacio
   ENCONTRADO:
     if el espacio vacio esta dentro de la medida lo pongo ahi y actualizo radar
     if esta muy lejos se va para atras 
	*/

	int distancia = (actual - origen + capacidad) % capacidad;

	while(distancia >= MEDIDA_ORIGEN)
	{
		int movido = 0;
		// programamos el swap
		for(int i = 1; i < MEDIDA_ORIGEN; i++)
		{
			int evaluar = (actual - i + capacidad) % capacidad;
			int vecino = hashPorDivision(tabla->destinos[evaluar]->destino->key, capacidad);
			int distancia_v = (actual - vecino + capacidad) % capacidad;

			if(distancia_v < MEDIDA_ORIGEN)
			{
				//swap
				tabla->destinos[actual] = tabla->destinos[evaluar];
				int dist_anterior = (evaluar - vecino + capacidad) % capacidad;
				tabla->radar[vecino] &= ~(1 << dist_anterior);
				tabla->radar[vecino] |= (1 << distancia_v);
				actual = evaluar;
				movido = 1;
				break;
			}
		}

		if(movido == 0)
		{
			return 0;
		}
		distancia = (actual - origen + capacidad) % capacidad;
			
	}

	tabla->destinos[actual] = crearArista(destino, costo);
	tabla->disponible[actual] = 1;
	tabla->radar[origen] |= (1 << distancia);
	return 1;
}

/*
FUNCIÓN DE HASH

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/
/*
Para qué necesito un reahshing si ya creo un hash?

porque mi hash tiene un limite, si mi tabla se satura mi algoritmo rayuela 
no puede funcionar y colapsa, el rehashing crea más espacio, muda los datos
anteriores y elimina la tabla vieja

- Construcción del nuevo hash:
	tomar la capacidad original y duplicarla

- Revisión el hash viejo
	recorrer el hash buscamos aristas guardadas en la casilla

- Extracción y recalculo
	estraemos el puntero del nodo destino y su costo aplicamos la formula de division
	y se agregaran en su nuevo destino segun lo calculado

- inserción
	revisamos la nueva casilla calculada
	if libre crear arista ahí, marcar ocupado y encender radar
	en caso de choques rayuela

- Lineración de memoria
	eliminamos liberando la memoria la tabla vieja
*/
HASH *rehashing(HASH *tablaVieja)
{
	int nuevaCapacidad = tablaVieja->capacidadActual * 2;

	HASH *nuevaTabla = crearHash(nuevaCapacidad);
	if(nuevaTabla == NULL) return NULL;

	for(int i = 0; i < tablaVieja->capacidadActual; i++)
	{
		if(tablaVieja->disponible[i] == 1)
		{
			NODO *destinoGuardado = tablaVieja->destinos[i]->destino;
			int costoGuardado = tablaVieja->destinos[i]->costo;
			int nuevoDato = hashPorDivision(destinoGuardado->key, nuevaCapacidad);

			if(nuevaTabla->disponible[nuevoDato] == 0)
			{
				nuevaTabla->destinos[nuevoDato] = crearArista(destinoGuardado, costoGuardado);
				nuevaTabla->disponible[nuevoDato] = 1;
				nuevaTabla->radar[nuevoDato] |= 1;
			}
			else
			{
				juegoRayuela(nuevaTabla, nuevoDato, destinoGuardado, costoGuardado, nuevaCapacidad);
			}
		}
	}

	free(tablaVieja->destinos);
	free(tablaVieja->disponible);
	free(tablaVieja->radar);
	free(tablaVieja);

	return nuevaTabla; 
}

/*
FUNCIONES PARA CREAR GRAFO

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

int unirNodos(NODO *catalogo, int idOrigen, int idDestino, int costo)
{
	if(catalogo[idOrigen].aristas == NULL)
	{
		catalogo[idOrigen].aristas = crearHash(CAPACIDAD);
	}
	if (catalogo[idDestino].aristas == NULL)
	{
		catalogo[idDestino].aristas = crearHash(CAPACIDAD);
	}

	// solo una dirección
	insertarArista(&catalogo[idDestino], costo, catalogo[idOrigen].aristas);
	return 1;
}

/*
 función para verificar que si funcione mi grafo
void imprimirGrafo(NODO *catalogo, int totalNodos, int capacidadActual)
{
	printf("\n CONEXIONES DE LA MALLA FACIAL\n");

	for(int i = 0; i < totalNodos; i++)
	{
		printf("Vertice %d (X: %.2f, Y: %.2f) concetado con:",
			     catalogo[i].key, catalogo[i].coordenada.x, catalogo[i].coordenada.y);

		if(catalogo[i].aristas != NULL)
		{
			int conexiones = 0;
			for(int j = 0; j < capacidadActual; j++)
			{
				if(catalogo[i].aristas->disponible[j] == 1)
				{
					printf("[%d]", catalogo[i].aristas->destinos[j]->destino->key);
					conexiones++;
				}
			}
			if(conexiones == 0)
			{
				printf("Ninguno");
			}
		}
		else
		{
			printf("Tabala no inicializada");
		}
		printf("\n");
	}
}
*/

void liberarGrafo(NODO *catalogo, int totalNodos, int capacidadActual)
{
	for(int i = 0; i < totalNodos; i++)
	{
		if(catalogo[i].aristas != NULL)
		{
			for(int j = 0; j < capacidadActual; j++)
			{
				if(catalogo[i].aristas->disponible[j] == 1)
				{
					free(catalogo[i].aristas->destinos[j]);
				}
			}
			free(catalogo[i].aristas->destinos);
			free(catalogo[i].aristas->disponible);
			free(catalogo[i].aristas->radar);
			free(catalogo[i].aristas);
		}
	}
	printf("\nMemoria liberada.\n");
}

