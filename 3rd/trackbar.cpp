#include <stdio.h>
#include <opencv/highgui.h>

// コールバック関数のプロトタイプ宣言
void trackCallback(int pos);

IplImage* img;
IplImage* img2;

void main(int argc, char* argv[])
{
  // 起動オプションのチェック
  printf("argc = %d\n", argc);
  for (int k = 0; k < argc; k++) {
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc > 1 && (img = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) != NULL) {

    img2 = cvCloneImage(img); // 画像データをコピー

    int pos = 100;

    cvNamedWindow("p02-3_Trackbar"); // ウインドウの生成
    cvCreateTrackbar("明るさ", "p02-3_Trackbar", &pos, 200, trackCallback);
    cvShowImage("p02-3_Trackbar", img2); // 画像の表示

    // ----------------------------------------
    cvWaitKey(0); // キーが押されるまで待機

    cvDestroyAllWindows(); // 全ウインドウの破棄
    cvReleaseImage(&img);
    cvReleaseImage(&img2);
  }
  else
    printf("ファイル名を指定してください。\n");
}

void trackCallback(int pos){
  printf("%d%%\n", pos);

  int p;
  for (int x = 0; x < img->width; x++) {
    for (int y = 0; y < img->height; y++) {
      p = (unsigned char)img->imageData[img->widthStep * y + x] * pos / 100;
      if (p>255) p = 255;

      img2->imageData[img->widthStep * y + x] = p;
    }
  }
  cvShowImage("p02-3_Trackbar", img2);
}