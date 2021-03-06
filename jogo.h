#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "camera.h"
#include "math.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define FPS 60

int randompoint(){
        time_t t;
        srand((unsigned) time(&t));
        return rand()%480+1;
}

int randini(){
        time_t t;
        srand((unsigned) time(&t));
        return rand()%200+1;
}

float aleatorio(float *y){
    float random;
    *y = randompoint();
    random = *y - 450;
    if(random < 0){
        random = random * -1;
    }
    //printf("ramdom%f  y%f\n", random, *y);
    return random;
}

void pver(unsigned char ***matriz, int altura, int largura, int *rx, int *ry){
    int baixo, direita, esquerda, cima;
    int r, g, b;
    int achoux, achouy;
    *ry = 0;
    for(int x = 0; x < altura ; x++){
        for(int y = 0; y < largura ; y++){
            r = matriz[x][y][0];
            g = matriz[x][y][1];
            b = matriz[x][y][2];
            if(r == 255 && b == 0 && g == 0){
                if(x + 5 < altura && x - 5 > 0){
                    cima = matriz[(x + 5)][y][0];
                    baixo = matriz[(x - 5)][y][0];
                }
                if(y + 5 < largura && y - 5 > 0){
                    direita = matriz[x][(y + 5)][0];
                    esquerda = matriz[x][(y - 5)][0];
                }
                if(cima == 255 && baixo == 255 && direita == 255 && esquerda == 255 && y > *ry){
                    *rx = x;
                    *ry = y;
                }
            }
            cima = 0;
            baixo = 0;
            direita = 0;
            esquerda = 0;
        }
    }
    return;
}

void pazul(unsigned char ***matriz, int altura, int largura, int *bx, int *by, int *ry){
    int baixo, direita, esquerda, cima;
    int r, g, b;
    int achoux, achouy;
    for(int x = 0; x < altura ; x++){
        for(int y = 0; y < largura ; y++){
            r = matriz[x][y][0];
            g = matriz[x][y][1];
            b = matriz[x][y][2];
            if(r == 0 && b == 255 && g == 0){
                if(x + 8 < altura && x - 8 > 0){
                    cima = matriz[(x + 8)][y][2];
                    baixo = matriz[(x - 8)][y][2];
                }
                if(y + 8 < largura && y - 8 > 0){
                    direita = matriz[x][(y + 8)][2];
                    esquerda = matriz[x][(y - 8)][2];
                }
                if(cima == 255 && baixo == 255 && direita == 255 && esquerda == 255 && *ry - 50 > y){
                    *bx = x;
                    *by = y;
                }
            }
            cima = 0;
            baixo = 0;
            direita = 0;
            esquerda = 0;
        }
    }
    return;
}

int pega(int rx, int ry, int bx, int by, int *srx, int *sry, int *sbx, int *sby){
    //printf("%d %d\n", rx, *srx);
    int x = 0;
    if(rx < *srx - 80 && rx != *srx){
        //printf("pulou: %d %d\n", rx, *srx);
        x = 1;
    }

    *srx = rx;
    *sry = ry;
    *sbx = bx;
    *sby = by;
    return x;
}

int ang (int rx, int ry, int bx, int by){
    int xa = 0, xb = 0, ya = 0 , yb = 0;
    int dx, dy;
    float d;
    float seno;
    int angulo = 0;

    if(ry < by){
        xa = ry;
        xb = by;
    }
    else{
        xb = ry;
        xa = by;
    }
    if(rx < bx){
        ya = rx;
        yb = bx;
    }
    else{
        yb = rx;
        ya = bx;
    }

    if(xa == 0 || xb == 0 || ya == 0 || yb == 0)
        return 0;

    //printf("xa = %d, xb = %d, ya = %d, yb = %d\n", xa, xb, ya, yb);
    dx = xb - xa;
    dy = yb - ya;
    d = (dx*dx) + (dy*dy);
    //printf("d %f\n", d);
    d = sqrt(d);
    //printf("raiz %f\n", d);
    //printf("dx %d dy %d\n", dx, dy);
    seno = dy/d;

    //printf("%f\n", seno);
    if (seno > 0.34 && seno < 1 && bx < rx){
        //printf("45\n");
        angulo = 1;
    }
    if (seno > 0.34 && seno < 1 && bx > rx){
        //printf("- 45\n");
        angulo = -1;
    }

    return angulo;

}

void testecam(){
    int rx, ry, bx, by;
    int srx, sry, sbx, sby; //guarda valor antigo
    int testex, testey, contador = 0, acao = 0;
    camera *cam = camera_inicializa(0);
    int f;
    int pulou = 0, angulo = 0;
    int pause = 0;

   
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

    al_register_event_source(queue, al_get_mouse_event_source());

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
        
            angulo = 0;
            pulou = 0;
            contador++;
            camera_atualiza(cam);  
            for(int x = 0; x < altura ; x++){
           
                for(int y = 0; y < largura ; y++){
            
                
                    int r = cam->quadro[x][y][0];
                    int g = cam->quadro[x][y][1];
                    int b = cam->quadro[x][y][2];
                    if(r > 200 && r>g+100 && r>b+100){
                                    matriz[x][y][0] = 255;
                                    matriz[x][y][1] = 0;  
                                    matriz[x][y][2] = 0;
                    }  
                    
                    else{
                        
                        if(b >120 && b > g+70 && b > r+70){
                            matriz[x][y][0] = 0;
                            matriz[x][y][1] = 0;  
                            matriz[x][y][2] = 255;

    
                        }
                        else{
                            matriz[x][y][0] = cam->quadro[x][y][0];
                            matriz[x][y][1] = cam->quadro[x][y][1];
                            matriz[x][y][2] = cam->quadro[x][y][2];
                        }
                    }
                    
                }

            }
            pver(matriz,altura,largura,&rx,&ry);
            pazul(matriz,altura,largura,&bx,&by,&ry);

            if(rx < 10 && ry < 10){
                pause++;
                printf("%d\n", pause);
                if(pause == 60){
                    continuar = 0;
                    //printf("Pausou\n");
                    while(1){
                        al_flip_display();
                        camera_atualiza(cam);
                        for(int x = 0; x < altura ; x++){
               
                            for(int y = 0; y < largura ; y++){
                                
                
                
                                int r = cam->quadro[x][y][0];
                                int g = cam->quadro[x][y][1];
                                int b = cam->quadro[x][y][2];
                                if(r > 240 && r>g+150 && r>b+150){
                                    matriz[x][y][0] = 255;
                                    matriz[x][y][1] = 0;  
                                    matriz[x][y][2] = 0;
                                }            
                      
                           
                                else{
                                    matriz[x][y][0] = cam->quadro[x][y][0];
                                    matriz[x][y][1] = cam->quadro[x][y][1];
                                    matriz[x][y][2] = cam->quadro[x][y][2];
                                }
                            }
                    
                        }
                        pver(matriz,altura,largura,&rx,&ry);
                        //printf("passou1\n");
                        
                        if(rx > 10){
                            pause = 0;
                            break;
                            //printf("passou2\n");
                        }
                    }
                }

            }
            if(rx > 10 && pause > 30)
                pause = 0;
            //printf("vermelho x = %d y = %d / azul x = %d y = %d\n", rx, ry, bx, by);
            if(contador == 5){
                pulou = pega(rx, ry, bx, by, &srx, &sry, &sbx, &sby);
                contador = 0;
                angulo = ang(rx, ry, bx, by);
            }
            
            
            for(;rx < altura; rx++){
                for(;ry < largura; ry++){
                matriz[rx][ry][0] = 0;
                matriz[rx][ry][1] = 0;  
                matriz[rx][ry][2] = 255;
              }
            }
            for(;bx < altura; bx++){
                for(;by < largura; by++){
                matriz[bx][by][0] = 255;
                matriz[bx][by][1] = 0;  
                matriz[bx][by][2] = 0;
              }
            }


            rx = 0;
            ry = 0;
            bx = 0;
            by = 0;
            

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
    al_unregister_event_source(queue, al_get_timer_event_source(timer));
    al_destroy_timer(timer);
    al_unregister_event_source(queue, al_get_display_event_source(display));
    al_destroy_event_queue(queue);
    //al_shutdown_primitives_addon();
    //al_uninstall_system();

    camera_finaliza(cam);

    return;
}




void detectacam(ALLEGRO_DISPLAY *janela){
    ALLEGRO_BITMAP *map = NULL, *tiro = NULL, *tirobaixo = NULL, *tirocima = NULL, *pula = NULL, *pula2 = NULL;
    ALLEGRO_BITMAP *fogo = NULL, *fogo2  = NULL, *ini = NULL, *fogo3 = NULL, *ipause = NULL, *inicia = NULL;
    ALLEGRO_FONT *fonte = NULL;
    ALLEGRO_BITMAP *v1 = NULL, *v2 = NULL, *v3 = NULL, *v4 = NULL, *v5 = NULL;
    int rx, ry, bx, by;
    int srx, sry, sbx, sby; //guarda valor antigo
    int testex, testey, contador = 0, acao = 0, contadorrandom = 0;
    int contadorrandom2 = 0;
    float contador2 = 0;
	camera *cam = camera_inicializa(0);
    int f;
    int pulou = 0, angulo = 0;
    int flag = 0;
    int randx = 0, var;
    int randx2 = 0, var2;
    float mult, randy, dif;
    float mult2, randy2, dif2;
    int life = 0, pause = 0;
    int enex1 = 0, enex2 = 0, enex3 = 0, eney1 = 300, eney2 = 0, eney3 = 0;
    int tpx = 0, tpy = 0, flagtp = 0;
    int score = 0, lifetorre = 0;

    fonte = al_load_font("fonte/arial_narrow_7.ttf", 30, 0);

    map = al_load_bitmap("imagens/mapadefender.png");
    tiro = al_load_bitmap("imagens/tiro.png");
    tirobaixo = al_load_bitmap("imagens/tirobaixo.png");
    tirocima = al_load_bitmap("imagens/tirocima.png");
    pula = al_load_bitmap("imagens/pula1.png");
    pula2 = al_load_bitmap("imagens/pula2.png");
    fogo = al_load_bitmap("imagens/tiros.png");
    fogo3 = al_load_bitmap("imagens/tiros2.png");
    fogo2 = al_load_bitmap("imagens/tiros.png");
    ini = al_load_bitmap("imagens/inimigo.png");
    ipause = al_load_bitmap("imagens/pause.png");
    inicia = al_load_bitmap("imagens/inicio.png");
    v1 = al_load_bitmap("imagens/1v.png");
    v2 = al_load_bitmap("imagens/2v.png");
    v3 = al_load_bitmap("imagens/3v.png");
    v4 = al_load_bitmap("imagens/4v.png");
    v5 = al_load_bitmap("imagens/5v.png");


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

  	/*ALLEGRO_DISPLAY *display = al_create_display(2 * largura, altura);
  	if(!display)
    	printf("nao foi possivel criar janela");

  	al_register_event_source(queue, al_get_display_event_source(display));*/

  	ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
  	if(!timer)
    	printf("nao foi possivel criar relogio");

	al_register_event_source(queue, al_get_timer_event_source(timer));

    //al_register_event_source(queue, al_get_mouse_event_source());

	unsigned char ***matriz = camera_aloca_matriz(cam);

	ALLEGRO_COLOR cor = al_map_rgb_f(0, 0, 1);

	//ALLEGRO_BITMAP *buffer = al_get_backbuffer(display);

	//ALLEGRO_BITMAP *esquerda = al_create_sub_bitmap(buffer, 0, 0, largura, altura);

	//ALLEGRO_BITMAP *direita = al_create_sub_bitmap(buffer, largura, 0, largura, altura);

    al_start_timer(timer);
    int atualizar = 1, continuar = 1;
    printf("%d %d\n", altura, largura);
    al_draw_bitmap(inicia, 0, 0, 0);
    al_flip_display();
    al_rest(10);
    al_draw_bitmap(map, 0, 0, 0);
    al_draw_bitmap(tiro, 70, 0, 0);


    

    while(continuar == 1){
    
        ALLEGRO_EVENT event;
        //al_wait_for_event(queue, &event);
        if(atualizar == 1){  
            if(contadorrandom >= 0){
                contadorrandom++;
            }

            

            
            contadorrandom2++;
            

            

            if(flag == 1){
                contador2--;
            }
            if(contador2 < 0 && flag == 0){
                contador2++;
                flag = 0;
            }
            al_flip_display();
     
            //angulo = 0;
            pulou = 0;
            contador++;
            camera_atualiza(cam);  
            for(int x = 0; x < altura ; x++){
           
                for(int y = 0; y < largura ; y++){
            
                
                    int r = cam->quadro[x][y][0];
                    int g = cam->quadro[x][y][1];
                    int b = cam->quadro[x][y][2];
                    if(r > 200 && r>g+100 && r>b+100){
                                    matriz[x][y][0] = 255;
                                    matriz[x][y][1] = 0;  
                                    matriz[x][y][2] = 0;
                    } 
                    
                    else{
                        
                        if(b >140 && b > g+50 && b > r+50 ){
                            matriz[x][y][0] = 0;
                            matriz[x][y][1] = 0;  
                            matriz[x][y][2] = 255;

    
                        }
                        else{
                            matriz[x][y][0] = cam->quadro[x][y][0];
                            matriz[x][y][1] = cam->quadro[x][y][1];
                            matriz[x][y][2] = cam->quadro[x][y][2];
                        }
                    }
                    
                }

            }
            pver(matriz,altura,largura,&rx,&ry);
            pazul(matriz,altura,largura,&bx,&by,&ry);

            if(rx < 10 && ry < 10){
                pause++;
                //printf("%d\n", pause);
                if(pause == 60){
                    printf("Pausou\n");
                    while(1){
                        al_draw_bitmap(ipause, 0, 0, 0);
                        al_flip_display();
                        camera_atualiza(cam);
                        for(int x = 0; x < altura ; x++){
               
                            for(int y = 0; y < largura ; y++){
                                
                
                
                                int r = cam->quadro[x][y][0];
                                int g = cam->quadro[x][y][1];
                                int b = cam->quadro[x][y][2];
                                if(r > 180 && r>g+80 && r>b+80){
                                    matriz[x][y][0] = 255;
                                    matriz[x][y][1] = 0;  
                                    matriz[x][y][2] = 0;
                                }            
                      
                           
                                else{
                                    matriz[x][y][0] = cam->quadro[x][y][0];
                                    matriz[x][y][1] = cam->quadro[x][y][1];
                                    matriz[x][y][2] = cam->quadro[x][y][2];
                                }
                            }
                    
                        }
                        pause++;
                        pver(matriz,altura,largura,&rx,&ry);
                        if(pause > 300){
                            continuar = 0;
                            break;
                        }
                        if(rx > 10){
                            pause = 0;
                            break;
                        }
                        //printf("%d\n", pause);
                    }
                }

            }

            //printf("vermelho x = %d y = %d / azul x = %d y = %d\n", rx, ry, bx, by);
            if(contador == 5){
                pulou = pega(rx, ry, bx, by, &srx, &sry, &sbx, &sby);
                contador = 0;
                if(pulou == 0)
                    angulo = ang(rx, ry, bx, by);
            }
            if(pulou == 1 && contador2 == 0){
                al_draw_bitmap(map, 0, 0, 0);
                al_draw_bitmap(pula, 0, 0, 0);
                al_draw_bitmap(ini, enex1, eney1, 0);
                al_draw_bitmap(fogo3, tpx, tpy, 0);
                contador2 = -1;
                flag = 1;
                al_flip_display();
                //printf("entrou\n");
            }
            if(contador2 <= -1 && contador2 > -12){
                al_draw_bitmap(map, 0, 0, 0);
                al_draw_bitmap(pula, 0, (contador2 * 5), 0);
                al_draw_bitmap(ini, enex1, eney1, 0);
                al_draw_bitmap(fogo3, tpx, tpy, 0);
                //printf("entrou 2\n");
                al_flip_display();
                if(contador2 == -11){
                    flag = 0;
                }
            }

            ////////////////////////

            if(contadorrandom == 1){
                dif = aleatorio(&randy);
                mult = dif/130;
                randx = 0;
            }

            randy = randy + mult;
            randx = randx + 5;

            var = contador2 * 10;
            //printf("var %d\n", var);
            if(randx > 410 && randy  < 425 + var && randy > 360 + var && randx < 440){
                life++;
                contadorrandom = 0;
                //printf("%d %f %d\n", randx, randy, var);
            }
            if(randx > 800 || randy > 600){
                contadorrandom = 0;
            }
            if(life == 5){
                continuar = 0;
            }
            ////////////////////////  


            if(contadorrandom2 == 1){
                dif2 = aleatorio(&randy2);
                mult2 = dif2/130;
                randx2 = 0;
            }

            randy2 = randy2 + mult2;
            randx2 = randx2 + 5;

            var2 = contador2 * 10;
            //printf("var %d\n", var);
            if(randx2 > 410 && randy2  < 430 + var2 && randy2 > 360 + var2 && randx2 < 450){
                life++;
                contadorrandom2 = -30;
                randx2 = 2000;
                randy2 = 2000;
                //printf("%d %f %d\n", randx2, randy2, var2);
            }
            if(randx2 > 800 || randy2 > 600){
                contadorrandom2 = -50;
                randx2 = -2000; 
                randy2 = -2000;
            }
            if(life == 5){
                continuar = 0;
            }    
            //////////////////////////////////



            al_draw_bitmap(fogo, randx, randy, 0);
            al_draw_bitmap(fogo2, randx2, randy2, 0);
            al_draw_bitmap(fogo3, tpx, tpy, 0);
            al_draw_bitmap(ini, enex1, eney1, 0);
            al_draw_textf(fonte, al_map_rgb(255, 255, 255), 10, 10, 0, "Pontos: %d", score);
            if(life == 4)
                al_draw_bitmap(v1, 0, 0, 0);
            if(life == 3)
                al_draw_bitmap(v2, 0, 0, 0);
            if(life == 2)
                al_draw_bitmap(v3, 0, 0, 0);
            if(life == 1)
                al_draw_bitmap(v4, 0, 0, 0);
            if(life == 0)
                al_draw_bitmap(v5, 0, 0, 0);


            if(enex1 == -300){
                eney1 =  randini();
                eney1 = eney1 + 200;
            }
            enex1 =  enex1 + 10;

            if(enex1 > 500){
                lifetorre++;
                enex1 = -300;
            }
            
       
           

            //printf("%d %f, %f\n", randx, randy, mult);

            
            //printf("%f\n", contador2);

            if(pulou == 0 && contador2 == 0){
                al_draw_bitmap(map, 0, 0, 0);
                if(angulo == 0)
                    al_draw_bitmap(tiro, 70, 0, 0);
                if(angulo == 1)
                    al_draw_bitmap(tirocima, 0, 0, 0);
                if(angulo == -1)
                    al_draw_bitmap(tirobaixo, 0, 0, 0);
                al_draw_bitmap(fogo, randx, randy, 0);
                al_draw_bitmap(fogo2, randx2, randy2, 0);
                al_draw_bitmap(fogo3, tpx, tpy, 0);
                al_draw_bitmap(ini, enex1, eney1, 0);
                al_draw_textf(fonte, al_map_rgb(255, 255, 255), 10, 10, 0, "Pontos: %d", score);
                if(life == 4)
                    al_draw_bitmap(v1, 0, 0, 0);
                if(life == 3)
                    al_draw_bitmap(v2, 0, 0, 0);
                if(life == 2)
                    al_draw_bitmap(v3, 0, 0, 0);
                if(life == 1)
                    al_draw_bitmap(v4, 0, 0, 0);
                if(life == 0)
                    al_draw_bitmap(v5, 0, 0, 0);
                
            }


            /////////////// tiro personagem ///////////////////

            if(angulo == 0 && flagtp == 0){
                tpx = 380 + contador2;
                tpy = 375;
                flagtp = 1;
            }

            if(angulo == 1 && flagtp == 0){
                tpx = 380 + contador2;
                tpy = 330;
                flagtp = 1;
            }

            if(angulo == -1 && flagtp == 0){
                tpx = 380 + contador2;
                tpy = 400;
                flagtp = 1;   
            }

            if(tpy < 375){
                tpy = tpy - 20;
            }

            if(tpy > 375){
                tpy = tpy + 20;
            }

            tpx = tpx - 20;

            if(tpx < -200 || tpy < 0 || tpy > 600){
                flagtp = 0;
            }

            if(tpx >= randx && tpx <= randx + 30 && tpy >= randy && tpy <= randy + 30){
                contadorrandom = 0;
            }

            if(tpx >= randx2 && tpx <= randx2 + 30 && tpy >= randy2 && tpy <= randy2 + 30){
                contadorrandom2 = 0;
            }

            if(tpx >= enex1 && tpx <= (enex1 + 150) && tpy >= eney1 && tpy <= (eney1 + 100) && enex1 > -50 && eney1 > 0){
                enex1 = -300;
                score++;
                //printf("%d\n", score);
            }

            if(lifetorre == 10){
                continuar = 0;
            }



            
            
            for(;rx < altura; rx++){
                for(;ry < largura; ry++){
                matriz[rx][ry][0] = 0;
                matriz[rx][ry][1] = 0;  
                matriz[rx][ry][2] = 255;
              }
            }
            for(;bx < altura; bx++){
                for(;by < largura; by++){
                matriz[bx][by][0] = 255;
                matriz[bx][by][1] = 0;  
                matriz[bx][by][2] = 0;
              }
            }


            rx = 0;
            ry = 0;
            bx = 0;
            by = 0;

            

            //camera_copia(cam, cam->quadro, esquerda);
            atualizar = 0;
            //camera_copia(cam,matriz,direita);ma
        }
  
        al_wait_for_event(queue, &event);
        switch(event.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            continuar = 0;
            break;
        case ALLEGRO_EVENT_TIMER:
            atualizar = 1;
            break;

        //al_flip_display();


        }

   

    }
    al_stop_timer(timer);
    al_destroy_bitmap(map);
    al_destroy_bitmap(tiro);
    al_destroy_bitmap(tirobaixo);
    al_destroy_bitmap(tirocima);
    al_destroy_bitmap(pula);
    al_destroy_bitmap(pula2);
    al_destroy_bitmap(fogo);
    al_destroy_bitmap(fogo2);
    al_destroy_bitmap(ini);
    al_destroy_bitmap(ipause);
    al_destroy_bitmap(v1);
    al_destroy_bitmap(v2);
    al_destroy_bitmap(v3);
    al_destroy_bitmap(v4);
    al_destroy_bitmap(v5);
    //al_destroy_bitmap(direita);

    //al_destroy_bitmap(esquerda);

    camera_libera_matriz(cam, matriz);
    al_unregister_event_source(queue, al_get_timer_event_source(timer));
    al_destroy_timer(timer);
    //al_unregister_event_source(queue, al_get_display_event_source(display));
    al_destroy_event_queue(queue);
    //al_shutdown_primitives_addon();
    //al_uninstall_system();

    camera_finaliza(cam);

	return;
}