CC = gcc
CFLAGS = -Wall
LIBS = -lfreeglut -lopengl32 -lglu32 -lm
TARGET = CaraZombi.exe

SRCS = main.c Grafo.c Rostro.c Textura.c Iluminacion.c Render.c

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)