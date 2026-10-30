#include <stdio.h>
#include <GL/freeglut.h>

#include "Grafo.h"
#include "Rostro.h"
#include "Iluminacion.h"
#include "Render.h"
#include "Textura.h"

//main
int main(int argc, char **argv)
{
	
	lecturaArchivo(mallaFacial, &totalNodos);

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
