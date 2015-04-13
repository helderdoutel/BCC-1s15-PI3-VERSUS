#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "camera.h"

#define FPS 60

int main(){
	camera *cam = camera_inicializa(0);
	if(!cam)
        printf("nao foi possivel inicializar camera");

        int largura = cam->largura;
	    int altura = cam->altura;

  	if(!al_init())
        printf("nao foi possivel inicializar allegro");

  	if(!al_init_primitives_addon())
        printf("nao foi possivel inicializar adicional de primitivas");

  	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  	if(!queue)
        printf("nao foi possivel criar fila de eventos");

  	ALLEGRO_DISPLAY *display = al_create_display(2 * largura, altura);
  	if(!display)
    	printf("nao foi possivel criar janela");

  	al_register_event_source(queue, al_get_display_event_source(display));

  	ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
  	if(!timer)
    	printf("nao foi possivel criar relogio");

	al_register_event_source(queue, al_get_timer_event_source(timer));

	unsigned char ***matriz = camera_aloca_matriz(cam);

	ALLEGRO_COLOR cor = al_map_rgb_f(0, 0, 1);

	ALLEGRO_BITMAP *buffer = al_get_backbuffer(display);

	ALLEGRO_BITMAP *esquerda = al_create_sub_bitmap(buffer, 0, 0, largura, altura);

	ALLEGRO_BITMAP *direita = al_create_sub_bitmap(buffer, largura, 0, largura, altura);

    al_start_timer(timer);
    int atualizar = 1, continuar = 1;
    printf("%d %d\n", altura, largura);

    while(continuar == 1){
        ALLEGRO_EVENT event;

        al_wait_for_event(queue, &event);
        al_flip_display();
        if(atualizar == 1){
            camera_atualiza(cam);  
            for(int x = 0; x < altura ; x++){
           
                for(int y = 0; y < largura ; y++){
            
                
                    int r = cam->quadro[x][y][0];
                    int g = cam->quadro[x][y][1];
                    int b = cam->quadro[x][y][2];
                    if(r > 130 && g > 130 && b >130){
                        matriz[x][y][0] = 255;
                        matriz[x][y][1] = 0;  
                        matriz[x][y][2] = 0;
    
                    }
                    else{
                    
                        matriz[x][y][0] = cam->quadro[x][y][0];
                        matriz[x][y][1] = cam->quadro[x][y][1];
                        matriz[x][y][2] = cam->quadro[x][y][2];
                    }
                    if(x == 639)
                    printf("preencheu\n");
                }

            }

            camera_copia(cam, cam->quadro, esquerda);
            atualizar = 0;
            camera_copia(cam,matriz,direita);
        }
  
        al_wait_for_event(queue, &event);
        switch(event.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            continuar = 0;
            break;
        case ALLEGRO_EVENT_TIMER:
            atualizar = 1;
            break;
        al_flip_display();

        }

   

    }
    al_stop_timer(timer);
    al_destroy_bitmap(direita);

    al_destroy_bitmap(esquerda);

    camera_libera_matriz(cam, matriz);

	return 0;
}