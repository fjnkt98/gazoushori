#include <stdio.h>
#include <opencv/highgui.h>

IplImage* img1;
IplImage* img2;

void rCallback(int r);
void gCallback(int g);
void bCallback(int b);

void main(int argc, char* argv[])
{
  // Variables for trackbar
  int r = 100;
  int g = 100;
  int b = 100;

  printf("argc = %d\n", argc);
  for (int k = 0; k < argc; k++) {
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc < 2) {
    printf("ファイル名を指定してください。\n");
    return;
  }

  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {
    printf("画像ファイルの読み込みに失敗しました。\n");
    return;
  }

  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  printf("nChannels = %d\n", img1->nChannels);
  if (img1->nChannels != 3) {
    printf("カラー画像を指定して下さい。\n");
    return;
  }

  // Create and copy image
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);
  cvCopy(img1, img2);

  // Create window
  cvNamedWindow("p04-2-2");

  // Create trackbar
  cvCreateTrackbar("R", "p04-2-2", &r, 100, rCallback);
  cvCreateTrackbar("G", "p04-2-2", &g, 100, gCallback);
  cvCreateTrackbar("B", "p04-2-2", &b, 100, bCallback);

  // Show image
  cvShowImage("p04-2-2", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}

void rCallback(int r){
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 2] = (unsigned char)(img1->imageData[img1->widthStep*y + x * 3 + 2] * r / 100); // R
    }
  }
  cvShowImage("p04-2-2", img2);
}

void gCallback(int g){
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 1] = (unsigned char)(img1->imageData[img1->widthStep*y + x * 3 + 1] * g / 100); // G
    }
  }
  cvShowImage("p04-2-2", img2);
}

void bCallback(int b){
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 0] = (unsigned char)(img1->imageData[img1->widthStep*y + x * 3 + 0] * b / 100); // G
    }
  }
  cvShowImage("p04-2-2", img2);
}