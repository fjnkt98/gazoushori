// p04-2 : 1つのカラーチャネルを0にした画像を生成する
#include <stdio.h>
#include <opencv/highgui.h>

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* img2;

  printf("argc = %d\n", argc);                 // 起動オプションのチェック
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

  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  printf("nChannels = %d\n", img1->nChannels);
  if (img1->nChannels != 3) {
    printf("カラー画像を指定して下さい。\n");
    return;
  }

  // 読み込んだ画像と同じサイズの画像を生成
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);

  // Rチャネルを0に ----------------
  cvCopy(img1, img2);            // 元画像をコピー
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 1] = 0;
      img2->imageData[img2->widthStep*y + x * 3 + 3] = 0;
    }
  }
  cvNamedWindow("R");
  cvShowImage("R", img2);

  // Gチャネルを0に ----------------
  cvCopy(img1, img2);            // 元画像をコピー
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 0] = 0;
      img2->imageData[img2->widthStep*y + x * 3 + 2] = 0;
    }
  }
  cvNamedWindow("G");
  cvShowImage("G", img2);

  // Bチャネルを0に ----------------
  cvCopy(img1, img2);            // 元画像をコピー
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 1] = 0;
      img2->imageData[img2->widthStep*y + x * 3 + 2] = 0;
    }
  }
  cvNamedWindow("B");
  cvShowImage("B", img2);


  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}