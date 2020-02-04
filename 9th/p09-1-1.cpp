#include <stdio.h>
#include <opencv/highgui.h>

// �t�B���^�W�����̃t�B���^�̃p�����[�^��ێ�����\���� (�O��Ɠ���)
typedef struct {
  int type;           // �t�B���^�̎�ʁB1:���`�t�B���^, 2:����`�t�B���^(���f�B�A���t�B���^)
  int size;           // �t�B���^�̑傫���B�e�L�X�g�ł����Ƃ���� w
  int widthStep;       // imageData[] �Ɠ��l�̕��@�ŌW���z��ɃA�N�Z�X���o����悤�ɂ��邽�߂̕ϐ�
  double* weight;     // �W���̔z�񂪓���
  double sigma;       // �K�E�V�A���t�B���^�p(��)
} Operator;


IplImage* subImage(IplImage* subImg, IplImage* img, int x, int y, int w, int h);    // �O��Ɠ���
unsigned char medianFilter(IplImage* subImg, int ch);                               // �O��Ɠ���

double linearFilter(IplImage* subImg, int ch, Operator *ope);                       // [�Ԃ�l�̌^��ύX]
void filterImage(IplImage* img1, IplImage* img2, int size, int kind);               // [��L�ύX�ɑΉ����ďC��]
unsigned char clip(double v);                                                       // [�ǉ�] 0-255�̐����l�ɃN���b�v����(v<0 ==> 0, v>255 ==> 255)
void stretch(double* tmp, IplImage* img2, double* minV, double* maxV);              // [�ǉ�] �ŏ��l0, �ő�l255�ƂȂ�悤�����L�΂�

// �摜�̈ꕔ��(�摜�u���b�N)��؂�o��
IplImage* subImage(IplImage* subImg, IplImage* img, int x, int y, int w, int h) {

  // ��������0�ŏ�����(img�̕��⍂���𒴂����ꍇ�̑΍�)
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

// �摜�u���b�N subImage ����(�w�肵��ch ��)�����l��Ԃ��B
unsigned char medianFilter(IplImage* subImg, int ch) {
  long sum = 0;
  unsigned char hist[256] = {};   // ={} �ɂ��A�z�񂪑S�� 0 �ŏ����������(�����l�̐����z��̑傫����菭�Ȃ��ꍇ�A�c��̕�����0�ŏ����������)
  int r = 0;
  long npixel = subImg->width * subImg->height;

  // �q�X�g�O���� hist[] ���쐬
  for (int y = 0; y < subImg->height; y++) {
    for (int x = 0; x < subImg->width; x++) {
      hist[(unsigned char)subImg->imageData[subImg->widthStep * y + x * subImg->nChannels + ch]]++;
    }
  }

  // �q�X�g�O���� hist ���璆���l r �����߂�B
  for (int i = 0; i < 256; i++){
    sum += hist[i];

    if (sum >= (npixel / 2)){
      r = i;
      break;
    }
  }

  return r;
}

// 0-255�̐����l�ɃN���b�v����(v<0 ==> 0, v>255 ==> 255)
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


// ��ݍ��ݐϕ����s�� (�J���[�^�O���C�X�P�[�����Ή��̂��߁Ach ���w��B(BGR�̃`���l�����ɏ�������))
double linearFilter(IplImage* subImg, int ch, Operator *ope) {
  double sum = 0.0;

  for (int y = 0; y < subImg->height; y++) {
    for (int x = 0; x < subImg->width; x++) {
      sum += (unsigned char)subImg->imageData[y * subImg->widthStep + x * subImg->nChannels + ch] * ope->weight[y * ope->widthStep + x];
    }
  }
  return sum;
}


// �s�N�Z���l�͈̔͂�ch���� �ŏ��l0, �ő�l255�ƂȂ�悤�����L�΂�(���J���[�摜�̏ꍇ�F���ς��ꍇ����)
void stretch(double* tmp, IplImage* img2, double* minV, double* maxV) {
  for (int ch = 0; ch < img2->nChannels; ch++) {
    for (int y = 0; y < img2->height; y++) {
      for (int x = 0; x < img2->width; x++) {
        img2->imageData[y * img2->widthStep + x * img2->nChannels + ch] = (int)((tmp[y * (img2->width * img2->nChannels) + x * img2->nChannels + ch] - minV[ch]) / (maxV[ch] - minV[ch]) * 255);
      }
    }
  }
}


// img1 �� ope �Őݒ肵���t�B���^�����������̂� img2 �Ƃ��ďo��
void filterImage(IplImage* img1, IplImage* img2, Operator ope) {

  IplImage* buff = cvCreateImage(cvSize(ope.size * 2 + 1, ope.size * 2 + 1), img1->depth, img1->nChannels);

  switch (ope.type) {
  case 1: // ���`�t�B���^�̏ꍇ�͂���
    for (int y = ope.size; y < img1->height - ope.size - 1; y++) {
      for (int x = ope.size; x < img1->width - ope.size - 1; x++) {
        for (int ch = 0; ch < img2->nChannels; ch++) {    // �O���C�X�P�[���^�J���[���Ή��̂��߂̃��[�v
          img2->imageData[y * img2->widthStep + x * img2->nChannels + ch]
            = clip(linearFilter(subImage(buff, img1, x - ope.size, y - ope.size, ope.size * 2 + 1, ope.size * 2 + 1), ch, &ope)); // [clip()���g�p����悤�ɕύX]
        }
      }
    }
    break;

  case 2: // ���f�B�A���t�B���^�̏ꍇ
    for (int y = ope.size; y < img1->height - ope.size - 1; y++) {
      for (int x = ope.size; x < img1->width - ope.size - 1; x++) {
        for (int ch = 0; ch < img2->nChannels; ch++) {    // �O���C�X�P�[���^�J���[���Ή��̂��߂̃��[�v
          img2->imageData[y * img2->widthStep + x * img2->nChannels + ch]
            = medianFilter(subImage(buff, img1, x - ope.size, y - ope.size, ope.size * 2 + 1, ope.size * 2 + 1), ch);
        }
      }
    }
    break;


  case 3: // ���`�t�B���^������Astretch()���s���ꍇ�B
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
        for (int ch = 0; ch < img2->nChannels; ch++) {    // �O���C�X�P�[���^�J���[���Ή��̂��߂̃��[�v
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


// X������1�������t�B���^�̌W�������
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

// Y������1�������t�B���^�̌W�������
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

// 4�ߖTLaplacian�t�B���^�̌W�������
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
// X������Prewitt�t�B���^�̌W�������
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

// Y������Prewitt�t�B���^�̌W�������
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

// X������Sobel�t�B���^�̌W�������
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

// Y������Sobel�t�B���^�̌W�������
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


// 8�ߖTLaplacian�t�B���^�̌W�������
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


// �N�s���t�B���^�̌W�������
void makeSharpenOpe(Operator * ope){
  ope->type = 1; // �P�x�␳(stretch())���s��Ȃ�
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

  // �N���I�v�V�����̃`�F�b�N
  printf("argc = %d\n", argc);
  for (int k = 0; k < argc; k++) {
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc < 2) {
    printf("�t�@�C�������w�肵�Ă��������B\n");
    return;
  }


  // �摜�f�[�^�̓ǂݍ���
  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {   // �ǂݍ��񂾉摜�̓J���[�̏ꍇ���A�O���C�X�P�[���摜�̏ꍇ������
    printf("�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n");
    return;
  }
  // �ǂݍ��񂾉摜�̕\��
  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  // -------------------------------------------------------------
  Operator ope;
  ope.weight = NULL;  // �ŏ��ɏ��������Ă���

  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);   // �ǂݍ��񂾉摜�Ɠ����傫���̉摜�𐶐�;
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