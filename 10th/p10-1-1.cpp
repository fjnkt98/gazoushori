#include <stdio.h>
#include <opencv/highgui.h>


IplImage *complexInput;          // cvDFT()の入力のために、入力画像に虚数部(=0)を付け加えた複素数画像

CvMat *dft_img;                  // DFT結果(複素数)
CvMat *dft_img_masked;           // DFT結果(dft_img)から mask_img_shift によって一部を抽出したもの

IplImage *image_Re;              // DFT結果の実部。(その後DFT絶対値に書き換えられる)
IplImage *image_Re_masked;       // DFTの絶対値画像上に、マウスで作成中のマスクの状態(mask_img)を赤色で表示するためのカラー画像
IplImage *image_Re_bgr;          // DFTの絶対値画像。image_Re_maskedの初期状態でもある。
// 実際はグレイスケール画像だが、cvCopy()でimage_Re_maskedにコピーを行うために、カラー画像として保持している。

IplImage *mask_img;              // DFT 画像のマスク(0=不使用,0以外=使用)
IplImage *mask_img_shift;        // mask_img の第1象限<==>第3象限, 第2象限<==>第4象限 を入れ替えたもの。cvDFT()の引数はこの形式で入れる必要がある

IplImage *idft_img;              // 逆離散フーリエ変換(IDFT)の結果
IplImage *idft_img_8U;           // 逆離散フーリエ変換(IDFT)の結果を表示するために、8bitグレイスケール画像に変換したもの


int pen = 5;        // マウスでマスクを作成する際に、Ctrlを押している時のペンのサイズ


/* プロトタイプ宣言 */
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr);  // 画像の第1象限<==>第3象限, 第2象限<==>第4象限 を入れ替える 
void onMouse(int event, int x, int y, int flags, void *param); // マウス操作時のコールバック関数

void fillImage(int x, int y);      // DFT画像中の x,y 座標点を「使用する」(マスク画像 mask_image, image_Re_masked の(x,y)点をONにする )
void showIdft();                   // DFT画像をマスクした結果から、IDFTを行い、空間画像として表示する

void setMaskImg();               // DFT画像のマスク mask_img を設定する。(0:不使用, 0以外:使用)
void showMaskImg();                // mask_img の状態を表示する



// DFT画像中の x,y 座標点を「使用する」(マスク画像 mask_image, image_Re_masked の(x,y)点をONにする )
void fillImage(int x, int y) {
  if (x > mask_img->width - 1) x = mask_img->width - 1;
  if (x < 0) x = 0;
  if (y > mask_img->height - 1) y = mask_img->height - 1;
  if (y < 0) y = 0;

  // マスクをONに
  mask_img->imageData[y * mask_img->widthStep + x] = 255;

  // マスクの状態を表示するimage_Re_maskedを設定。(使用部分を赤色(B,G,R)=(0,0,255)にする)
  image_Re_masked->imageData[y * image_Re_masked->widthStep + x *image_Re_masked->nChannels + 0] = 0;
  image_Re_masked->imageData[y * image_Re_masked->widthStep + x *image_Re_masked->nChannels + 1] = 0;
  image_Re_masked->imageData[y * image_Re_masked->widthStep + x *image_Re_masked->nChannels + 2] = 255;
}


// DFT画像をマスクした結果から、IDFTを行い、空間画像として表示する
void showIdft() {
  cvShiftDFT(mask_img, mask_img);                    // 第1象限<==>第3象限, 第2象限<==>第4象限 を入れ替える(cvDFT()の引数の仕様に合わせる)
  cvSetZero(dft_img_masked);                         // いったん 0 で初期化
  cvCopy(dft_img, dft_img_masked, mask_img);         // mask_img がONの部分(≠0の部分)はdft_img、それ以外は0の DFT 画像 dft_img_masked を生成
  cvShiftDFT(mask_img, mask_img);                    // mask_img を元に戻す

  cvDFT(dft_img_masked, idft_img, CV_DXT_INVERSE_SCALE, complexInput->height);  // dft_img_masked を逆フーリエ変換
  cvConvertScaleAbs(idft_img, idft_img_8U, 1.0, 0);  // 結果表示用の 8bitグレイスケール画像 idft_img_8U に変換
  cvShowImage("IDFT", idft_img_8U);                  // 結果を表示

}


// マウスのコールバック関数
void onMouse(int event, int x, int y, int flags, void *param){

  unsigned char size = pen;

  if (flags & CV_EVENT_FLAG_LBUTTON) {    // 左ボタンが押されていて、
    if (flags & CV_EVENT_FLAG_CTRLKEY) {               // かつCtrlキーが押されている時
      for (int j = y - size; j < y + size; j++) {
        for (int i = x - size; i < x + size; i++) {
          fillImage(i, j);
        }
      }
    }
    else {                                           // Ctrlキーが押されていない時
      fillImage(x, y);
    }
    cvShowImage("Magnitude", image_Re_masked);

    showIdft();    // 現在のマスクでの IDFT 結果を表示する

  }


  switch (event) {
  case CV_EVENT_RBUTTONDOWN:             // 右ボタンを押したとき
    cvSet(mask_img, cvScalar(0));            // マスクを初期化
    cvCopy(image_Re_bgr, image_Re_masked);         // 表示するidft画像を初期状態にもどす
    cvShowImage("Magnitude", image_Re_masked);
    showIdft();
    break;
  }

}


// DFT画像のマスク mask_img を設定する。(0:不使用, 0以外:使用)
// ※mode = 1 の場合のみ使用
void setMaskImg() {
  int wsize = 50;

  cvSetZero(mask_img);   // 0でいったん初期化(全て使用しない)

  // このサンプルは、ローパスフィルター的な動作となる
  for (int y = mask_img->height / 2 - wsize; y < mask_img->height / 2 + wsize; y++) {
    for (int x = mask_img->width / 2 - wsize; x < mask_img->width / 2 + wsize; x++){
      mask_img->imageData[y * mask_img->widthStep + x] = 255;
    }
  }

  // 反転
  for (int y = 0; y < mask_img->height; y++){
    for (int x = 0; x < mask_img->width; x++){
      mask_img->imageData[y * mask_img->widthStep + x] = 255 - (unsigned char)mask_img->imageData[y * mask_img->widthStep + x];
    }
  }
}


// mask_img の状態を表示する
// ※mode = 1 の場合のみ使用
void showMaskImg() {
  cvNamedWindow("Mask Pattern");
  cvShowImage("Mask Pattern", mask_img);
}



int main(int argc, char **argv)
{
  int mode = 1;  // mode = 0: マウスによるインタラクティブなマスク作成
  // mode = 1: setMaskImg() でマスクを作成
  // ※この値はコマンドライン引数で指定したほうが良いかも

  IplImage *src_img;
  IplImage *realInput;
  IplImage *imaginaryInput;
  IplImage *image_Im;
  IplImage *image_Re_8U;
  int dft_M, dft_N;
  CvMat tmp;
  double m, M;

  src_img = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  if (!src_img)
    return -1;

  realInput = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_64F, 1);
  imaginaryInput = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_64F, 1);
  complexInput = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_64F, 2);

  // (1)入力画像を実数配列にコピーし，虚数配列とマージして複素数平面を構成
  cvScale(src_img, realInput, 1.0, 0.0);
  cvZero(imaginaryInput);
  cvMerge(realInput, imaginaryInput, NULL, NULL, complexInput);

  // (2)DFT用の最適サイズを計算し，そのサイズで行列を確保する
  dft_M = cvGetOptimalDFTSize(src_img->height - 1);
  dft_N = cvGetOptimalDFTSize(src_img->width - 1);
  dft_img = cvCreateMat(dft_M, dft_N, CV_64FC2);
  dft_img_masked = cvCreateMat(dft_M, dft_N, CV_64FC2);
  image_Re = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_64F, 1);
  image_Im = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_64F, 1);

  image_Re_masked = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_8U, 3);   // bgrカラー画像


  mask_img = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_8U, 1);
  mask_img_shift = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_8U, 1);


  // (3)複素数平面をdft_imgにコピーし，残りの行列右側部分を0で埋める
  cvGetSubRect(dft_img, &tmp, cvRect(0, 0, src_img->width, src_img->height));

  cvCopy(complexInput, &tmp, NULL);
  if (dft_img->cols > src_img->width) {
    cvGetSubRect(dft_img, &tmp, cvRect(src_img->width, 0, dft_img->cols - src_img->width, src_img->height));
    cvZero(&tmp);
  }

  // (4)離散フーリエ変換を行い，その結果を実数部分と虚数部分に分解
  cvDFT(dft_img, dft_img, CV_DXT_FORWARD, complexInput->height);
  cvSplit(dft_img, image_Re, image_Im, 0, 0);


  /*
  // 位相を求める
  IplImage *image_Phase = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_8U, 1);

  for (int y = 0; y < image_Re->height; y++) {
  for (int x = 0; x < image_Re->width; x++) {
  image_Phase->imageData[y * image_Phase->widthStep + x] = atan2(image_Re->imageData[y*image_Re->widthStep + x], image_Im->imageData[y*image_Im->widthStep + x])/CV_PI/2*255+127;
  }
  }
  cvSaveImage("DFT_Phase.png", image_Phase);
  cvShiftDFT(image_Phase, image_Phase);
  cvSaveImage("DFT_Phase_Shifted.png", image_Phase);
  */



  // (5)スペクトルの絶対値(振幅)を計算 Mag = sqrt(Re^2 + Im^2)
  cvPow(image_Re, image_Re, 2.0);
  cvPow(image_Im, image_Im, 2.0);
  cvAdd(image_Re, image_Im, image_Re, NULL);
  cvPow(image_Re, image_Re, 0.5);

  // (6)振幅の対数をとる log(1 + Mag)   … 値のレンジが広いので、見やすくするため
  cvAddS(image_Re, cvScalarAll(1.0), image_Re, NULL);  // 全要素に1を足す
  cvLog(image_Re, image_Re);

  // (7)原点（直流成分）が画像の中心にくるように，画像の象限を入れ替える
  cvShiftDFT(image_Re, image_Re);

  // (8)振幅画像のピクセル値が0.0-1.0に分布するようにスケーリング
  cvMinMaxLoc(image_Re, &m, &M, NULL, NULL, NULL);
  cvScale(image_Re, image_Re, 1.0 / (M - m), 1.0 * (-m) / (M - m));

  image_Re_8U = cvCreateImage(cvGetSize(image_Re), IPL_DEPTH_8U, 1);
  image_Re_bgr = cvCreateImage(cvGetSize(image_Re), IPL_DEPTH_8U, 3);
  cvConvertScaleAbs(image_Re, image_Re_8U, 255, 0);

  for (int y = 0; y < image_Re->height; y++) {
    for (int x = 0; x < image_Re->width; x++){
      image_Re_bgr->imageData[y * image_Re_bgr->widthStep + x * 3 + 0] = (unsigned char)image_Re_8U->imageData[y * image_Re_8U->widthStep + x];
      image_Re_bgr->imageData[y * image_Re_bgr->widthStep + x * 3 + 1] = (unsigned char)image_Re_8U->imageData[y * image_Re_8U->widthStep + x];
      image_Re_bgr->imageData[y * image_Re_bgr->widthStep + x * 3 + 2] = (unsigned char)image_Re_8U->imageData[y * image_Re_8U->widthStep + x];
    }
  }

  cvSet(mask_img, cvScalar(0));                       // マスクを初期化
  cvCopy(image_Re_bgr, image_Re_masked);    // 表示するidft画像を初期状態に


  cvNamedWindow("Original");
  cvShowImage("Original", src_img);
  cvNamedWindow("Magnitude");
  cvShowImage("Magnitude", image_Re_bgr);



  cvSaveImage("DFT_Shifted.png", image_Re_8U);

  cvShiftDFT(image_Re_8U, image_Re_8U);  // 既に象限の入れ替え後なので、いったん再度入れ替えることで、入れ替え前の状態に
  cvSaveImage("DFT.png", image_Re_8U);
  cvShiftDFT(image_Re_8U, image_Re_8U);  // 元に戻しておく



  ///////
  idft_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_64F, 1);
  idft_img_8U = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  cvDFT(dft_img, idft_img, CV_DXT_INVERSE_SCALE, complexInput->height);      // IDFT を実行
  cvConvertScaleAbs(idft_img, idft_img_8U, 1.0, 0);         // 表示のために 8bit グレイスケール画像に変換

  cvNamedWindow("IDFT");
  cvShowImage("IDFT", idft_img_8U);



  if (mode == 0) {   // mode==0 の場合は、マウスでマスクを作成するモードとなる
    cvSetMouseCallback("Magnitude", onMouse, (void *)0);
    cvCreateTrackbar("Pen size", "Magnitude", &pen, 100);
  }
  else {             // mode==1 の場合は、setMasImg() 関数でマスクを生成し、結果を表示して終了
    setMaskImg();
    showMaskImg();
  }

  showIdft();    // mask_img によってマスクしたDFT画像 dft_img を元に IDFT を行った結果を表示する 
  ////////

  cvWaitKey(0);

  return 0;
}

/* 原点（直流成分）が画像の中心にくるように，画像の象限を入れ替える関数．
src_arr, dst_arr は同じサイズ，タイプの配列 */
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr)
{
  CvMat *tmp = 0;
  CvMat q1stub, q2stub;
  CvMat q3stub, q4stub;
  CvMat d1stub, d2stub;
  CvMat d3stub, d4stub;
  CvMat *q1, *q2, *q3, *q4;
  CvMat *d1, *d2, *d3, *d4;

  CvSize size = cvGetSize(src_arr);
  CvSize dst_size = cvGetSize(dst_arr);
  int cx, cy;

  if (dst_size.width != size.width || dst_size.height != size.height) {
    cvError(CV_StsUnmatchedSizes, "cvShiftDFT", "Source and Destination arrays must have equal sizes", __FILE__,
      __LINE__);
  }
  // (9)インプレースモード用のテンポラリバッファ
  if (src_arr == dst_arr) {
    tmp = cvCreateMat(size.height / 2, size.width / 2, cvGetElemType(src_arr));
  }
  cx = size.width / 2;          /* 画像中心 */
  cy = size.height / 2;

  // (10)1?4象限を表す配列と，そのコピー先
  q1 = cvGetSubRect(src_arr, &q1stub, cvRect(0, 0, cx, cy));
  q2 = cvGetSubRect(src_arr, &q2stub, cvRect(cx, 0, cx, cy));
  q3 = cvGetSubRect(src_arr, &q3stub, cvRect(cx, cy, cx, cy));
  q4 = cvGetSubRect(src_arr, &q4stub, cvRect(0, cy, cx, cy));
  d1 = cvGetSubRect(src_arr, &d1stub, cvRect(0, 0, cx, cy));
  d2 = cvGetSubRect(src_arr, &d2stub, cvRect(cx, 0, cx, cy));
  d3 = cvGetSubRect(src_arr, &d3stub, cvRect(cx, cy, cx, cy));
  d4 = cvGetSubRect(src_arr, &d4stub, cvRect(0, cy, cx, cy));

  // (11)実際の象限の入れ替え
  if (src_arr != dst_arr) {
    if (!CV_ARE_TYPES_EQ(q1, d1)) {
      cvError(CV_StsUnmatchedFormats, "cvShiftDFT", "Source and Destination arrays must have the same format",
        __FILE__, __LINE__);
    }
    cvCopy(q3, d1, 0);
    cvCopy(q4, d2, 0);
    cvCopy(q1, d3, 0);
    cvCopy(q2, d4, 0);
  }
  else {                        /* インプレースモード */
    cvCopy(q3, tmp, 0);
    cvCopy(q1, q3, 0);
    cvCopy(tmp, q1, 0);
    cvCopy(q4, tmp, 0);
    cvCopy(q2, q4, 0);
    cvCopy(tmp, q2, 0);
  }
}