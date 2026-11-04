#include "render.h"
#include "textura.h" // Para usar texture_id
#include <math.h>
#include <stdlib.h>  // Para la función exit()

// variables de rotación
float rot_x = 0.0;
float rot_y = 0.0;
float rot_z = 0.0;

// variables de traslación
float pos_x = 0.0f;
float pos_y = 0.0f;
float pos_z = 0.0f;

//variables de escalado
float scale_x = 1.0f;
float scale_y = 1.0f;
float scale_z = 1.0f;

//Mostrar mallado y numeros
int mostrar_malla = 0;
int mostrar_numeros = 0;

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
		case '5': 
			mostrar_malla = !mostrar_malla;
			break;
		case '6': 
			mostrar_numeros = !mostrar_numeros;
			break;

		case 'a':
		case 'A':
			rot_z += 5.0f;
			break;

		case 'b':
		case 'B':
			rot_z -= 5.0f;
			break;

		//teclas de traslación
		case 'c':
		case 'C':
			pos_x += 0.05f;
			break;

		case 'd':
		case 'D':
			pos_x -= 0.05f;
			break;

		case 'e':
		case 'E':
			pos_y += 0.05f;
			break;

		case 'f':
		case 'F':
			pos_y -= 0.05f;
			break;

		case 'g':
		case 'G':
			pos_z += 0.05f;
			break;

		case 'h':
		case 'H':
			pos_z -= 0.05f;
			break;

		//teclas de escalado

		case 'i':
		case 'I':    
			scale_x += 0.1f; 
			scale_y += 0.1f; 
			scale_z += 0.1f; 
			break;

		case 'j': 
		case 'J': 
			if(scale_x > 0.1f)
			{
				scale_x -= 0.1f; 
				scale_y -= 0.1f; 
				scale_z -= 0.1f; 
			}
			break;

		//restauración de estados
		case 'k':
		case 'K':

			pos_x = 0.0f; 
			pos_y = 0.0f; 
      pos_z = 0.0f;

      rot_x = 0.0f; 
      rot_y = 0.0f; 
      rot_z = 0.0f;

      scale_x = 1.0f; 
      scale_y = 1.0f; 
      scale_z = 1.0f;
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
		// teclas de rotación
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
	}
	glutPostRedisplay(); 
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


  GLfloat luz_ambiente[] = { 0.30f, 0.30f, 0.30f, 1.0f };
  GLfloat luz_difusa[] = { 0.85f, 0.85f, 0.85f, 1.0f };
  GLfloat posicion_luz[] = { 0.0f, 3.0f, 8.0f, 0.0f };

  // brillo para hacer la linea del tabique
  GLfloat luz_especular[] = { 0.3f, 0.4f, 0.3f, 1.0f };
  GLfloat brillo_material[] = { 60.0f };

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

	glTranslatef(pos_x, pos_y, pos_z);
	glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
	glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
	glRotatef(rot_z, 0.0f, 0.0f, 1.0f);
	glScalef(scale_x, scale_y, scale_z);

	GLfloat light_position[] = { 1.0f, 1.0f, 3.0f, 1.0f }; 
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//activamos la textura
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	//dibujar el fondo
	glColor3f(1.0f, 1.0f, 1.0f);
	
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
		else if (i >= 14 && i <= 25) 
		{
			// labios
			u1 = u2 = u3 = 0.5f;
			v1 = v2 = v3 = 0.25f;
		}
		else if (i >= 72 && i <= 80) 
		{
			// Colmillos / Dientes (Franja color hueso)
			u1 = u2 = u3 = 0.5f;
			v1 = v2 = v3 = 0.05f; 
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
		else if (i >= 14 && i <= 25)
		{
			u1 = u2 = u3 = 0.5f;
			v1 = v2 = v3 = 0.25f;
		}
		else if (i >= 72 && i <= 80) 
		{
			// Colmillos / Dientes (Franja color hueso)
			u1 = u2 = u3 = 0.5f;
			v1 = v2 = v3 = 0.05f; 
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

	if(mostrar_malla == 1)
	{
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

		glEnable(GL_POINT_SMOOTH);// es para que se vean los circulos
		glEnable(GL_BLEND);// para los puntos
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // artefactos visuales para los puntos 
		glColor3f(1.0f, 0.0f, 0.0f);
		glPointSize(8.0f);

		//dibujar la mitad original
		glBegin(GL_POINTS);
		for(int i = 0; i < totalNodos; i++)
		{
			float tx = 0.0f, ty = 0.0f, tz = 0.0f;
			obtenerTraslacion(mallaFacial[i].key, &tx, &ty, &tz);

			asignarColorPunto(mallaFacial[i].key, 0);
			glVertex3f(mallaFacial[i].coordenada.x + tx, mallaFacial[i].coordenada.y + ty, mallaFacial[i].coordenada.z + tz);
		}
		glEnd();

		glPushMatrix();
		glScalef(-1.0f, 1.0f, 1.0f);

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

	}

	if(mostrar_numeros == 1)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glColor3f(0.0f, 0.0f, 1.0f); // Color de los números

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
		}
	}
	
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
  

	// ojo original
	dibujarOjo(0.26f, 0.21f, 0.02f, 0.80f); // coordenada

	// Ojo izquierdo
	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f);

	dibujarOjo(0.26f, 0.21f, 0.02f, 0.80f);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	//dibujar colmillos


	/*
	glColor3f(0.02f, 0.02f, 0.02f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.20f, -0.24f, 0.005f); // Arriba Izquierda
		glVertex3f( 0.20f, -0.24f, 0.005f); // Arriba Derecha
		glVertex3f( 0.14f, -0.45f, 0.005f); // Abajo Derecha
		glVertex3f(-0.14f, -0.45f, 0.005f); // Abajo Izquierda
	glEnd();

	glColor3f(0.92f, 0.88f, 0.60f); 
	
	glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, 0.0f, 1.0f);

		float x_inicio = -0.18f;
		float x_fin = 0.18f;
		float ancho = 0.025f; 
		float z_dientes = 0.008f;

		
		for (float x = x_inicio; x < x_fin; x += ancho) 
		{
			glVertex3f(x, -0.25f, z_dientes); 
			glVertex3f(x + ancho, -0.25f, z_dientes); 
			glVertex3f(x + (ancho / 2.0f), -0.40f, z_dientes);
		}

		for (float x = x_inicio + (ancho / 2.0f); x < x_fin - (ancho / 2.0f); x += ancho) 
		{
			glVertex3f(x, -0.43f, z_dientes);
			glVertex3f(x + ancho, -0.43f, z_dientes);
			glVertex3f(x + (ancho / 2.0f), -0.32f, z_dientes);
		}
	glEnd();*/

	glutSwapBuffers(); // mostramos el lienzo
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

void dibujarCuencaOjoZombi(float cx, float cy, float cz, float rx, float ry, int num_segmentos) 
{
	
	glBegin(GL_TRIANGLE_FAN);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(cx, cy, cz);
		for(int i = 0; i <= num_segmentos; i++) 
		{
			float theta = 2.0f * 3.1415926f * (float)i / (float)num_segmentos;
			float x = (rx * 0.55f) * cosf(theta);
			float y = (ry * 0.55f) * sinf(theta);
			glVertex3f(x + cx, y + cy, cz);
		}
	glEnd();

	glBegin(GL_QUAD_STRIP);
		for(int i = 0; i <= num_segmentos; i++) 
		{
			float theta = 2.0f * 3.1415926f * (float)i / (float)num_segmentos;
			float cos_t = cosf(theta);
			float sin_t = sinf(theta);

			// Vértice interior (Negro puro e intenso)
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			glVertex3f((rx * 0.55f * cos_t) + cx, (ry * 0.55f * sin_t) + cy, cz);

			// Vértice exterior (Totalmente transparente para fundirse con el verde)
			glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
			glVertex3f((rx * cos_t) + cx, (ry * sin_t) + cy, cz);
		}
	glEnd();
}



// funcion par adibujar los 3 circulos de los ojos
void dibujarOjo(float cx, float cy, float cz, float escala) 
{
	glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	dibujarCuencaOjoZombi(cx, cy, cz - 0.002f, 0.26f * escala, 0.16f * escala, 36);

  // iris
 	glColor3f(0.95f, 0.80f, 0.05f); 
	dibujarCirculo(cx, cy, cz, 0.06f * escala, 30);

  // pupila
  glColor3f(0.45f, 0.00f, 0.10f); 
	dibujarCirculo(cx, cy, cz + 0.001f, 0.03f * escala, 30);

  // brillito
  glColor3f(1.0f, 1.0f, 1.0f); // Color blanco puro
  // Desplazamos el centro un poco hacia arriba y a la derecha para dar el efecto de reflejo
  dibujarCirculo(cx, cy, cz + 0.002f, 0.007f * escala, 20);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}


/*

CAPA 6: PANTALLA DE FRUSTRUM
≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼ ≽^•⩊•^≼

*/
void vistaFrustum(int const win_width, int const win_height)
{
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
