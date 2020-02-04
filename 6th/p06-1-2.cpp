#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>

void histogram(long* hist, IplImage* imgGray);

void bgr2gray(IplImage* gray, IplImage* bgr);

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* imgGray;

  // 起動オプションのチェック
  printf("argc = %d\n", argc);
  for (int k = 0; k < argc; k++) {
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc < 2) {
    printf("ファイル名を指定してください。\n");
    return;
  }

  // 画像データの読み込み
  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {
    printf("画像ファイルの読み込みに失敗しました。\n");
    return;
  }

  // 読み込んだ画像の表示
  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  // カラー画像だった場合はグレイスケール化
  printf("nChannels = %d\n", img1->nChannels);
  if (img1->nChannels != 1) {
    imgGray = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 1);
    bgr2gray(imgGray, img1);
    cvNamedWindow("Gray");
    cvShowImage("Gray", imgGray);
  }
  else {
    imgGray = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 1);
    cvCopy(img1, imgGray);
  }

  long hist[256];

  // 配列の初期化
  for (int i = 0; i < 256; i++){
    hist[i] = 0;
  }
  histogram(hist, imgGray);

  // ファイル出力
  FILE* fp;
  if (fopen_s(&fp, "histogram.csv", "w") == 0) {
    for (int i = 0; i < 256; i++) {
      fprintf(fp, "%d, %ld\n", i, hist[i]);
      printf("%3d = %4ld\n", i, hist[i]);
    }
    fclose(fp);
  }

  cvWaitKey(0);
  cvDestroyAllWindows();

  cvReleaseImage(&img1);
  cvReleaseImage(&imgGray);
}

// カラー画像をグレイスケール化  
void bgr2gray(IplImage* gray, IplImage* bgr) {
  for (int y = 0; y < gray->height; y++) {
    for (int x = 0; x < gray->width; x++) {
      gray->imageData[gray->widthStep * y + x]
        = (unsigned char)bgr->imageData[bgr->widthStep * y + x * 3 + 2] * 0.298912
        + (unsigned char)bgr->imageData[bgr->widthStep * y + x * 3 + 1] * 0.586611
        + (unsigned char)bgr->imageData[bgr->widthStep * y + x * 3 + 0] * 0.114478;
    }
  }
}

// ビン数256限定
void histogram(long* hist, IplImage* imgGray){
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      hist[(unsigned char)imgGray->imageData[imgGray->widthStep * y + x]]++;
    }
  }
}