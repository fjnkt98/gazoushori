#include <stdio.h>
#include <opencv/highgui.h>

// カラー画像をグレイスケール化  
void bgr2gray(IplImage* gray, IplImage* bgr) {
  for (int y = 0; y < gray->height; y++) {
    for (int x = 0; x < gray->width; x++) {
      gray->imageData[gray->widthStep * y + x] = (unsigned char)(0.298912 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 2]
                                                               + 0.586611 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 1]
                                                               + 0.114478 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 0]);
    }
  }
}

// 疑似カラー化
void bgr2pseudo(unsigned char* bgr, unsigned char gray) {
  // COOL
  bgr[2] = 255 - gray;  // R値
  bgr[1] = gray;        // G値
  bgr[0] = 255;         // B値
}

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* imgPseudo;
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
  imgGray = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 1);
  if (img1->nChannels != 1) {
    bgr2gray(imgGray, img1);
    cvNamedWindow("Gray");
    cvShowImage("Gray", imgGray);
  }
  else {
    cvCopy(img1, imgGray);
  }

  // 読み込んだ画像と同じサイズのカラー画像を生成
  imgPseudo = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 3);

  // 疑似カラー画像を生成
  for (int y = 0; y < imgPseudo->height; y++) {
    for (int x = 0; x < imgPseudo->width; x++) {
      bgr2pseudo((unsigned char *)&imgPseudo->imageData[imgPseudo->widthStep * y + x * 3], (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]);
    }
  }

  cvNamedWindow("Pseudo Color");
  cvShowImage("Pseudo Color", imgPseudo);
  cvSaveImage("p05-1-5.bmp", imgPseudo);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&imgGray);
  cvReleaseImage(&imgPseudo);
}