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


void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* img2;

  float a = 0;
  int b = 0;
  float gamma = 0;

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

  if (argc == 4){
    a = atof(argv[2]);
    b = atoi(argv[3]);
  }

  if (argc == 3){
    gamma = atof(argv[2]);
  }

  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);

  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      if (argc == 4){ // Linear transform
        img2->imageData[img2->widthStep*y + x * 3 + 0] = max(min((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 0]) * a + b), 255), 0);
        img2->imageData[img2->widthStep*y + x * 3 + 1] = max(min((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 1]) * a + b), 255), 0);
        img2->imageData[img2->widthStep*y + x * 3 + 2] = max(min((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 2]) * a + b), 255), 0);
      }

      if (argc == 3){ // gamma transform
        img2->imageData[img2->widthStep*y + x * 3 + 0] = max(min((255 * pow(((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 0] / 255.0), 1 / gamma)), 255), 0);
        img2->imageData[img2->widthStep*y + x * 3 + 1] = max(min((255 * pow(((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 1] / 255.0), 1 / gamma)), 255), 0);
        img2->imageData[img2->widthStep*y + x * 3 + 2] = max(min((255 * pow(((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 2] / 255.0), 1 / gamma)), 255), 0);
      }
      
    }
  }
  cvNamedWindow("Tuned up image");
  cvShowImage("Tuned up image", img2);
  cvSaveImage("p05-1-4.bmp", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}