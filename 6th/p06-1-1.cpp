#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>


// プロトタイプ宣言  …　これらを作る
long npixel(IplImage* imgGray);
double mean(IplImage* imgGray);
unsigned char median(IplImage* imgGray);
double variance(IplImage* imgGray);
double stddev(IplImage* imgGray);
unsigned char minimum(IplImage* imgGray);
unsigned char maximum(IplImage* imgGray);
unsigned char range(IplImage* imgGray);
double contrast(IplImage* imgGray);

int compare(const void *a, const void *b);

void bgr2gray(IplImage* gray, IplImage* bgr);


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

  // -------------------------------------------------------------
  long n;
  unsigned char max, min, med, rg;
  double mu, var, sigma, cont;

  n = npixel(imgGray);
  mu = mean(imgGray);
  med = median(imgGray);
  var = variance(imgGray);
  sigma = stddev(imgGray);
  min = minimum(imgGray);
  max = maximum(imgGray);
  rg = range(imgGray);
  cont = contrast(imgGray);

  printf("ピクセル数 : %ld\n", n);
  printf("平均値     : %lf\n", mu);
  printf("中央値     : %d\n", med);
  printf("分散       : %lf\n", var);
  printf("標準偏差   : %lf\n", sigma);
  printf("最小値     : %d\n", min);
  printf("最大値     : %d\n", max);
  printf("範囲       : %d\n", rg);
  printf("Michelsonコントラスト : %lf\n", cont);

  cvWaitKey(0);
  cvDestroyAllWindows();

  cvReleaseImage(&img1);
  cvReleaseImage(&imgGray);
}

// ピクセル数を求める
long npixel(IplImage* imgGray){
  long n = imgGray->width * imgGray->height; // 画像サイズ縦×横
  return n;
}

// 平均値を求める
double mean(IplImage* imgGray){
  unsigned long sum = 0;
  unsigned long n = imgGray->width * imgGray->height;

  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      sum += (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]; // 各ピクセル値の総和
    }
  }

  return (double)(sum / n); // ピクセル数で割って平均
}

// 標準ソート関数qsortのための比較関数
int compare(const void *a, const void *b){
  return *(unsigned char *)a - *(unsigned char *)b;
}

// 中央値を求める
unsigned char median(IplImage* imgGray){
  size_t length = imgGray->width * imgGray->height; // ピクセル数
  IplImage* temp; // ソート対象 元画像のクローン
  temp = cvCreateImage(cvSize(imgGray->width, imgGray->height), imgGray->depth, 1);

  cvCopy(imgGray, temp);

  // ピクセル配列を破壊ソート
  qsort(temp->imageData, length, sizeof(unsigned char), compare);

  return temp->imageData[length / 2]; // 中央値を返す
}

// 分散を求める
double variance(IplImage* imgGray){
  double mean = 0.0; // 平均
  unsigned long sum = 0; // 平均のための総和
  unsigned long n = imgGray->width * imgGray->height; // ピクセル数
  double variance = 0.0; // 分散

  // 総和を計算
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      sum += (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
    }
  }

  // 平均を計算
  mean = (double)(sum / n);

  // 分散の分母を計算
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      variance += pow((unsigned char)imgGray->imageData[imgGray->widthStep * y + x] - mean, 2);
    }
  }

  // ピクセル数で割って分散
  variance = variance / n; 

  return variance;
}

// 標準偏差を求める
double stddev(IplImage* imgGray){
  double mean = 0.0;
  unsigned long sum = 0;
  unsigned long n = imgGray->width * imgGray->height;
  double variance = 0.0;

  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      sum += (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
    }
  }

  mean = (double)(sum / n);

  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      variance += pow((unsigned char)imgGray->imageData[imgGray->widthStep * y + x] - mean, 2);
    }
  }

  variance = variance / n;

  // 分散求めて平方根
  return sqrt(variance);
}

// 最小値を求める
unsigned char minimum(IplImage* imgGray){
  unsigned char min = 255;
  
  // 逐一比較
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if (min > (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]){
        min = (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
      }
    }
  }

  return min;
}

// 最大値を求める
unsigned char maximum(IplImage* imgGray){
  unsigned char max = 0;

  // 逐一比較
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if (max < (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]){
        max = (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
      }
    }
  }

  return max;
}

// 範囲を求める
unsigned char range(IplImage* imgGray){
  unsigned char max = 0;
  unsigned char min = 0;

  //使いまわし
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if (min >(unsigned char)imgGray->imageData[imgGray->widthStep * y + x]){
        min = (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
      }
    }
  }

  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if (max < (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]){
        max = (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
      }
    }
  }

  return max - min;
}

// Michelsonコントラストを求める
double contrast(IplImage* imgGray){
  unsigned char max = 0;
  unsigned char min = 0;

  // 使いまわし
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if (min >(unsigned char)imgGray->imageData[imgGray->widthStep * y + x]){
        min = (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
      }
    }
  }

  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if (max < (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]){
        max = (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
      }
    }
  }

  return (max - min) / (max + min);
}