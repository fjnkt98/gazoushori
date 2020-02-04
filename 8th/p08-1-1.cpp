#include <stdio.h>
#include <opencv/highgui.h>

void writeFile(IplImage* img, char* imgFilename, char *addName);
void addSoltAndPepper(IplImage *img, double noiseLv);
unsigned char clip(int gray);
void addWhite(IplImage *img, double noiseLv);
void addNoise(IplImage* img, int noiseType, double noiseLv);


// ===============================================================================

// �m�C�Y�t���摜�̏����o��
void writeFile(IplImage* img, char* imgFilename, char *addName) {
  char filename[256];
  char *ext_p;

  strcpy_s(filename, 256, imgFilename); // �ǂݍ��񂾉摜�̃t�@�C�������R�s�[
  ext_p = strrchr(filename, '.');
  *ext_p = '\0';   // �t�@�C��������g���q������
  sprintf_s(filename, 255, "%s+%s.bmp", filename, addName); // �m�C�Y����t�������t�@�C�����𐶐�

  cvSaveImage(filename, img);
}


// ���܉��m�C�Y�̕t��
void addSoltAndPepper(IplImage *img, double noiseLv) {
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      for (int ch = 0; ch < img->nChannels; ch++) {   // �O���C�X�P�[���摜�E�J���[�摜�ǂ���ɂ��Ή�
        if (rand() < RAND_MAX * noiseLv) {
          if (rand() < RAND_MAX / 2) {
            img->imageData[y * img->widthStep + x * img->nChannels + ch] = 0;
          }
          else {
            img->imageData[y * img->widthStep + x * img->nChannels + ch] = 255;
          }
        }
      }
    }
  }
}


// �l�� 0-255 �͈̔͂ɃN���b�v����
unsigned char clip(int gray) {
  return (gray < 0) ? 0 : (gray > 255) ? 255 : gray;
}


// �z���C�g�m�C�Y(�Ȉ�)�̕t��
void addWhite(IplImage *img, double noiseLv) {
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      for (int ch = 0; ch < img->nChannels; ch++) {   // �O���C�X�P�[���摜�E�J���[�摜�ǂ���ɂ��Ή�
        img->imageData[y * img->widthStep + x * img->nChannels + ch]
          = clip(((unsigned char)img->imageData[y * img->widthStep + x * img->nChannels + ch]) + ((float)rand() / RAND_MAX * 511 - 255) * noiseLv);

      }
    }
  }
}


// �m�C�Y�̎�ނɂ��Ăяo���֐���U�蕪��
void addNoise(IplImage* img, int noiseType, double noiseLv) {
  switch (noiseType) {
  case 0:
  default:
    addWhite(img, noiseLv);  // �z���C�g�m�C�Y��t��
    break;

  case 1:
    addSoltAndPepper(img, noiseLv);  // ���܉��m�C�Y��t��
    break;
  }
}


void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* img2;
  int noiseType = 1;       // �m�C�Y�^�C�v [0(Default):�z���C�g�m�C�Y, 1:���܉��m�C�Y]
  double noiseLv = 0.05;   // �m�C�Y���x�� [noiseType==0 : �m�C�Y�̋��x, noiseType==1 : �m�C�Y�̐��N����] (Default=0.05)
  char* noiseStr[] = { "WhiteNoise", "Salt&PepperNoise" };

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

  if (argc >= 3) {
    noiseType = atoi(argv[2]);  // �m�C�Y���x���̐ݒ�
  }
  printf("Noise Type = %d (%s)\n", noiseType, noiseStr[noiseType]);


  if (argc >= 4) {
    noiseLv = atof(argv[3]);  // �m�C�Y���x���̐ݒ�
  }
  printf("Noise Lv = %f\n", noiseLv);

  // �摜�f�[�^�̓ǂݍ���
  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {   // �ǂݍ��񂾉摜�̓J���[�̏ꍇ���A�O���C�X�P�[���摜�̏ꍇ������
    printf("�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n");
    return;
  }
  // �ǂݍ��񂾉摜�̕\��
  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  // -------------------------------------------------------------
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);   // �ǂݍ��񂾉摜�Ɠ����傫���̉摜�𐶐�
  cvCopy(img1, img2);  // �摜�f�[�^���R�s�[

  addNoise(img2, noiseType, noiseLv);


  cvNamedWindow(noiseStr[noiseType]);
  cvShowImage(noiseStr[noiseType], img2);

  writeFile(img2, argv[1], noiseStr[noiseType]);         // �m�C�Y�t���摜���t�@�C���ɏo��

  cvWaitKey(0);
  cvDestroyAllWindows();
}