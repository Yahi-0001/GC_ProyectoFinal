#include "rostro.h"

NODO mallaFacial[CAPACIDAD];
int totalNodos = 0;
int estadoExpresion = 1; //default rostro serio

// se pasó como una variable global devido a que la función display no puede llamarla en los parámetros
int conexiones[][3] = {
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

int totalConexiones = sizeof(conexiones) / sizeof(conexiones[0]);


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

