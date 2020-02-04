#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>


// �v���g�^�C�v�錾  �c�@���������
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


// �J���[�摜���O���C�X�P�[����  
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
  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {
    printf("�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n");
    return;
  }

  // �ǂݍ��񂾉摜�̕\��
  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  // �J���[�摜�������ꍇ�̓O���C�X�P�[����
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

  printf("�s�N�Z���� : %ld\n", n);
  printf("���ϒl     : %lf\n", mu);
  printf("�����l     : %d\n", med);
  printf("���U       : %lf\n", var);
  printf("�W���΍�   : %lf\n", sigma);
  printf("�ŏ��l     : %d\n", min);
  printf("�ő�l     : %d\n", max);
  printf("�͈�       : %d\n", rg);
  printf("Michelson�R���g���X�g : %lf\n", cont);

  cvWaitKey(0);
  cvDestroyAllWindows();

  cvReleaseImage(&img1);
  cvReleaseImage(&imgGray);
}

// �s�N�Z���������߂�
long npixel(IplImage* imgGray){
  long n = imgGray->width * imgGray->height; // �摜�T�C�Y�c�~��
  return n;
}

// ���ϒl�����߂�
double mean(IplImage* imgGray){
  unsigned long sum = 0;
  unsigned long n = imgGray->width * imgGray->height;

  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      sum += (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]; // �e�s�N�Z���l�̑��a
    }
  }

  return (double)(sum / n); // �s�N�Z�����Ŋ����ĕ���
}

// �W���\�[�g�֐�qsort�̂��߂̔�r�֐�
int compare(const void *a, const void *b){
  return *(unsigned char *)a - *(unsigned char *)b;
}

// �����l�����߂�
unsigned char median(IplImage* imgGray){
  size_t length = imgGray->width * imgGray->height; // �s�N�Z����
  IplImage* temp; // �\�[�g�Ώ� ���摜�̃N���[��
  temp = cvCreateImage(cvSize(imgGray->width, imgGray->height), imgGray->depth, 1);

  cvCopy(imgGray, temp);

  // �s�N�Z���z���j��\�[�g
  qsort(temp->imageData, length, sizeof(unsigned char), compare);

  return temp->imageData[length / 2]; // �����l��Ԃ�
}

// ���U�����߂�
double variance(IplImage* imgGray){
  double mean = 0.0; // ����
  unsigned long sum = 0; // ���ς̂��߂̑��a
  unsigned long n = imgGray->width * imgGray->height; // �s�N�Z����
  double variance = 0.0; // ���U

  // ���a���v�Z
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      sum += (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
    }
  }

  // ���ς��v�Z
  mean = (double)(sum / n);

  // ���U�̕�����v�Z
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      variance += pow((unsigned char)imgGray->imageData[imgGray->widthStep * y + x] - mean, 2);
    }
  }

  // �s�N�Z�����Ŋ����ĕ��U
  variance = variance / n; 

  return variance;
}

// �W���΍������߂�
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

  // ���U���߂ĕ�����
  return sqrt(variance);
}

// �ŏ��l�����߂�
unsigned char minimum(IplImage* imgGray){
  unsigned char min = 255;
  
  // �����r
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if (min > (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]){
        min = (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
      }
    }
  }

  return min;
}

// �ő�l�����߂�
unsigned char maximum(IplImage* imgGray){
  unsigned char max = 0;

  // �����r
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      if (max < (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]){
        max = (unsigned char)imgGray->imageData[imgGray->widthStep * y + x];
      }
    }
  }

  return max;
}

// �͈͂����߂�
unsigned char range(IplImage* imgGray){
  unsigned char max = 0;
  unsigned char min = 0;

  //�g���܂킵
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

// Michelson�R���g���X�g�����߂�
double contrast(IplImage* imgGray){
  unsigned char max = 0;
  unsigned char min = 0;

  // �g���܂킵
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