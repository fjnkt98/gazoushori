#include <stdio.h>
#include <opencv/highgui.h>
   
// �t�B���^�W�����̃t�B���^�̃p�����[�^��ێ�����\����
typedef struct {
   int type;           // �t�B���^�̎�ʁB1:���`�t�B���^, 2:����`�t�B���^(���f�B�A���t�B���^)
   int size;           // �t�B���^�̑傫���B�e�L�X�g�ł����Ƃ���� w
   int widthStep;       // imageData[] �Ɠ��l�̕��@�ŌW���z��ɃA�N�Z�X���o����悤�ɂ��邽�߂̕ϐ�
   double* weight;     // �W���̔z�񂪓���
   double sigma;       // �K�E�V�A���t�B���^�p(��)
} Operator;

// �v���g�^�C�v�錾
IplImage* subImage(IplImage* subImg, IplImage* img, int x, int y, int w, int h);
unsigned char medianFilter(IplImage* subImg, int ch);
unsigned char linearFilter(IplImage* subImg, int ch, Operator* ope);
void filterImage(IplImage* img1, IplImage* img2, int size, int kind);
void makeMovingAverageOpe(Operator * ope);

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
// �q���g: �w�肵�� ch (B==0, G==1, R==2 �̂����ꂩ)�̉�f�l�ւ́A�ȉ��ŃA�N�Z�X�ł���
// (unsigned char)subImg->imageData[y * subImg->widthStep + x * subImg->nChannels + ch]
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
   
   
// ��ݍ��ݐϕ����s�� (�J���[�^�O���C�X�P�[�����Ή��̂��߁Ach ���w��B(BGR�̃`���l�����ɏ�������))
// �q���g: (x,y)�̈ʒu�̃t�B���^�W���́A�u�@ope->weight[y * ope->widthStep + x]�@�v �Ƃ��ăA�N�Z�X�ł���
unsigned char linearFilter(IplImage* subImg, int ch, Operator* ope) {
  double sum = 0;
   
  for (int y = 0; y < subImg->height; y++) {
    for (int x = 0; x < subImg->width; x++) {
      sum += (unsigned char)subImg->imageData[subImg->widthStep * y + x * subImg->nChannels + ch] * ope->weight[y * ope->widthStep + x];
    }
  }
  return sum;
}
   
// img1 �� ope �Őݒ肵���t�B���^�����������̂� img2 �Ƃ��ďo��
void filterImage(IplImage* img1, IplImage* img2, Operator ope) {
  IplImage* buff = cvCreateImage(cvSize(ope.size * 2 + 1, ope.size * 2 + 1), img1->depth, img1->nChannels);
   
  switch (ope.type) {
  case 1: // ���`�t�B���^�̏ꍇ�͑S�Ă���
  for (int y = ope.size; y < img1->height - ope.size - 1; y++) {
    for (int x = ope.size; x < img1->width - ope.size - 1; x++) {
    for (int ch = 0; ch < img2->nChannels; ch++) {    // �O���C�X�P�[���^�J���[���Ή��̂��߂̃��[�v
        img2->imageData[y * img2->widthStep + x * img2->nChannels + ch]
          = linearFilter(subImage(buff, img1, x - ope.size, y - ope.size, ope.size * 2 + 1, ope.size * 2 + 1), ch, &ope);
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
  }
}
   
   
// �ړ����σt�B���^�̌W�������
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
   
   
  // ---------------------���f�B�A���t�B���^---------------------
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);   // �ǂݍ��񂾉摜�Ɠ����傫���̉摜�𐶐�;
  cvSetZero(img2);   // 0(��)�ŏ��������Ă���
   
  ope.size = 1;
  ope.type = 2;   // ���f�B�A���t�B���^
  filterImage(img1, img2, ope);  // img1 �� ope �Őݒ肵���t�B���^�����������̂� img2 �Ƃ��ďo��
  cvNamedWindow("p08-1-2-median");
  cvShowImage("p08-1-2-median", img2);
  cvSaveImage("p08-1-2-median.bmp", img2);
   
  // -------------------------------------------------------------

  cvSetZero(img2);   // ��U����

  // ---------------------�ړ����σt�B���^---------------------
  makeMovingAverageOpe(&ope);   // �����ňړ����ϗp�̌W�����̃p�����[�^���쐬�B �� ������ ope.type=1 (���`�t�B���^) �ɐݒ肳���
   
  filterImage(img1, img2, ope);
  cvNamedWindow("p08-1-2-moving-average");
  cvShowImage("p08-1-2-moving-average", img2);
  cvSaveImage("p08-1-2-moving-average.bmp", img2);
  // ---------------------------------------------------------
   
  cvWaitKey(0);
  cvDestroyAllWindows();

  free(ope.weight);
}