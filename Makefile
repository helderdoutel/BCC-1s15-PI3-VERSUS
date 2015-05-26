TARGETS = jogo

CC = gcc

FLAGS = -std=c99

LIBS = -lm -lopencv_core -lopencv_highgui -lallegro -lallegro_main -lallegro_primitives -lallegro -lallegro_image -lallegro_acodec -lallegro_audio -lallegro_ttf -lallegro_font

all: $(TARGETS)

camera.o: camera.c camera.h
	$(CC) $(FLAGS) $< -c

jogo: main.c camera.o jogo.h
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f *~ *.o

destroy: clean
	rm -f $(TARGETS)
