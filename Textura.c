#include "Textura.h"

GLuint texture_id;

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
				imagen[i][j][0] = 30;
				imagen[i][j][1] = 38;
				imagen[i][j][2] = 32;
			}

			else if (i < TEX_HEIGHT * 0.18) 
			{
				imagen[i][j][0] = 245; // Rojo
				imagen[i][j][1] = 240; // Verde
				imagen[i][j][2] = 225; // Azul
			}

			else if (i >= TEX_HEIGHT * 0.18 && i <= TEX_HEIGHT * 0.4) 
			{
				imagen[i][j][0] = 25;
				imagen[i][j][1] = 22;
				imagen[i][j][2] = 25;
			}
			// Piel
			else 
			{
				imagen[i][j][0] = 90;
				imagen[i][j][1] = 102;
				imagen[i][j][2] = 88;
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
