#include <stdio.h>
#include <opencv/highgui.h>

// プロトタイプ宣言
void histogram(long* hist, IplImage* imgGray);
unsigned char otsu(long* hist, unsigned char min, unsigned char max, double mu);
void bw(IplImage* imgBw, IplImage* imgGray, unsigned char th);

unsigned char minimum(IplImage* imgGray);
unsigned char maximum(IplImage* imgGray);
double mean(IplImage* imgGray);

long calcN1(unsigned char th, unsigned char min, long *hist);
long calcN2(unsigned char th, unsigned char max, long *hist);
double calcMean1(unsigned char th, unsigned char min, long *hist);
double calcMean2(unsigned char th, unsigned char max, long *hist);

void bgr2gray(IplImage* gray, IplImage* bgr);

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* imgGray;
  IplImage* imgBw;
  // 起動オプションのチェック    // 画像データの読み込み    // 読み込んだ画像の表示
  // カラー画像だった場合はグレイスケール化
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
  long hist[256];
  unsigned char th;
  double mu = mean(imgGray);
  unsigned char min = minimum(imgGray);
  unsigned char max = maximum(imgGray);

  // 配列の初期化
  for (int i = 0; i < 256; i++){
    hist[i] = 0;
  }

  histogram(hist, imgGray);       // ヒストグラムを作成

  th = otsu(hist, min, max, mu);  // 大津の方法により閾値 th を決定
  printf("Threshold = %d", th);

  imgBw = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 1);
  bw(imgBw, imgGray, th);  //閾値 th により、imgGray を二値化したものを imgBw に格納

  cvNamedWindow("BW");
  cvShowImage("BW", imgBw);
  cvSaveImage("p06-1-3.bmp", imgBw);

  cvWaitKey(0);
  cvDestroyAllWindows();
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

// 最大値を抽出する関数
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

// 最小値を抽出する関数
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

// 平均値を計算する関数
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

// 閾値th による二値化画像 imgBw (0か255のみのグレイスケール画像)を生成
void bw(IplImage* imgBw, IplImage* imgGray, unsigned char th) {
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if ((unsigned char)imgGray->imageData[imgGray->widthStep * y + x] > th){
        imgBw->imageData[imgBw->widthStep * y + x] = 255;
      }
      else{
        imgBw->imageData[imgBw->widthStep * y + x] = 0;
      }
    }
  }
}

// 大津の方法
unsigned char otsu(long* hist, unsigned char min, unsigned char max, double mu){
  unsigned char th_max = 0;
  unsigned char th = 0;

  long n1 = 0;
  long n2 = 0;
  double mean1 = 0.0;
  double mean2 = 0.0;

  double v_max = 0.0;
  double v = 0.0;

  // 総当たり
  for (th = min; th < max; th++){
    // クラス1のピクセル数、平均、分散を求める
    n1 = calcN1(th, min, hist);
    mean1 = calcMean1(th, min, hist);
    
    // クラス2のピクセル数、平均、分散を求める
    n2 = calcN2(th, max, hist);
    mean2 = calcMean2(th, max, hist);

    // 評価
    v = n1 * pow((mean1 - mu), 2) + n2 * pow((mean2 - mu), 2);

    if (v > v_max){
      v_max = v;
      th_max = th;
    }
  }

  return th_max;
}

// クラス1のピクセル数を計算する関数
long calcN1(unsigned char th, unsigned char min, long *hist){
  long sum = 0;

  for (int i = min; i < th; i++){
    sum += hist[i];
  }

  return sum;
}

// クラス2のピクセル数を計算する関数
long calcN2(unsigned char th, unsigned char max, long *hist){
  long sum = 0;

  for (int i = th; i < max; i++){
    sum += hist[i];
  }

  return sum;
}

// クラス1の平均値を計算する関数
double calcMean1(unsigned char th, unsigned char min, long *hist){
  long sum_of_number = 0;
  long sum_of_value = 0;

  for (int i = min; i < th; i++){
    sum_of_number += hist[i];
    sum_of_value += i * hist[i];
  }

  if (sum_of_number == 0){
    return 0;
  }
  else{
    return (double)(sum_of_value / sum_of_number);
  }
}

// クラス2の平均値を計算する関数
double calcMean2(unsigned char th, unsigned char max, long *hist){
  long sum_of_number = 0;
  long sum_of_value = 0;

  for (int i = th; i < max; i++){
    sum_of_number += hist[i];
    sum_of_value += i * hist[i];
  }

  if (sum_of_number == 0){
    return 0;
  }
  else{
    return (double)(sum_of_value / sum_of_number);
  }
}