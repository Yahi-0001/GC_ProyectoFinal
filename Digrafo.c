/*
CAPA 1: El Grafo

 - función para inicializar
 - Función para recibir las coordenadas y devuelva un nodo 
 - El jefe rayuela 
 - función paara saber si mi grafo funciona, imprimir los vecinos conectados
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <GL/freeglut.h>

//para la textura
#define TEX_WIDTH 64
#define TEX_HEIGHT 64
static GLuint texture_id;

//cargar iluminación
const GLfloat light_ambiente[] = {0.1f, 0.1f, 0.1f, 0.1f};
const GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat light_position[] = {3.0f, 6.0f, 6.0f, 1.0f};

//propiedades del material
const GLfloat mat_ambiente[] = {0.1f, 0.7f, 0.1f, 0.1f};
const GLfloat mat_diffuse[] = {0.1f, 0.7f, 0.1f, 0.1f};
const GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat mat_shininess[] = {100.0f};


// definir constantes
#define CAPACIDAD 100
#define MEDIDA_ORIGEN 32

//datos de rotación
static double rot_x = 0.0;
static double rot_y = 0.0;

// variables traslación
static float pos_x = 0.0f;


//estructura de las coordenadas
typedef struct punto
{
	float x;
	float y;
	float z; //nuevo eje

}PUNTO;

/*
El NODO representa el vertice de mi grafo, que define  quien es con su key y donde esta
con las coordenadas  y posee un diccionario de adyacencias que son las aristas que es un 
puntero a HASH, cada nodo tiene su propia tabla privada.

La ARISTA es la prepresentación del enlace dirigido y con costo entre dos vertices, guardamos el puntero al NODO de destino
entonces eso evita duplicados en memoria, esta preparado por si necesito navegar por la cara en un fututo.

El HASH es la tabla con la resolucion de colisiones por Hopscoth es una lista de adyacencia del nodo 
que nos ayuda con la busqueda de una conexión conotro nodo.
*/


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

//estructura del hash (hopscoth hashing)
/*
Para el hopscoth hay que considerar lo siguiente:

 - Cada dato tiene un lugar ORIGINAL el indicie que se cálcula y esta obligado a vivir cerca de el origen,
   tenemos que dar un número máximo para los saltos en el vecindario.

 - Si llega una nueva arista a mi tabla y la casilla esta ocupada avanzamos hacia adelante para encontrar,
   un espacio vacio.

 - Si el espacio vacio esta demasiado lejos del ORIGEN el dato no se puede quedar ahí entonces el algoritmo 
   hará lo siguiente:

     * Va a mirar a los vecinso cercanos del espacio vacio y les va a preguntrar que si se pueden mover a la
       casilla vacía, pero lejana del ORIGEN

     * Si la respuesta es si, se pasa el vecino a la casilla lejana y se repite hasta que el espacio vacio quede 
       dentro del vecindario del ORIGEN.
*/


//static int medida = 32;  32 tamaño que se puede alejar del ORIGEN
// static int capacidad = 100;

// Variables gloables para las funciones del callback que no permite recibir argumentos por referencia
static NODO mallaFacial[CAPACIDAD];
static int totalNodos = 0;

// se pasó como una variable global devido a que la función display no puede llamarla en los parámetros
static int conexiones[][3] = {
  //oreja
	{40, 39, 34}, {34, 41, 40}, {41, 34, 42}, {34, 42, 33},

	//cejas
	{20, 19, 22}, {19, 22, 18}, {18, 22, 17}, {22, 17, 21},

	//nariz
	{3, 23, 4}, {23, 4, 24}, {26, 6, 5},{24, 4, 25}, {25, 5, 4},

	//labio superior
	{6, 7, 26}, {7, 26, 28}, {28, 26, 27}, {27, 26, 47},

	//labio inferior
	{27, 30, 29}, {30, 29, 9}, {29, 8, 9},

	// relleno triangulos por arriba de las cejas
	{0, 1, 46}, {1, 2, 46}, {2, 3, 17}, {3, 17, 21}, {0, 46, 38}, 
	{38, 46, 44}, {38, 37, 44}, {37, 36, 44}, {36, 44, 19}, {44, 19, 18},
	{18, 44, 46}, {18, 46, 17}, {36, 20, 19}, {36, 35, 20}, {46, 2, 17}, {20, 22, 12},
	
	//relleno triangulos de los ojos y de la nariz
	{35, 20, 11}, {20, 11, 12}, {12, 22, 21}, {12, 21, 13}, {13, 21, 14},
	{21, 14, 3}, {3, 14, 23}, {23, 14, 24}, {14, 24, 47},
	{47, 14, 15}, {16, 15, 47}, {16, 45, 47}, {11, 16, 45},
	{45, 39, 11}, {39, 34, 45}, {39, 35, 11}, {47, 5, 26}, {47, 25, 24},

	//relleno triangulos debajo de la nariz y labios superiores
	{34, 45, 33}, {45, 33, 47}, {47, 33, 48}, {48, 47, 27}, {47, 25, 5},

	//relleno triangulos por debajo del labio inferior
	{33, 48, 32}, {48, 32, 27}, { 27, 32, 31}, {27, 30, 31}, {30, 31, 43},
	{30, 9, 43}, {31, 43, 10}
};



//prototipo de funciones
NODO *crearNodo(PUNTO coordenada, HASH *aristas, int key, const char *nombreAnatomico);
ARISTA *crearArista(NODO *destino, int costo);
HASH *crearHash(int capacidadInicial);
int hashPorDivision(int key, int capacidad);
int insertarArista(NODO *destino, int costo, HASH *tabla);
int insertarNodo(NODO **nodos, int posicion, PUNTO coordenada, int key, const char *nombreAnatomico);
int juegoRayuela(HASH *tabla, int origen, NODO *destino, int costo, int capacidad);
int lecturaArchivo(NODO *catalogo, int *total);
HASH *rehashing(HASH *tablaVieja);
int unirNodos(NODO *catalogo, int idOrigen, int idDestino, int costo);
//void imprimirGrafo(NODO *catalogo, int totalNodos, int capacidadActual);
void liberarGrafo(NODO *catalogo, int totalNodos, int capacidadActual);
void asignarColorPunto(int key, int reflejo);
void obtenerTraslacion(int id, float *tx, float *ty, float *tz);
void redimensionar(int w, int h);
void teclado(unsigned char tecla, int x, int y);
void teclasEspeciales(int tecla, int x, int y);
//void menu();
void display(void);
void cargar_textura(void);
void dibujarCirculo(float cx, float cy, float cz, float radio, int num_segmentos);
void dibujarOjo(float cx, float cy, float cz, float escala);
void init(void);
void vistaFrustum(int const win_width, int const win_height);

// prototipos de funciones de opengl

/*
 Modificiación ya que esto es estatico y me di cuenta que al ingresar otras coordenadas todo se desordeno y tuve que cambiar 
 manualmente los id lo cual no es muy cómodo y lo voy a hacer dinámico agregando a NODO  una nueva variable para los nombres.

// diccionario anatomico
const char* nombreAnatomico(int key)
{
	if(key >= 0 && key <= 10)
		return "Linea_Central";
	if(key >= 11 && key <= 16)
		return "Ojo";
	if(key >= 17 && key <= 22) 
		return "Ceja";
	if(key >= 23 && key <= 25)
		return "Nariz";
	if(key >= 26 && key <= 29)
		return "Boca";
	if(key >= 30 && key <= 38)
		return "Contorno";
	if(key >= 39 && key <= 42)
		return "Oreja";
	if(key >= 43 && key <= 48) 
		return "Relleno_Malla";
		
	return "Desconocido";
}*/

//main
int main(int argc, char **argv)
{
	
	lecturaArchivo(mallaFacial, &totalNodos);

	//matriz de la conexion

	int totalConexiones = sizeof(conexiones) / sizeof(conexiones[0]);
	for(int i = 0; i < totalConexiones; i++)
	{
		unirNodos(mallaFacial, conexiones[i][0], conexiones[i][1], 10);
	}

	//imprimirGrafo(mallaFacial, totalNodos, CAPACIDAD);

	//menu();
	//inicializar
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE ); //habilitando
	glutInitWindowSize(600, 600); // es nuestro canva tamaño
	glutInitWindowPosition(200,200);//donde va a aparecer la ventana
	glutCreateWindow("Cara tringulada - Yahaira Garcia");
	init();
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //trabajamos con el valor alfa es el ultimo numero
	glutDisplayFunc(display);
	glutKeyboardFunc(teclado);
	glutSpecialFunc(teclasEspeciales);
	glutReshapeFunc(redimensionar);
	cargar_textura();//agregada fase2
	glutMainLoop();
	liberarGrafo(mallaFacial, totalNodos, CAPACIDAD);
	printf("Memoria liberada exitosamente.\n");
	return 0;
}

/*
FUNCIONES DE MENÚ

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

/*
Ya no utilizo esta función del menú asi que se debe de quitar 
void menu()
{
	printf("\nEXPRESIONES FACIALES\n");
	printf("[1] Rostro serio (default)\n");
	printf("[2] Rostro feliz\n");
	printf("[3] Rostro enojado\n");
	printf("[4] Rostro triste\n");
	printf("[Q] o [ESC] Salir\n");
}*/

// Variable global para controlar la expresión
int estadoExpresion = 1; 

void teclado(unsigned char tecla, int x, int y)
{
	switch(tecla) 
	{
		case '1': 
			estadoExpresion = 1; // Default
			break; 
		case '2': 
			estadoExpresion = 2; // Feliz
			break; 
		case '3': 
			estadoExpresion = 3; // Enojado
			break; 
		case '4': 
			estadoExpresion = 4; // Triste
			break;
		case 'q': 
		case 'Q':
		case 27:  // 27 es el codigo ASCII de la tecla ESC
			exit(0); 
			break;
	}
	glutPostRedisplay(); 
}


void teclasEspeciales(int tecla, int x, int y)
{
	switch(tecla) 
	{
		case GLUT_KEY_RIGHT: 
			rot_y += 5.0; 
			break;
		case GLUT_KEY_LEFT:  
			rot_y -= 5.0; 
			break;
		case GLUT_KEY_UP:    
			rot_x += 5.0; 
			break;
		case GLUT_KEY_DOWN:  
			rot_x -= 5.0; 
			break;
		case GLUT_KEY_F1:    
			pos_x += 5.0; 
			break;
		case GLUT_KEY_F2:  
			pos_x -= 5.0; 
			break;
	}
	glutPostRedisplay(); 
}

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
LECTURA DE ARCHIVO

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

int lecturaArchivo(NODO *catalogo, int *total)
{
	FILE *archivo;
	char linea[100];
	int i = 0;

	archivo = fopen("coordenadas.txt", "r");

	if(archivo == NULL)
	{
		printf("\nError de apertura del archivo. \n\n");
		return 0;
	}
	else
	{
		
		while(fgets(linea,sizeof(linea), archivo) != NULL)
		{
			float temp_x, temp_y, temp_z; //se agrega profundidad
			char temp_zona[40]; // variable temporal para guardar la zona
			
			if(sscanf(linea, "%f %f %f %s", &temp_x, &temp_y, &temp_z, temp_zona) == 4)
			{
				catalogo[i].key = i;
				catalogo[i].coordenada.x = temp_x;
				catalogo[i].coordenada.y = temp_y;
				catalogo[i].coordenada.z = temp_z;
				catalogo[i].aristas = NULL;
				strcpy(catalogo[i].nombreAnatomico, temp_zona);
				
				i++;
			}
		}
		fclose(archivo);
		*total = i; // guardamos el total de los nodos leidos

		// Imprimir nodos del lado izquierdo
		// % -( significa hacia la izquierda) 4 (ancho de la columna la reserva de espacios)s
		printf("NODOS LADO IZQUIERDO\n");
		printf("%-4s  %-15s  %-8s  %-8s\n", "ID", "Zona", "X", "Y");

	
		for(int j = 0; j < *total;  j++)
		{
			printf("%-4d  %-15s  %-8.3f  %-8.3f\n", 
			catalogo[j].key, 
			catalogo[j].nombreAnatomico, 
			catalogo[j].coordenada.x, 
			catalogo[j].coordenada.y);
		}

		// imprimir lado derecho
		int idDerecho = *total; // para empezar donde termino el izquierdo
		printf("NODOS LADO DERECHO\n");
		for(int j = 0; j < *total; j ++)
		{
			if(catalogo[j].coordenada.x != 0.0f) // Ignoramos el centro
		 {
		 	printf("%-4d  %-15s  %-8.3f  %-8.3f\n", 
			idDerecho, 
			catalogo[j].nombreAnatomico, 
			catalogo[j].coordenada.x * -1.0f, // Reflejamos la X
			catalogo[j].coordenada.y);
			idDerecho++;
			}
		}

		// imprimimos menú
		printf("\n\nPRESIONA ESTAS TECLAS EN LA VENTANA GRAFICA:\n");
		printf("  [1] Rostro serio (default)\n");
		printf("  [2] Rostro feliz\n");
		printf("  [3] Rostro enojado\n");
		printf("  [4] Rostro triste\n");
		printf("  [Q] o [ESC] Salir\n");
	}
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


/*
CAPA 2: REFLEXION

≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/


// solo es para que se note la diferencia entre los puntos importantes y los otros 
void asignarColorPunto(int key, int reflejo)
{
	if( (key >= 11 && key <= 22) || // Ojos (11 al 16) y Cejas (17 al 22)
	    (key >= 6 && key <= 9)   || // Centro de la boca (6, 7, 8, 9)
	    (key >= 26 && key <= 30) || // Resto de la boca (26 al 30)
	    key == 1 || key == 46 ||    // Frente
	    key == 4 || key == 25 ||    // Nariz
	    key == 10 || key == 31 )    // Barbilla
	{
		glColor3f(0.0f, 0.0f, 0.0f); // Azul para el original y reflejo
	}
	else 
	{
		glColor3f(0.0f, 0.0f, 0.0f); // Rojo para todo el contorno y mejillas
	}
}

/*
OPEN GL 
utiliza para la reflexion Pila de matrices  en donde tenemos glPushMatrix y glPopMatrix

- glPushMatrix:
	saca una fotocopia exacta de la matriz actual  y empujarla hasta arriba de la pila
	lo que nos deja dos matrices identicas, la original de abajo y la fotocopia de arriba
	OPEN GL solo va a alterar la fotocopia de arriba.

- glScalef(-1.0f, 1.0f, 1.0f):
	la reflexión matematicas multiplicando todo el eje x por -1, la matriz copiada esta siendo utilizada
	para dibujar el otro lado de la cara.
	matriz original guardada.

- glPopMatrix:
	cuando terminamos de dibujar el lado contrario, necesitamos que el grafico vuelva a la normalidad
	llamamos a pop toma la matriz alterada que estaba hasta arriba de la pila y la destruyes
	la matriz original que estaba guardada abajo vuelve a quedar arriba de la pila 

*/
// mi motor de renderizado 
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // limpiar pantalla
	glEnable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glShadeModel(GL_SMOOTH);


  GLfloat luz_ambiente[] = { 0.45f, 0.45f, 0.45f, 1.0f }; 
  GLfloat luz_difusa[]   = { 0.9f,  0.85f, 0.8f,  1.0f };
  GLfloat posicion_luz[] = { 0.0f, 0.0f, 10.0f, 0.0f };

  // brillo para hacer la linea del tabique
  GLfloat luz_especular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat brillo_material[] = { 25.0f };

  glLightfv(GL_LIGHT0, GL_AMBIENT, luz_ambiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_difusa);
  glLightfv(GL_LIGHT0, GL_POSITION, posicion_luz);
  glLightfv(GL_LIGHT0, GL_SPECULAR, luz_especular);

   
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glMaterialfv(GL_FRONT, GL_SPECULAR, luz_especular);
    glMaterialfv(GL_FRONT, GL_SHININESS, brillo_material);

	glTranslatef(0.0f, -0.4f, -5.0f);
	glScalef(4.8f, 4.8f, 1.0f);

	glTranslatef(pos_x, 0.0f, 0.0f);
	glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
	glRotatef(rot_y, 0.0f, 1.0f, 0.0f);

	GLfloat light_position[] = { 1.0f, 1.0f, 3.0f, 1.0f }; 
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//activamos la textura
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	//dibujar el fondo
	glColor3f(1.0f, 1.0f, 1.0f);
	
	int totalConexiones = sizeof(conexiones) / sizeof(conexiones[0]);
	/*
	 Recorremos el arreglo de nodos, si tiene una tabla, entra y revisa
	 por cada conexion lee las coordenadas de origen y destino
	 tx y ty manda a llamar traslacion para que se mueva
	*/
	glBegin(GL_TRIANGLES);
	//glNormal3f(0.0f, 0.0f, 1.0f);

	for(int i = 0; i < totalConexiones; i++)
	{
		int id1 = conexiones[i][0];
		int id2 = conexiones[i][1];
		int id3 = conexiones[i][2];

		float tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3;
		obtenerTraslacion(id1, &tx1, &ty1, &tz1);
		obtenerTraslacion(id2, &tx2, &ty2, &tz2);
		obtenerTraslacion(id3, &tx3, &ty3, &tz3);

		float v1x = mallaFacial[id1].coordenada.x + tx1;
		float v1y = mallaFacial[id1].coordenada.y + ty1;
		float v1z = mallaFacial[id1].coordenada.z + tz1;

		float v2x = mallaFacial[id2].coordenada.x + tx2;
		float v2y = mallaFacial[id2].coordenada.y + ty2;
		float v2z = mallaFacial[id2].coordenada.z + tz2;

		float v3x = mallaFacial[id3].coordenada.x + tx3;
		float v3y = mallaFacial[id3].coordenada.y + ty3;
		float v3z = mallaFacial[id3].coordenada.z + tz3;

		float nx = ((v2y - v1y) * (v3z - v1z)) - ((v2z - v1z) * (v3y - v1y));
		float ny = ((v2z - v1z) * (v3x - v1x)) - ((v2x - v1x) * (v3z - v1z));
		float nz = ((v2x - v1x) * (v3y - v1y)) - ((v2y - v1y) * (v3x - v1x));

		if (nz < 0.0f) 
		{
			nx = -nx;
			ny = -ny;
			nz = -nz;
		}

		glNormal3f(nx, ny, nz);

		float u1, v1, u2, v2, u3, v3;

		// Evaluamos qué triángulo es para saber de qué parte de la textura leer
		if (i >= 4 && i <= 7) 
		{
			// cejas
			u1 = u2 = u3 = 0.5f;
			v1 = v2 = v3 = 0.85f;
		}
		else if ((i >= 13 && i <= 15) || (i >= 17 && i <= 19)) 
		{
			// labios
			u1 = u2 = u3 = 0.5f;
			v1 = v2 = v3 = 0.15f;
		}
		else 
		{
			u1 = (mallaFacial[id1].coordenada.x + 1.0f) / 2.0f;
			v1 = 0.5f; 
			
			u2 = (mallaFacial[id2].coordenada.x + 1.0f) / 2.0f;
			v2 = 0.5f; 
			
			u3 = (mallaFacial[id3].coordenada.x + 1.0f) / 2.0f;
			v3 = 0.5f; 
		}

		glTexCoord2f(u1, v1); 
		glVertex3f(v1x, v1y, v1z);
		glTexCoord2f(u2, v2); 
		glVertex3f(v2x, v2y, v2z);
		glTexCoord2f(u3, v3); 
		glVertex3f(v3x, v3y, v3z);
	}
	glEnd();

	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f); 

	glBegin(GL_TRIANGLES);
	//glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i < totalConexiones; i++)
	{
		int id1 = conexiones[i][0];
		int id2 = conexiones[i][1];
		int id3 = conexiones[i][2];

		float tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3;
		obtenerTraslacion(id1, &tx1, &ty1, &tz1);
		obtenerTraslacion(id2, &tx2, &ty2, &tz2);
		obtenerTraslacion(id3, &tx3, &ty3, &tz3);

		float v1x = mallaFacial[id1].coordenada.x + tx1;
		float v1y = mallaFacial[id1].coordenada.y + ty1;
		float v1z = mallaFacial[id1].coordenada.z + tz1;

		float v2x = mallaFacial[id2].coordenada.x + tx2;
		float v2y = mallaFacial[id2].coordenada.y + ty2;
		float v2z = mallaFacial[id2].coordenada.z + tz2;

		float v3x = mallaFacial[id3].coordenada.x + tx3;
		float v3y = mallaFacial[id3].coordenada.y + ty3;
		float v3z = mallaFacial[id3].coordenada.z + tz3;

		float nx = -(((v2y - v1y) * (v3z - v1z)) - ((v2z - v1z) * (v3y - v1y)));
		float ny = ((v2z - v1z) * (v3x - v1x)) - ((v2x - v1x) * (v3z - v1z));
		float nz = ((v2x - v1x) * (v3y - v1y)) - ((v2y - v1y) * (v3x - v1x));

		if (nz < 0.0f) 
		{
			nx = -nx;
			ny = -ny;
			nz = -nz;
		}
		
		glNormal3f(nx, ny, nz);

		float u1, v1, u2, v2, u3, v3;

		if (i >= 4 && i <= 7) 
		{
			u1 = u2 = u3 = 0.5f;
			v1 = v2 = v3 = 0.85f;
		}
		else if ((i >= 13 && i <= 15) || (i >= 17 && i <= 19)) 
		{
			u1 = u2 = u3 = 0.5f;
			v1 = v2 = v3 = 0.15f;
		}
		else 
		{
			u1 = (mallaFacial[id1].coordenada.x + 1.0f) / 2.0f;
			v1 = 0.5f; 
			
			u2 = (mallaFacial[id2].coordenada.x + 1.0f) / 2.0f;
			v2 = 0.5f; 
			
			u3 = (mallaFacial[id3].coordenada.x + 1.0f) / 2.0f;
			v3 = 0.5f; 
		}

		glTexCoord2f(u1, v1); 
		glVertex3f(v1x, v1y, v1z);
		glTexCoord2f(u2, v2); 
		glVertex3f(v2x, v2y, v2z);
		glTexCoord2f(u3, v3); 
		glVertex3f(v3x, v3y, v3z);
	}
	glEnd();
	glPopMatrix(); // Restauramos el lienzo después de los triángulos

	/*
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.0f, 0.0f, 0.0f); // Color de la línea
	
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < totalConexiones; i++)
	{
		int id1 = conexiones[i][0];
		int id2 = conexiones[i][1];
		int id3 = conexiones[i][2];

		float tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3;
		obtenerTraslacion(id1, &tx1, &ty1, &tz1);
		obtenerTraslacion(id2, &tx2, &ty2, &tz2);
		obtenerTraslacion(id3, &tx3, &ty3, &tz3);

		glVertex3f(mallaFacial[id1].coordenada.x + tx1, mallaFacial[id1].coordenada.y + ty1, mallaFacial[id1].coordenada.z + tz1);
		glVertex3f(mallaFacial[id2].coordenada.x + tx2, mallaFacial[id2].coordenada.y + ty2, mallaFacial[id2].coordenada.z + tz2);
		glVertex3f(mallaFacial[id3].coordenada.x + tx3, mallaFacial[id3].coordenada.y + ty3, mallaFacial[id3].coordenada.z + tz3);
	}
	glEnd();

	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f); 
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < totalConexiones; i++)
	{
		int id1 = conexiones[i][0];
		int id2 = conexiones[i][1];
		int id3 = conexiones[i][2];

		float tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3;
		obtenerTraslacion(id1, &tx1, &ty1, &tz1);
		obtenerTraslacion(id2, &tx2, &ty2, &tz2);
		obtenerTraslacion(id3, &tx3, &ty3, &tz3);

		glVertex3f(mallaFacial[id1].coordenada.x + tx1, mallaFacial[id1].coordenada.y + ty1, mallaFacial[id1].coordenada.z + tz1);
		glVertex3f(mallaFacial[id2].coordenada.x + tx2, mallaFacial[id2].coordenada.y + ty2, mallaFacial[id2].coordenada.z + tz2);
		glVertex3f(mallaFacial[id3].coordenada.x + tx3, mallaFacial[id3].coordenada.y + ty3, mallaFacial[id3].coordenada.z + tz3);
	}
	glEnd();
	glPopMatrix(); // Restauramos lienzo después de las líneas

	*/

	glEnable(GL_POINT_SMOOTH);// es para que se vean los circulos
	glEnable(GL_BLEND);// para los puntos
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // artefactos visuales para los puntos 
	glColor3f(1.0f, 0.0f, 0.0f);
	glPointSize(8.0f);

	/*
	//dibujar la mitad original
	glBegin(GL_POINTS);
	for(int i = 0; i < totalNodos; i++)
	{
		float tx = 0.0f, ty = 0.0f, tz = 0.0f;
		obtenerTraslacion(mallaFacial[i].key, &tx, &ty, &tz);

		asignarColorPunto(mallaFacial[i].key, 0);
		glVertex3f(mallaFacial[i].coordenada.x + tx, mallaFacial[i].coordenada.y + ty, mallaFacial[i].coordenada.z + tz);
	}
	glEnd();*/

	/*
	// para debug de la matriz
	glColor3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i < totalNodos; i++)
	{
		float tx = 0.0f, ty = 0.0f, tz = 0.0f;
		obtenerTraslacion(mallaFacial[i].key, &tx, &ty, &tz);

		glRasterPos3f(mallaFacial[i].coordenada.x + 0.03f, mallaFacial[i].coordenada.y + 0.03f, mallaFacial[i].coordenada.z + 0.03f);
		char texto[10];
		sprintf(texto, "%d", mallaFacial[i].key); 
		
		for(int k = 0; texto[k] != '\0'; k++)
		{
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, texto[k]);
		}
	}*/


	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f);

	/*
	glBegin(GL_POINTS);
	for(int i = 0; i < totalNodos; i++)
	{
		//dibujamos todo menos la linea central
		if(mallaFacial[i].coordenada.x != 0.0f)
		{
			float tx = 0.0f, ty = 0.0f, tz = 0.0f;
			obtenerTraslacion(mallaFacial[i].key, &tx, &ty, &tz);

			asignarColorPunto(mallaFacial[i].key, 1);
			glVertex3f(mallaFacial[i].coordenada.x + tx, mallaFacial[i].coordenada.y + ty, mallaFacial[i].coordenada.z + tz);
		}
	}
	glEnd();

	glPopMatrix(); //restauramos el lienzo

	*/
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
  

	// ojo original
	dibujarOjo(0.26f, 0.23f, 0.02f, 0.80f); // coordenada

	// Ojo izquierdo
	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f);

	dibujarOjo(0.26f, 0.23f, 0.02f, 0.80f);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers(); // mostramos el lienzo
}

/*

CAPA 3: TRASLACIÓN
≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

/*
FÓRMULA DEL PRINCIPIO DE LA TRASLACIÓN

	P nuevo = P original + T

	mi mallado original es inmutable para cambiar las expresiones no modifico los datos originales
	entonces le sumamos un desplazamiento temporal (tx y ty) antes de pintar en la pantalla
	cuando el rostro se regresa al default tx  y ty regresan a 0.0


*/
void obtenerTraslacion(int id, float *tx, float *ty, float *tz)
{
	// Por defecto, nadie se mueve (Rostro Serio)
	*tx = 0.0f;
	*ty = 0.0f;
	*tz = 0.0f;

	if (estadoExpresion == 2) //rostro feliz
	{
		// Comisuras de los labios (se estiran afuera y suben)
		if (id == 27 || id == 30) {
			*tx = -0.04f; 
			*ty = 0.05f;  
		}
		// Apertura de mejillas/pómulos (suben ligeramente)
		else if (id == 41 || id == 42 || id == 45) {
			*ty = 0.03f;  
		}
	}
	else if (estadoExpresion == 3) // rostro enojado
	{
		// Cejas internas (bajan y se juntan hacia el centro)
		if (id == 17 || id == 21) {
			*tx = 0.02f;  
			*ty = -0.04f; 
		}
		// Tensión en la boca (labios centrales suben un poco)
		else if (id == 26 || id == 28 || id == 29) {
			*ty = 0.02f;  
		}
	}
	else if (estadoExpresion == 4) // rostro triste
	{
		// Comisuras de los labios (caen)
		if (id == 27 || id == 30) {
			*tx = 0.01f;
			*ty = -0.04f; 
		}
		// Cejas internas (suben)
		else if (id == 17 || id == 21) {
			*ty = 0.03f;  
		}
	}
}



/*

CAPA 4: TEXTURA
≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

void cargar_textura(void)
{
	GLubyte imagen[TEX_HEIGHT][TEX_WIDTH][3];

	for(int i = 0; i < TEX_HEIGHT; i++)
	{
		for(int j = 0; j < TEX_WIDTH; j++)
		{

			if (i > TEX_HEIGHT * 0.7) 
			{
				imagen[i][j][0] = 150;
				imagen[i][j][1] = 135;
				imagen[i][j][2] = 120;
			}

			else if (i < TEX_HEIGHT * 0.3) 
			{
				imagen[i][j][0] = 190;
				imagen[i][j][1] = 115;
				imagen[i][j][2] = 125;
			}
			// Piel
			else 
			{
				imagen[i][j][0] = 230;
				imagen[i][j][1] = 190;
				imagen[i][j][2] = 165;
			}
		}
	}

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	//Parámetros de textura y filtrado
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Cargar los bytes
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, imagen);
}


// quiero agregar pupilas y pestañas
/*
Puedo utilizar el matematicas
*/
void dibujarCirculo(float cx, float cy, float cz, float radio, int num_segmentos) 
{
  glBegin(GL_TRIANGLE_FAN);
  for(int i = 0; i < num_segmentos; i++) 
  {
    float theta = 2.0f * 3.1415926f * (float)i / (float)num_segmentos;
    float x = radio * cosf(theta);
    float y = radio * sinf(theta);
    glVertex3f(x + cx, y + cy, cz);
  }
  glEnd();
}

// funcion par adibujar los 3 circulos de los ojos
void dibujarOjo(float cx, float cy, float cz, float escala) 
{
	glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  // iris
  glColor3f(0.45f, 0.25f, 0.14f); // Color café oscuro
  dibujarCirculo(cx, cy, cz, 0.06f * escala, 30);

  // pupila
  glColor3f(0.0f, 0.0f, 0.0f); // Color negro
  dibujarCirculo(cx, cy, cz + 0.001f, 0.03f * escala, 30);

  // brillito
  glColor3f(1.0f, 1.0f, 1.0f); // Color blanco puro
  // Desplazamos el centro un poco hacia arriba y a la derecha para dar el efecto de reflejo
  dibujarCirculo(cx + (0.015f * escala), cy + (0.015f * escala), cz + 0.002f, 0.01f * escala, 20);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

/*

CAPA 5: ILUMINACIÓN
≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/

void init(void) 
{
  glClearColor(0.10f, 0.10f, 0.14f, 1.0f); // Fondo azul oscuro / gris pizarra
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LESS);

  // renderizado
  glShadeModel(GL_SMOOTH);
  glEnable(GL_NORMALIZE);

    //habilitacion de la cara trasera
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    // motor de iluminación
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

    //parametros de la luz
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  cargar_textura();                 
}

/*

CAPA 6: PANTALLA DE FRUSTRUM
≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/
void vistaFrustum(int const win_width, int const win_height)
{
	int h = (win_height == 0) ? 1 : win_height;
	float win_aspect = (float)win_width / (float)win_height;

	glViewport(0, 0, win_width, win_height);
	glClearColor(0.3, 0.3, 0.6, 1.0);//color de fondo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	float zoom = 0.08f; 
	glFrustum(
		-win_aspect * zoom, 
		win_aspect * zoom, 
		-1.0f * zoom, 
		1.0f * zoom, 
		1.0, 
		20.0
	);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	display();


	glutSwapBuffers();
}


// utilizando la camara ortográfica cuidamos que no se desfigure la cara
/*
los tamaños son de la pantalla glutReshapeFunc(redimensionar) gracias a esto vemos el tamaño

- evitar que colapse
	if (h == 0) h = 1; si se minimiza la ventana previene que se crashe

- ajustar la cámara
	glViewport(0, 0, w, h);

- recalcular la proporción
	glMatrixMode(GL_PROJECTION); //avisa que va a haber modificacion
	glLoadIdentity();
	float aspecto = (float)w / (float)h;

- glOrtho
*/
void redimensionar(int w, int h)
{
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float aspecto = (float)w / (float)h;

	//agregamos frustrum
	glFrustum(-aspecto, aspecto, -1.0, 1.0, 1.0, 10.0);

	/*
	if (w >= h) {
		glOrtho(-1.2 * aspecto, 1.2 * aspecto, -1.2, 1.2, -1.0, 1.0);
	} else {
		glOrtho(-1.2, 1.2, -1.2 / aspecto, 1.2 / aspecto, -1.0, 1.0);
	}*/

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
