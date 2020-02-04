// p04-2 : 1つのカラーチャネルを0にした画像を生成する
#include <stdio.h>
#include <opencv/highgui.h>

int gbr2h(const unsigned char *bgr);
unsigned char gbr2s(const unsigned char *bgr);
unsigned char gbr2v(const unsigned char *bgr);
void hsv2rgb(const int h, const unsigned char s, const unsigned char v, unsigned char *bgr);

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* img2;
  int h;
  unsigned char v;

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
  cvCopy(img1, img2);

  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      h = gbr2h((unsigned char *)&img2->imageData[img2->widthStep*y + x * 3]);
      v = gbr2v((unsigned char *)&img2->imageData[img2->widthStep*y + x * 3]);

      hsv2rgb(h, 255, v, (unsigned char *)&img2->imageData[img2->widthStep*y + x * 3]);
    }
  }
  cvNamedWindow("p04-2-4");
  cvShowImage("p04-2-4", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}

int gbr2h(const unsigned char *bgr){
  int h;

  if (bgr[0] == bgr[1] && bgr[1] == bgr[2]){  // B = G = R
    h = 0;
    return h;
  }

  if (bgr[0] > bgr[1] && bgr[0] > bgr[2]){ // B is biggest
    if (bgr[1] > bgr[2]){ // B > G > R
      h = (60 * (bgr[2] - bgr[1]) / (bgr[0] - bgr[2]) + 240);
      (h < 0) ? (h += 360) : h;
      return h;
    }
    else{ // B > R > G or B > G = R
      h = (60 * (bgr[2] - bgr[1]) / (bgr[0] - bgr[1]) + 240);
      (h < 0) ? (h += 360) : h;
      return h;
    }
  }

  if (bgr[1] > bgr[0] && bgr[1] > bgr[2]){ // G is biggest
    if (bgr[0] > bgr[2]){ // G > B > R
      h = (60 * (bgr[0] - bgr[2]) / (bgr[1] - bgr[2]) + 120);
      (h < 0) ? (h += 360) : h;
      return h;
    }
    else{ // G > R > B or G > B = R
      h = (60 * (bgr[0] - bgr[2]) / (bgr[1] - bgr[0]) + 120);
      (h < 0) ? (h += 360) : h;
      return h;
    }
  }

  if (bgr[2] > bgr[1] && bgr[2] > bgr[0]){ // R is biggest
    if (bgr[0] > bgr[1]){ // R > B > G
      h = (60 * (bgr[1] - bgr[0]) / (bgr[2] - bgr[1]));
      (h < 0) ? (h += 360) : h;
      return h;
    }
    else{ // R > G > B or R > G = B
      h = (60 * (bgr[1] - bgr[0]) / (bgr[2] - bgr[0]));
      (h < 0) ? (h += 360) : h;
      return h;
    }
  }
}

unsigned char gbr2s(const unsigned char *bgr){
  unsigned char max;
  unsigned char min;

  // estimate max value
  max = bgr[0];
  for (int i = 0; i < 3; i++){
    if (max < bgr[i]){
      max = (unsigned char)bgr[i];
    }
  }

  // estimate min value
  min = bgr[0];
  for (int i = 0; i < 3; i++){
    if (min > bgr[i]){
      min = (unsigned char)bgr[i];
    }
  }

  return ((max - min) * 255 / max);
}

unsigned char gbr2v(const unsigned char *bgr){
  unsigned char max;

  // estimate max value
  max = bgr[0];
  for (int i = 0; i < 3; i++){
    if (max < bgr[i]){
      max = (unsigned char)bgr[i];
    }
  }

  return max;
}

void hsv2rgb(const int h, const unsigned char s, const unsigned char v, unsigned char *bgr){
  float max = float(v);
  float min = (float)(v - (((float)s / 255) * v));

  if (h < 60) {
    bgr[2] = (unsigned char)max; // R
    bgr[1] = (unsigned char)((h * (max - min)) / 60 + min); // G
    bgr[0] = (unsigned char)min; // B
  }
  else if (h < 120) {
    bgr[2] = (unsigned char)((120 - h) * (max - min) / 60 + min); // R
    bgr[1] = (unsigned char)max; // G
    bgr[0] = (unsigned char)min; // B
  }
  else if (h < 180) {
    bgr[2] = (unsigned char)min; // R
    bgr[1] = (unsigned char)max; // G
    bgr[0] = (unsigned char)((h - 120) * (max - min) / 60 + min); // B
  }
  else if (h < 240) {
    bgr[2] = (unsigned char)min; // R
    bgr[1] = (unsigned char)((240 - h) * (max - min) / 60 + min); // G
    bgr[0] = (unsigned char)max; // B
  }
  else if (h < 300) {
    bgr[2] = (unsigned char)((h - 240) * (max - min) / 60 + min); // R
    bgr[1] = (unsigned char)min; // G
    bgr[0] = (unsigned char)max; // B
  }
  else if (h < 360) {
    bgr[2] = (unsigned char)max; // R
    bgr[1] = (unsigned char)min; // G
    bgr[0] = (unsigned char)((360 - h) * (max - min) / 60 + min); // B
  }
}