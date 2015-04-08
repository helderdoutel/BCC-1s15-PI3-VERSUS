#include "camera.h"


void camera_converte(camera *cam, IplImage *image) {
  char *row = image->imageData;

  for(int y = 0; y < cam->altura; y++) {
    char *pixel = row;

    for(int x = cam->largura - 1; x >= 0; x--) {
      cam->quadro[y][x][2] = *pixel;
      pixel++;

      cam->quadro[y][x][1] = *pixel;
      pixel++;

      cam->quadro[y][x][0] = *pixel;
      pixel++;
    }

    row += image->widthStep;
  }
}


camera *camera_inicializa(int i) {
  camera *cam = NULL;

  CvCapture *capture = cvCaptureFromCAM(i);

  if(capture) {
    IplImage *image = cvQueryFrame(capture);

    if(image) {
      cam = malloc(sizeof(camera));

      cam->capture = capture;
      cam->altura = image->height;
      cam->largura = image->width;
      cam->quadro = camera_aloca_matriz(cam);

      camera_converte(cam, image);
    }
    else
      cvReleaseCapture(&capture);
  }

  return cam;
}


void camera_finaliza(camera *cam) {
  camera_libera_matriz(cam, cam->quadro);

  cvReleaseCapture(&cam->capture);

  free(cam);
}


void camera_atualiza(camera *cam) {
  IplImage *image = cvQueryFrame(cam->capture);

  if(image)
    camera_converte(cam, image);
}


void camera_copia(camera *cam, unsigned char ***matriz, ALLEGRO_BITMAP *bitmap) {
  ALLEGRO_LOCKED_REGION *region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

  char *row = region->data;

  for(int y = 0; y < cam->altura; y++) {
    char *pixel = row;

    for(int x = 0; x < cam->largura; x++) {
      *pixel = matriz[y][x][2];
      pixel++;

      *pixel = matriz[y][x][1];
      pixel++;

      *pixel = matriz[y][x][0];
      pixel += 2;
    }

    row += region->pitch;
  }

  al_unlock_bitmap(bitmap);
}


unsigned char ***camera_aloca_matriz(camera *cam) {
  unsigned char ***matriz = malloc(cam->altura * sizeof(unsigned char **));

  for(int y = 0; y < cam->altura; y++) {
    matriz[y] = malloc(cam->largura * sizeof(unsigned char *));

    for(int x = 0; x < cam->largura; x++)
      matriz[y][x] = malloc(3 * sizeof(unsigned char));
  }

  return matriz;
}


void camera_libera_matriz(camera *cam, unsigned char ***matriz) {
  for(int y = 0; y < cam->altura; y++) {
    for(int x = 0; x < cam->largura; x++)
      free(matriz[y][x]);

    free(matriz[y]);
  }

  free(matriz);
}
