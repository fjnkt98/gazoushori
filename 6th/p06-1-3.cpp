#include <stdio.h>
#include <opencv/highgui.h>

// �v���g�^�C�v�錾
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
  // �N���I�v�V�����̃`�F�b�N    // �摜�f�[�^�̓ǂݍ���    // �ǂݍ��񂾉摜�̕\��
  // �J���[�摜�������ꍇ�̓O���C�X�P�[����
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
  long hist[256];
  unsigned char th;
  double mu = mean(imgGray);
  unsigned char min = minimum(imgGray);
  unsigned char max = maximum(imgGray);

  // �z��̏�����
  for (int i = 0; i < 256; i++){
    hist[i] = 0;
  }

  histogram(hist, imgGray);       // �q�X�g�O�������쐬

  th = otsu(hist, min, max, mu);  // ��Â̕��@�ɂ��臒l th ������
  printf("Threshold = %d", th);

  imgBw = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 1);
  bw(imgBw, imgGray, th);  //臒l th �ɂ��AimgGray ���l���������̂� imgBw �Ɋi�[

  cvNamedWindow("BW");
  cvShowImage("BW", imgBw);
  cvSaveImage("p06-1-3.bmp", imgBw);

  cvWaitKey(0);
  cvDestroyAllWindows();
}

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

// �r����256����
void histogram(long* hist, IplImage* imgGray){
  for (int y = 0; y < imgGray->height; y++) {
    for (int x = 0; x < imgGray->width; x++) {
      hist[(unsigned char)imgGray->imageData[imgGray->widthStep * y + x]]++;
    }
  }
}

// �ő�l�𒊏o����֐�
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

// �ŏ��l�𒊏o����֐�
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

// ���ϒl���v�Z����֐�
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

// 臒lth �ɂ���l���摜 imgBw (0��255�݂̂̃O���C�X�P�[���摜)�𐶐�
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

// ��Â̕��@
unsigned char otsu(long* hist, unsigned char min, unsigned char max, double mu){
  unsigned char th_max = 0;
  unsigned char th = 0;

  long n1 = 0;
  long n2 = 0;
  double mean1 = 0.0;
  double mean2 = 0.0;

  double v_max = 0.0;
  double v = 0.0;

  // ��������
  for (th = min; th < max; th++){
    // �N���X1�̃s�N�Z�����A���ρA���U�����߂�
    n1 = calcN1(th, min, hist);
    mean1 = calcMean1(th, min, hist);
    
    // �N���X2�̃s�N�Z�����A���ρA���U�����߂�
    n2 = calcN2(th, max, hist);
    mean2 = calcMean2(th, max, hist);

    // �]��
    v = n1 * pow((mean1 - mu), 2) + n2 * pow((mean2 - mu), 2);

    if (v > v_max){
      v_max = v;
      th_max = th;
    }
  }

  return th_max;
}

// �N���X1�̃s�N�Z�������v�Z����֐�
long calcN1(unsigned char th, unsigned char min, long *hist){
  long sum = 0;

  for (int i = min; i < th; i++){
    sum += hist[i];
  }

  return sum;
}

// �N���X2�̃s�N�Z�������v�Z����֐�
long calcN2(unsigned char th, unsigned char max, long *hist){
  long sum = 0;

  for (int i = th; i < max; i++){
    sum += hist[i];
  }

  return sum;
}

// �N���X1�̕��ϒl���v�Z����֐�
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

// �N���X2�̕��ϒl���v�Z����֐�
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