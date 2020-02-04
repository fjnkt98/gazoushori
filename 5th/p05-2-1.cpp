#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>

int min(int x, int y) {
  if (x > y){
    return y;
  }
  else{
    return x;
  }
}

int max(int x, int y) {
  if (x < y){
    return y;
  }
  else{
    return x;
  }
}

IplImage* img1;
IplImage* img2;

float a_global = 1;
int b_global = 0;
float gamma_global = 1;

// Trackbar callback functions
void aCallback(int a);
void bCallback(int b);
void gammaCallback(int gamma);

void main(int argc, char* argv[])
{
  // Variables for Trackbar
  int a = 40; // -3.0<=a_global<=3.0, step=0.1 -> 0<=a<=60
  int b = 255;  // -255<=b_global<=255 -> 0<=b<=510
  int gamma = 100;  // 0.0<=gamma_global<=2.0, step=0.01 -> 0<=gamma<=200

  // Command line argument estimation
  printf("argc = %d\n", argc);
  for (int i = 0; i < argc; i++){
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("\n\n");

  if (argc < 2){
    printf("Choose file.");
    return;
  }

  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL){
    printf("Failed to load image file.\n");
    return;
  }

  printf("nChannels = %d\n", img1->nChannels);
  if (img1->nChannels != 3){
    printf("Choose color image.");
    return;
  }

  // Create original image window
  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  // Create and copy image
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);
  cvCopy(img1, img2);

  cvNamedWindow("p05-2-1");

  // Create trackbar
  cvCreateTrackbar("a", "p05-2-1", &a, 60, aCallback);
  cvCreateTrackbar("b", "p05-2-1", &b, 510, bCallback);
  cvCreateTrackbar("gamma", "p05-2-1", &gamma, 200, gammaCallback);

  cvShowImage("p05-2-1", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}

void aCallback(int a){
  a_global = (float)a / 10.0 - 3;
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 0] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 0] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
      img2->imageData[img2->widthStep*y + x * 3 + 1] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 1] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
      img2->imageData[img2->widthStep*y + x * 3 + 2] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 2] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
    }
  }
  cvShowImage("p05-2-1", img2);
}

void bCallback(int b){
  b_global = b - 255;
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 0] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 0] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
      img2->imageData[img2->widthStep*y + x * 3 + 1] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 1] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
      img2->imageData[img2->widthStep*y + x * 3 + 2] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 2] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
    }
  }
  cvShowImage("p05-2-1", img2);
}

void gammaCallback(int gamma){
  gamma_global = (float)gamma / 100.0;
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 0] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 0] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
      img2->imageData[img2->widthStep*y + x * 3 + 1] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 1] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
      img2->imageData[img2->widthStep*y + x * 3 + 2] = max(min(255 * pow((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 2] * a_global + b_global) / 255), 1 / gamma_global), 255), 0);
    }
  }
  cvShowImage("p05-2-1", img2);
}
