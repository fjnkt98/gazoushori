#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>

// ローパスフィルタ係数
double iir_coefficient = 0.0;

void trackbarCb(int coefficient);

// IIRフィルタ(ディジタルローパスフィルタ)
void iir(IplImage *frame, IplImage *oldFrame, IplImage *outputFrame){
  for (int y = 0; y < outputFrame->height; y++) {
    for (int x = 0; x < outputFrame->width; x++) {
      for (int ch = 0; ch < outputFrame->nChannels; ch++) {
        outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch] =
          (unsigned char)(
            (unsigned char)frame->imageData[y * frame->widthStep + x * frame->nChannels + ch] * iir_coefficient
            + (unsigned char)oldFrame->imageData[y * oldFrame->widthStep + x * oldFrame->nChannels + ch] * (1 - iir_coefficient)
          );
      }
    }
  }
}

int main(int argc, char *argv[]){
  CvCapture *capture = NULL;      // ビデオキャプチャ構造体

  int c;
  int n = 0;
  IplImage *grab = NULL;          // cvQueryFrame() の返り値を一時的に保存するための変数
  IplImage *frame = NULL;         // 現在のフレーム(※この変数の内容を書き換えてはいけない)
  IplImage *outputFrame = NULL;   // 表示する画像(画像処理結果を入れる)
  IplImage *oldframe = NULL;      // 一つ前のフレーム
  IplImage *oldOutput = NULL;     // 一つ前の出力フレーム

  int mode = 0;                   // 起動時のモード
  int width = 640;                // デフォルトの画像サイズ
  int height = 480;

  int pos = 0;  // トラックバー変数

  // 起動オプションの数が 0 (argc==1)の場合　… キャプチャサイズ: 640x480、カメラ番号:0
  // 起動オプションの数が 1 (argc==2)の場合　… 指定した動画ファイルを読み込む
  // 起動オプションの数が 2 (argc==3)の場合　… キャプチャサイズ: arg[1]×arg[2]、カメラ番号:0
  // 起動オプションの数が 3 (argc==4)の場合　… キャプチャサイズ: arg[1]×arg[2]、カメラ番号:arg[3]
  switch (argc) {
  case 1:
    capture = cvCreateCameraCapture(0);
    break;

  case 2:
    capture = cvCaptureFromFile(argv[1]);
    break;

  case 3:
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    capture = cvCreateCameraCapture(0);
    break;

  case 4:
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    capture = cvCreateCameraCapture(atoi(argv[3]));
    break;
  }

  // 取込サイズの設定
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width);
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height);

  // ウィンドウの表示
  cvNamedWindow("Output");
  // トラックバーの生成
  cvCreateTrackbar("係数", "Output", &pos, 100, trackbarCb);

  // カメラの準備ができるまで待つ
  while (cvQueryFrame(capture) == NULL){
    c = cvWaitKey(2);    // esc を押した場合は終了(存在しないカメラ番号を指定した場合などのため)
    if (c == '\x1b') {
      return -1;
    }
  }

  // 現在のフレームをキャプチャし、frameにコピー(同じフォーマットの画像を生成して初期化)
  frame = cvCloneImage(cvQueryFrame(capture));
  outputFrame = cvCloneImage(frame);  // キャプチャ画像と同じフォーマットの画像を生成して初期化
  oldframe = cvCloneImage(frame);     // キャプチャ画像と同じフォーマットの画像を生成して初期化
  oldOutput = cvCloneImage(frame);

  // 動画像処理のループ ========================================
  while ((grab = cvQueryFrame(capture)) != NULL) {   // NULLが返った場合(動画ファイルの場合は末尾に達した場合)は終了
    // カメラから画像フレームを取込み(grab)、作業用の変数(frame)にコピー
    cvCopy(grab, frame);  // ※grab の中身は書き換えてはいけません!!

    switch (mode) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      iir(frame, oldOutput, outputFrame);  // IIRフィルタ
      break;
    case 0:
    default:
      cvCopy(frame, outputFrame);   // キャプチャ画像をそのまま表示
      break;
    }

    // 処理結果の表示
    cvShowImage("Output", outputFrame);

    //キー入力待ち
    c = cvWaitKey(2);
    if (c == '\x1b') {   // escキーで終了
      break;
    }
    else if (c >= '0' && c <= '9') {  // 0～9 を押した場合に、mode = 0～9 にセット
      mode = c - '0';     // ※ASCIIコードの並びを参照
      printf("mode = %d\n", mode);
    }
    else if (c == ' ') {   // 画像ファイルとして保存
      n++;
      char buf[10];
      sprintf_s(buf, 10, "%d.png", n);
      cvSaveImage(buf, outputFrame);
      puts(buf);
    }

    cvCopy(frame, oldframe);  // 一つ前のフレームとして保存
    cvCopy(outputFrame, oldOutput);
  }

  // 後始末
  cvReleaseCapture(&capture);
  cvDestroyWindow("Output");
}

void trackbarCb(int coefficient){
  iir_coefficient = (double)coefficient / 100;
}