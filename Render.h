#ifndef RENDER_H
#define RENDER_H

#include <GL/freeglut.h>
#include "rostro.h"

extern double rot_x;
extern double rot_y;
extern float pos_x;

void asignarColorPunto(int key, int reflejo);
void display(void);
void dibujarCirculo(float cx, float cy, float cz, float radio, int num_segmentos);
void dibujarCuencaOjoZombi(float cx, float cy, float cz, float rx, float ry, int num_segmentos);
void dibujarOjo(float cx, float cy, float cz, float escala);
void vistaFrustum(int const win_width, int const win_height);
void redimensionar(int w, int h);
void teclado(unsigned char tecla, int x, int y);
void teclasEspeciales(int tecla, int x, int y);

#endif