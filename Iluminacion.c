#include "Iluminacion.h"
#include "Textura.h"

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