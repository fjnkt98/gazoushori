#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>



IplImage* img1; // Original
IplImage* img2; // Color image
IplImage* img3; // Grayscale
IplImage* img4; // Pseudo image

float a_global = 1;
int b_global = 0;
float gamma_global = 1;
int colormap_global = 0;

// Trackbar callback functions
void aCallback(int a);
void bCallback(int b);
void gammaCallback(int gamma);
void colormapCallback(int colormap);
int min(int x, int y);
int max(int x, int y);
void bgr2gray(IplImage* gray, const IplImage* bgr);
void gray2pseudo(IplImage* pseudo, const IplImage* gray);

void main(int argc, char* argv[])
{
  // Variables for Trackbar
  int a = 40; // -3.0<=a_global<=3.0, step=0.1 -> 0<=a<=60
  int b = 255;  // -255<=b_global<=255 -> 0<=b<=510
  int gamma = 100;  // 0.0<=gamma_global<=2.0, step=0.01 -> 0<=gamma<=200
  int colormap = 0;

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

  img3 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 1);
  bgr2gray(img3, img1);

  img4 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);
  cvCopy(img1, img4);

  cvNamedWindow("p05-2-1");

  // Create trackbar
  cvCreateTrackbar("a", "p05-2-1", &a, 60, aCallback);
  cvCreateTrackbar("b", "p05-2-1", &b, 510, bCallback);
  cvCreateTrackbar("gamma", "p05-2-1", &gamma, 200, gammaCallback);
  cvCreateTrackbar("colormap", "p05-2-1", &colormap, 5, colormapCallback);

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

  switch (colormap_global)
  {
  case 0: // plane image
    cvShowImage("p05-2-1", img2);
    break;
  case 1: // grayscale
    bgr2gray(img3, img2);
    cvShowImage("p05-2-1", img3);
    break;
  case 2:
  case 3:
  case 4:
  case 5: // pseudo image
    bgr2gray(img3, img2);
    gray2pseudo(img4, img3);
    cvShowImage("p05-2-1", img4);
    break;
  default:
    break;
  }
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

  switch (colormap_global)
  {
  case 0: // plane image
    cvShowImage("p05-2-1", img2);
    break;
  case 1: // grayscale
    bgr2gray(img3, img2);
    cvShowImage("p05-2-1", img3);
    break;
  case 2:
  case 3:
  case 4:
  case 5: // pseudo image
    bgr2gray(img3, img2);
    gray2pseudo(img4, img3);
    cvShowImage("p05-2-1", img4);
    break;
  default:
    break;
  }
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
  
  switch (colormap_global)
  {
  case 0: // plane image
    cvShowImage("p05-2-1", img2);
    break;
  case 1: // grayscale
    bgr2gray(img3, img2);
    cvShowImage("p05-2-1", img3);
    break;
  case 2:
  case 3:
  case 4:
  case 5: // pseudo image
    bgr2gray(img3, img2);
    gray2pseudo(img4, img3);
    cvShowImage("p05-2-1", img4);
    break;
  default:
    break;
  }
}

void colormapCallback(int colormap){
  colormap_global = colormap;
  switch (colormap_global)
  {
  case 0: // plane image
    cvShowImage("p05-2-1", img2);
    break;
  case 1: // grayscale
    bgr2gray(img3, img2);
    cvShowImage("p05-2-1", img3);
    break;
  case 2:
  case 3:
  case 4:
  case 5: // pseudo image
    bgr2gray(img3, img2);
    gray2pseudo(img4, img3);
    cvShowImage("p05-2-1", img4);
    break;
  default:
    break;
  }
}

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

// カラー画像をグレイスケール化  
void bgr2gray(IplImage* gray, const IplImage* bgr) {
  for (int y = 0; y < gray->height; y++) {
    for (int x = 0; x < gray->width; x++) {
      gray->imageData[gray->widthStep * y + x] = (unsigned char)(0.298912 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 2]
        + 0.586611 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 1]
        + 0.114478 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 0]);
    }
  }
}

// 疑似カラー化
void gray2pseudo(IplImage* pseudo, const IplImage* gray) {
  for (int y = 0; y < pseudo->height; y++) {
    for (int x = 0; x < pseudo->width; x++) {
      switch (colormap_global)
      {
      case 2: // JET
        if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 32){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = (unsigned char)(4 * (unsigned char)gray->imageData[gray->widthStep * y + x] + 128);
        }
        else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 96){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = (unsigned char)(4 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 32));
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 255;
        }
        else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 160){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = (unsigned char)(4 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 96));
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = (unsigned char)(255 - (4 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 96)));
        }
        else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 244){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = (unsigned char)(255 - (4 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 160)));
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 0;
        }
        else{
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = (unsigned char)(255 - (4 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 244)));
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 0;
        }
        break;
      case 3: // HSV
        if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 42){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = (unsigned char)(6 * (unsigned char)gray->imageData[gray->widthStep * y + x]);
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 0;
        }
        else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 85){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = (unsigned char)(255 - 6 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 42));
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 0;
        }
        else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 128){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = (unsigned char)(6 * ((unsigned char)gray->imageData[gray->widthStep * y + x] + 85));
        }
        else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 170){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = (unsigned char)(255 - 6 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 128));
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 255;
        }
        else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 213){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = (unsigned char)(6 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 170));
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 255;
        }
        else{
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = (unsigned char)(255 - 6 * ((unsigned char)gray->imageData[gray->widthStep * y + x] - 213));
        }
        break;
      case 4: // HOT
        if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 96){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = (unsigned char)gray->imageData[gray->widthStep * y + x] * 8 / 3;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 0;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 0;
        }
        else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 192){
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = (unsigned char)gray->imageData[gray->widthStep * y + x] * 8 / 3;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 0;
        }
        else{
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = 255;
          pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = (unsigned char)gray->imageData[gray->widthStep * y + x] * 4;
        }
        break;
      case 5: // COOL
        pseudo->imageData[pseudo->widthStep * y + x * 3 + 2] = (unsigned char)gray->imageData[gray->widthStep * y + x];
        pseudo->imageData[pseudo->widthStep * y + x * 3 + 1] = (unsigned char)(255 - (unsigned char)gray->imageData[gray->widthStep * y + x]);
        pseudo->imageData[pseudo->widthStep * y + x * 3 + 0] = 255;
        break;
      default:
        break;
      }
    }
  }
}