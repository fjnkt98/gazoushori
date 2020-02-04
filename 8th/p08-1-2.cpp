#include <stdio.h>
#include <opencv/highgui.h>
   
// フィルタ係数等のフィルタのパラメータを保持する構造体
typedef struct {
   int type;           // フィルタの種別。1:線形フィルタ, 2:非線形フィルタ(メディアンフィルタ)
   int size;           // フィルタの大きさ。テキストでいうところの w
   int widthStep;       // imageData[] と同様の方法で係数配列にアクセスが出来るようにするための変数
   double* weight;     // 係数の配列が入る
   double sigma;       // ガウシアンフィルタ用(σ)
} Operator;

// プロトタイプ宣言
IplImage* subImage(IplImage* subImg, IplImage* img, int x, int y, int w, int h);
unsigned char medianFilter(IplImage* subImg, int ch);
unsigned char linearFilter(IplImage* subImg, int ch, Operator* ope);
void filterImage(IplImage* img1, IplImage* img2, int size, int kind);
void makeMovingAverageOpe(Operator * ope);

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
// ヒント: 指定した ch (B==0, G==1, R==2 のいずれか)の画素値へは、以下でアクセスできる
// (unsigned char)subImg->imageData[y * subImg->widthStep + x * subImg->nChannels + ch]
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
   
   
// 畳み込み積分を行う (カラー／グレイスケール両対応のため、ch を指定。(BGRのチャネル毎に処理する))
// ヒント: (x,y)の位置のフィルタ係数は、「　ope->weight[y * ope->widthStep + x]　」 としてアクセスできる
unsigned char linearFilter(IplImage* subImg, int ch, Operator* ope) {
  double sum = 0;
   
  for (int y = 0; y < subImg->height; y++) {
    for (int x = 0; x < subImg->width; x++) {
      sum += (unsigned char)subImg->imageData[subImg->widthStep * y + x * subImg->nChannels + ch] * ope->weight[y * ope->widthStep + x];
    }
  }
  return sum;
}
   
// img1 に ope で設定したフィルタをかけたものを img2 として出力
void filterImage(IplImage* img1, IplImage* img2, Operator ope) {
  IplImage* buff = cvCreateImage(cvSize(ope.size * 2 + 1, ope.size * 2 + 1), img1->depth, img1->nChannels);
   
  switch (ope.type) {
  case 1: // 線形フィルタの場合は全てここ
  for (int y = ope.size; y < img1->height - ope.size - 1; y++) {
    for (int x = ope.size; x < img1->width - ope.size - 1; x++) {
    for (int ch = 0; ch < img2->nChannels; ch++) {    // グレイスケール／カラー両対応のためのループ
        img2->imageData[y * img2->widthStep + x * img2->nChannels + ch]
          = linearFilter(subImage(buff, img1, x - ope.size, y - ope.size, ope.size * 2 + 1, ope.size * 2 + 1), ch, &ope);
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
  }
}
   
   
// 移動平均フィルタの係数を作る
void makeMovingAverageOpe(Operator * ope){
  ope->type = 1;
  ope->size = 1;
  ope->widthStep = ope->size * 2 + 1;
  ope->weight = (double *)malloc(sizeof(double) * ope->widthStep* ope->widthStep);
  ope->sigma = 1.0;

  for (int i = 0; i < ope->widthStep * ope->widthStep; i++){
    ope->weight[i] = 1.0 / (ope->widthStep * ope->widthStep);
  }
}

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* img2;
  Operator ope;
   
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
   
   
  // ---------------------メディアンフィルタ---------------------
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);   // 読み込んだ画像と同じ大きさの画像を生成;
  cvSetZero(img2);   // 0(黒)で初期化しておく
   
  ope.size = 1;
  ope.type = 2;   // メディアンフィルタ
  filterImage(img1, img2, ope);  // img1 に ope で設定したフィルタをかけたものを img2 として出力
  cvNamedWindow("p08-1-2-median");
  cvShowImage("p08-1-2-median", img2);
  cvSaveImage("p08-1-2-median.bmp", img2);
   
  // -------------------------------------------------------------

  cvSetZero(img2);   // 一旦消す

  // ---------------------移動平均フィルタ---------------------
  makeMovingAverageOpe(&ope);   // ここで移動平均用の係数等のパラメータを作成。 ※ 内部で ope.type=1 (線形フィルタ) に設定される
   
  filterImage(img1, img2, ope);
  cvNamedWindow("p08-1-2-moving-average");
  cvShowImage("p08-1-2-moving-average", img2);
  cvSaveImage("p08-1-2-moving-average.bmp", img2);
  // ---------------------------------------------------------
   
  cvWaitKey(0);
  cvDestroyAllWindows();

  free(ope.weight);
}