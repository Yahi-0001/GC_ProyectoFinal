#ifndef ILUMINACION_H
#define ILUMINACION_H

#include <GL/freeglut.h>

//cargar iluminación
extern const GLfloat light_ambiente[4];
extern const GLfloat light_diffuse[4];
extern const GLfloat light_specular[4];
extern const GLfloat light_position[4];

//propiedades del material
extern const GLfloat mat_ambiente[4];
extern const GLfloat mat_diffuse[4];
extern const GLfloat mat_specular[4];
extern const GLfloat mat_shininess[1];

void init(void);

#endif