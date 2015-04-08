#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "camera.h"

int main(){
	camera *cam = camera_inicializa(0);
	if(!cam)
    	erro("nao foi possivel inicializar camera");

	  int largura = cam->largura;
	  int altura = cam->altura;

  	if(!al_init())
	    erro("nao foi possivel inicializar allegro");

  	if(!al_init_primitives_addon())
  	  erro("nao foi possivel inicializar adicional de primitivas");

  	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  	if(!queue)
    	erro("nao foi possivel criar fila de eventos");

  	ALLEGRO_DISPLAY *display = al_create_display(2 * largura, altura);
  	if(!display)
    	erro("nao foi possivel criar janela");

  	al_register_event_source(queue, al_get_display_event_source(display));

  	ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
  	if(!timer)
    	erro("nao foi possivel criar relogio");

	al_register_event_source(queue, al_get_timer_event_source(timer));

	unsigned char ***matriz = camera_aloca_matriz(cam);

	ALLEGRO_COLOR cor = al_map_rgb_f(0, 0, 1);

	ALLEGRO_BITMAP *buffer = al_get_backbuffer(display);

	ALLEGRO_BITMAP *esquerda = al_create_sub_bitmap(buffer, 0, 0, largura, altura);

	ALLEGRO_BITMAP *direita = al_create_sub_bitmap(buffer, largura, 0, largura, altura);

	return 0;
}