// p05-1 : 256階調(8bit)のカラー画像を8階調(3bit相当)カラー画像に減色する
// 

#include <stdio.h>
#include <opencv/highgui.h>

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* img2;
  IplImage* img3;
  int threshold;

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

  printf("nChannels = %d\n", img1->nChannels);
  if (img1->nChannels != 3) {
    printf("カラー画像を指定して下さい。\n");
    return;
  }

  // Substituting for threshold
  threshold = atoi(argv[2]);

  cvNamedWindow("8bit Color");
  cvShowImage("8bit Color", img1);

  // 読み込んだ画像と同じサイズの画像を生成
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 1);

  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x] = (unsigned char)(0.298912 * (unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 2]
                                                + 0.586611 * (unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 1]
                                                + 0.114478 * (unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 0]);
    }
  }
  cvNamedWindow("Gray scale");
  cvShowImage("Gray scale", img2);
  cvSaveImage("p05-1-1-1.bmp", img2);

  img3 = cvCreateImage(cvSize(img2->width, img2->height), img2->depth, 1);

  for (int y = 0; y < img3->height; y++) {
    for (int x = 0; x < img3->width; x++) {
      if ((unsigned char)img2->imageData[img2->widthStep*y + x] < threshold) {
        img3->imageData[img3->widthStep*y + x] = 0;
      }
      else {
        img3->imageData[img3->widthStep*y + x] = 255;
      }
      
    }
  }
  
  cvNamedWindow("binary");
  cvShowImage("binary", img3);
  cvSaveImage("p05-1-1-2.bmp", img3);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
  cvReleaseImage(&img3);
}