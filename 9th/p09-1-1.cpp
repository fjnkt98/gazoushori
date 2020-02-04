#include <stdio.h>
#include <opencv/highgui.h>

// フィルタ係数等のフィルタのパラメータを保持する構造体 (前回と同じ)
typedef struct {
  int type;           // フィルタの種別。1:線形フィルタ, 2:非線形フィルタ(メディアンフィルタ)
  int size;           // フィルタの大きさ。テキストでいうところの w
  int widthStep;       // imageData[] と同様の方法で係数配列にアクセスが出来るようにするための変数
  double* weight;     // 係数の配列が入る
  double sigma;       // ガウシアンフィルタ用(σ)
} Operator;


IplImage* subImage(IplImage* subImg, IplImage* img, int x, int y, int w, int h);    // 前回と同じ
unsigned char medianFilter(IplImage* subImg, int ch);                               // 前回と同じ

double linearFilter(IplImage* subImg, int ch, Operator *ope);                       // [返り値の型を変更]
void filterImage(IplImage* img1, IplImage* img2, int size, int kind);               // [上記変更に対応して修正]
unsigned char clip(double v);                                                       // [追加] 0-255の整数値にクリップする(v<0 ==> 0, v>255 ==> 255)
void stretch(double* tmp, IplImage* img2, double* minV, double* maxV);              // [追加] 最小値0, 最大値255となるよう引き伸ばす

// 画像の一部分(画像ブロック)を切り出す
IplImage* subImage(IplImage* subImg, IplImage* img, int x, int y, int w, int h) {

  // いったん0で初期化(imgの幅や高さを超えた場合の対策)
  cvSetZero(subImg);

  for (int i = y; i < y + h && i < img->height; i++) {
    for (int j = x; j < x + w && j < img->width; j++) {
      for (int ch = 0; ch < img->nChannels; ch++) {
        subImg->imageData[(i - y) * subImg->widthStep + (j - x) * img->nChannels + ch] = img->imageData[img->widthStep * i + j * img->nChannels + ch];
      }
    }
  }
  return subImg;
}

// 画像ブロック subImage 内の(指定したch の)中央値を返す。
unsigned char medianFilter(IplImage* subImg, int ch) {
  long sum = 0;
  unsigned char hist[256] = {};   // ={} により、配列が全て 0 で初期化される(初期値の数が配列の大きさより少ない場合、残りの部分は0で初期化される)
  int r = 0;
  long npixel = subImg->width * subImg->height;

  // ヒストグラム hist[] を作成
  for (int y = 0; y < subImg->height; y++) {
    for (int x = 0; x < subImg->width; x++) {
      hist[(unsigned char)subImg->imageData[subImg->widthStep * y + x * subImg->nChannels + ch]]++;
    }
  }

  // ヒストグラム hist から中央値 r を求める。
  for (int i = 0; i < 256; i++){
    sum += hist[i];

    if (sum >= (npixel / 2)){
      r = i;
      break;
    }
  }

  return r;
}

// 0-255の整数値にクリップする(v<0 ==> 0, v>255 ==> 255)
unsigned char clip(double v) {
  if (v < 0){
    return 0;
  }
  else if (v > 255){
    return 255;
  }
  else{
    return (unsigned char)v;
  }
}


// 畳み込み積分を行う (カラー／グレイスケール両対応のため、ch を指定。(BGRのチャネル毎に処理する))
double linearFilter(IplImage* subImg, int ch, Operator *ope) {
  double sum = 0.0;

  for (int y = 0; y < subImg->height; y++) {
    for (int x = 0; x < subImg->width; x++) {
      sum += (unsigned char)subImg->imageData[y * subImg->widthStep + x * subImg->nChannels + ch] * ope->weight[y * ope->widthStep + x];
    }
  }
  return sum;
}


// ピクセル値の範囲がch毎に 最小値0, 最大値255となるよう引き伸ばす(※カラー画像の場合色が変わる場合あり)
void stretch(double* tmp, IplImage* img2, double* minV, double* maxV) {
  for (int ch = 0; ch < img2->nChannels; ch++) {
    for (int y = 0; y < img2->height; y++) {
      for (int x = 0; x < img2->width; x++) {
        img2->imageData[y * img2->widthStep + x * img2->nChannels + ch] = (int)((tmp[y * (img2->width * img2->nChannels) + x * img2->nChannels + ch] - minV[ch]) / (maxV[ch] - minV[ch]) * 255);
      }
    }
  }
}


// img1 に ope で設定したフィルタをかけたものを img2 として出力
void filterImage(IplImage* img1, IplImage* img2, Operator ope) {

  IplImage* buff = cvCreateImage(cvSize(ope.size * 2 + 1, ope.size * 2 + 1), img1->depth, img1->nChannels);

  switch (ope.type) {
  case 1: // 線形フィルタの場合はここ
    for (int y = ope.size; y < img1->height - ope.size - 1; y++) {
      for (int x = ope.size; x < img1->width - ope.size - 1; x++) {
        for (int ch = 0; ch < img2->nChannels; ch++) {    // グレイスケール／カラー両対応のためのループ
          img2->imageData[y * img2->widthStep + x * img2->nChannels + ch]
            = clip(linearFilter(subImage(buff, img1, x - ope.size, y - ope.size, ope.size * 2 + 1, ope.size * 2 + 1), ch, &ope)); // [clip()を使用するように変更]
        }
      }
    }
    break;

  case 2: // メディアンフィルタの場合
    for (int y = ope.size; y < img1->height - ope.size - 1; y++) {
      for (int x = ope.size; x < img1->width - ope.size - 1; x++) {
        for (int ch = 0; ch < img2->nChannels; ch++) {    // グレイスケール／カラー両対応のためのループ
          img2->imageData[y * img2->widthStep + x * img2->nChannels + ch]
            = medianFilter(subImage(buff, img1, x - ope.size, y - ope.size, ope.size * 2 + 1, ope.size * 2 + 1), ch);
        }
      }
    }
    break;


  case 3: // 線形フィルタ処理後、stretch()を行う場合。
  {
    double* tmp = (double *)malloc(sizeof(double) * img2->width * img2->height * img2->nChannels);
    double v;
    double* maxV = (double *)malloc(sizeof(double) * img2->nChannels);
    double* minV = (double *)malloc(sizeof(double) * img2->nChannels);

    for (int ch = 0; ch < img2->nChannels; ch++) {
      maxV[ch] = DBL_MIN;
      minV[ch] = DBL_MAX;
    }

    for (int y = ope.size; y < img1->height - ope.size - 1; y++) {
      for (int x = ope.size; x < img1->width - ope.size - 1; x++) {
        for (int ch = 0; ch < img2->nChannels; ch++) {    // グレイスケール／カラー両対応のためのループ
          v = linearFilter(subImage(buff, img1, x - ope.size, y - ope.size, ope.size * 2 + 1, ope.size * 2 + 1), ch, &ope);
          tmp[y * (img2->width * img2->nChannels) + x * img2->nChannels + ch] = v;
          if (v < minV[ch]) {
            minV[ch] = v;
          }
          if (v > maxV[ch]) {
            maxV[ch] = v;
          }
        }
      }
    }
    stretch(tmp, img2, minV, maxV);
    free(minV);
    free(maxV);
    free(tmp);
  }
  break;
  }

  cvReleaseImage(&buff);
}


// X方向の1次微分フィルタの係数を作る
void makeXDiffOpe(Operator * ope){
  ope->type = 3;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = 0;   ope->weight[1] = 0;    ope->weight[2] = 0;
  ope->weight[3] = 0;   ope->weight[4] = -1;   ope->weight[5] = 1;
  ope->weight[6] = 0;   ope->weight[7] = 0;    ope->weight[8] = 0;
}

// Y方向の1次微分フィルタの係数を作る
void makeYDiffOpe(Operator * ope){
  ope->type = 3;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = 0;   ope->weight[1] = 0;    ope->weight[2] = 0;
  ope->weight[3] = 0;   ope->weight[4] = -1;   ope->weight[5] = 0;
  ope->weight[6] = 0;   ope->weight[7] = 1;    ope->weight[8] = 0;
}

// 4近傍Laplacianフィルタの係数を作る
void make4LaplacianOpe(Operator * ope){
  ope->type = 3;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = 0;   ope->weight[1] = 1;    ope->weight[2] = 0;
  ope->weight[3] = 1;   ope->weight[4] = -4;   ope->weight[5] = 1;
  ope->weight[6] = 0;   ope->weight[7] = 1;    ope->weight[8] = 0;
}

//==================================
// X方向のPrewittフィルタの係数を作る
void makeXPrewittOpe(Operator * ope){
  ope->type = 3;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = -1;   ope->weight[1] = 0;    ope->weight[2] = 1;
  ope->weight[3] = -1;   ope->weight[4] = 0;    ope->weight[5] = 1;
  ope->weight[6] = -1;   ope->weight[7] = 0;    ope->weight[8] = 1;
}

// Y方向のPrewittフィルタの係数を作る
void makeYPrewittOpe(Operator * ope){
  ope->type = 3;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = -1;  ope->weight[1] = -1;   ope->weight[2] = -1;
  ope->weight[3] = 0;   ope->weight[4] = 0;    ope->weight[5] = 0;
  ope->weight[6] = 1;   ope->weight[7] = 1;    ope->weight[8] = 1;
}

// X方向のSobelフィルタの係数を作る
void makeXSobelOpe(Operator * ope){
  ope->type = 3;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = -1;   ope->weight[1] = 0;   ope->weight[2] = 1;
  ope->weight[3] = -2;   ope->weight[4] = 0;   ope->weight[5] = 2;
  ope->weight[6] = -1;   ope->weight[7] = 0;   ope->weight[8] = 1;
}

// Y方向のSobelフィルタの係数を作る
void makeYSobelOpe(Operator * ope){
  ope->type = 3;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = -1;   ope->weight[1] = -2;  ope->weight[2] = -1;
  ope->weight[3] = 0;    ope->weight[4] = 0;   ope->weight[5] = 0;
  ope->weight[6] = 1;    ope->weight[7] = 2;   ope->weight[8] = 1;
}


// 8近傍Laplacianフィルタの係数を作る
void make8LaplacianOpe(Operator * ope){
  ope->type = 3;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = 1;   ope->weight[1] = 1;   ope->weight[2] = 1;
  ope->weight[3] = 1;   ope->weight[4] = -8;   ope->weight[5] = 1;
  ope->weight[6] = 1;   ope->weight[7] = 1;   ope->weight[8] = 1;
}


// 鮮鋭化フィルタの係数を作る
void makeSharpenOpe(Operator * ope){
  ope->type = 1; // 輝度補正(stretch())を行わない
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  free(ope->weight);
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);

  ope->weight[0] = 0;    ope->weight[1] = -1;   ope->weight[2] = 0;
  ope->weight[3] = -1;   ope->weight[4] = 5;    ope->weight[5] = -1;
  ope->weight[6] = 0;    ope->weight[7] = -1;   ope->weight[8] = 0;
}



void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* img2;

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
  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {   // 読み込んだ画像はカラーの場合も、グレイスケール画像の場合もある
    printf("画像ファイルの読み込みに失敗しました。\n");
    return;
  }
  // 読み込んだ画像の表示
  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  // -------------------------------------------------------------
  Operator ope;
  ope.weight = NULL;  // 最初に初期化しておく

  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);   // 読み込んだ画像と同じ大きさの画像を生成;
  cvSetZero(img2);

  // =============================================================
  makeXDiffOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("XDiff");
  cvShowImage("XDiff", img2);

  // ----------------------------------
  makeYDiffOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("YDiff");
  cvShowImage("YDiff", img2);

  // ----------------------------------
  make4LaplacianOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("4Laplacian");
  cvShowImage("4Laplacian", img2);

  // =============================================================
  makeXPrewittOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("XPrewitt");
  cvShowImage("XPrewitt", img2);

  // ----------------------------------
  makeYPrewittOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("YPrewitt");
  cvShowImage("YPrewitt", img2);

  // ----------------------------------
  makeXSobelOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("XSobel");
  cvShowImage("XSobel", img2);

  // ----------------------------------
  makeYSobelOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("YSobel");
  cvShowImage("YSobel", img2);

  // ----------------------------------
  make8LaplacianOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("8Laplacian");
  cvShowImage("8Laplacian", img2);


  // =============================================================
  makeSharpenOpe(&ope);
  filterImage(img1, img2, ope);
  cvNamedWindow("Sharpening");
  cvShowImage("Sharpening", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
}