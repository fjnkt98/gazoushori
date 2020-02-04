#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>

const char useChar[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!#$%&'()+,-;=@[]^_`{}~";
const char charImgPath[] = "C:/Users/Kohta.AKIRA-PC/Documents/Visual Studio 2013/Projects/ConsoleApplication1/ConsoleApplication1/char";

typedef struct {
  double mean;            // ���ϖ��邳
} Param;

// --------------------------- Function prototype declaration --------------------------
void subImage(IplImage* subImg, IplImage* img, int x, int y, int w, int h);
void calcParam(Param *param, IplImage* img);
double calcDistance(Param* a, Param* b);
Param* calcCharParam(CvSize* size);
Param* calcImgparam(CvSize* outSize, IplImage* img1, CvSize size);
char* genString(Param* paramChar, Param* paramImg, CvSize outSize);
void writeFile(char *outStr, Param* paramImg, CvSize outSize, char* imgFilename);
// --------------------------------------------------------------------------------------

void main(int argc, char* argv[])
{
  IplImage* img1;

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
  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR)) == NULL) {   // (�O���C�X�P�[���ł����Ă�)�J���[�摜�Ƃ��ēǂݍ���
    printf("�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n");
    return;
  }

  // �ǂݍ��񂾉摜�̕\��
  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  // -------------------------------------------------------------
  Param* paramChar = NULL;
  Param* paramImg = NULL;
  char *outStr = NULL;
  CvSize outSize;

  CvSize size;

  paramChar = calcCharParam(&size);
  paramImg = calcImgparam(&outSize, img1, size);
  outStr = genString(paramChar, paramImg, outSize);

  printf("%s\n", outStr);
  writeFile(outStr, paramImg, outSize, argv[1]);

  cvWaitKey(0);
  cvDestroyAllWindows();
}

// �摜�̈ꕔ��(�摜�u���b�N)��؂�o��
void subImage(IplImage* subImg, IplImage* img, int x, int y, int w, int h) {
  // ��������0�ŏ�����(img�̕��⍂���𒴂����ꍇ�̑΍�)
  cvSetZero(subImg);

  for (int i = y; i < y + h && i < img->height; i++) {
    for (int j = x; j < x + w && j < img->width; j++) {
      subImg->imageData[(i - y) * subImg->widthStep + (j - x) * 3 + 2] = img->imageData[img->widthStep * i + j * 3 + 2];
      subImg->imageData[(i - y) * subImg->widthStep + (j - x) * 3 + 1] = img->imageData[img->widthStep * i + j * 3 + 1];
      subImg->imageData[(i - y) * subImg->widthStep + (j - x) * 3 + 0] = img->imageData[img->widthStep * i + j * 3 + 0];
    }
  }
}

// �摜�p�����[�^(���邳�̕��ϒl)���v�Z
void calcParam(Param *param, IplImage* img) {
  long sum = 0;
  long npixel = img->width * img->height;
  double brightness = 0.0;

  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      sum += (unsigned char)img->imageData[img->widthStep * y + x * 3 + 0] * 0.114478
            + (unsigned char)img->imageData[img->widthStep * y + x * 3 + 1] * 0.586611
            + (unsigned char)img->imageData[img->widthStep * y + x * 3 + 2] * 0.298912;
    }
  }
  brightness = (double)sum / npixel;

  param->mean = brightness;
}


double calcDistance(Param* a, Param* b){
  double r;

  r = fabsf(a->mean - b->mean);

  return r;
}

Param* calcCharParam(CvSize* size){

  IplImage* img = NULL;
  char fn[256];
  Param* param = (Param *)malloc(sizeof(Param) * strlen(useChar));

  for (int i = 0; i < strlen(useChar); i++) {
    sprintf_s(fn, "%s/%c.bmp", charImgPath, useChar[i]);

    if ((img = cvLoadImage(fn, CV_LOAD_IMAGE_COLOR)) == NULL) {  // (�O���C�X�P�[���ł����Ă�)�J���[�摜�Ƃ��ēǂݍ���
      printf("�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n");
      return NULL;
    }

    calcParam(&param[i], img);
    printf("%s : %f\n", fn, param[i].mean);
  }

  size->width = img->width;
  size->height = img->height;


  // ------------------ �l���0-255�Ɋg��(�������͖����Ă������܂�)
  double maxV = param[0].mean;
  double minV = param[0].mean;

  for (int i = 0; i < strlen(useChar); i++) {
    minV = (minV < param[i].mean) ? minV : param[i].mean;
    maxV = (maxV > param[i].mean) ? maxV : param[i].mean;
  }

  for (int i = 0; i < strlen(useChar); i++) {
    param[i].mean = (param[i].mean - minV) / (maxV - minV) * 255;
  }

  return param;
}

Param* calcImgparam(CvSize* outSize, IplImage* img1, CvSize size){

  outSize->width = img1->width / size.width + 1;
  outSize->height = img1->height / size.height + 1;
  Param* paramImg = (Param *)malloc(sizeof(Param) * outSize->width * outSize->height);

  IplImage* subImg = cvCreateImage(size, img1->depth, img1->nChannels);

  for (int i = 0; i < outSize->height; i++) {
    for (int j = 0; j < outSize->width; j++) {
      subImage(subImg, img1, j * size.width, i * size.height, size.width, size.height);
      calcParam(&paramImg[i * outSize->width + j], subImg);
    }
  }

  return paramImg;
}


char* genString(Param* paramChar, Param* paramImg, CvSize outSize) {
  char* outStr = (char *)malloc(sizeof(char) * ((outSize.width + 1) * outSize.height + 1)); // outSize.width �u+1�v �͉��s���A�Ō�́u+1�v�� '\0'��

  double min;
  int minInd;
  int x, y;

  for (y = 0; y < outSize.height; y++) {
    for (x = 0; x < outSize.width; x++) {
      min = calcDistance(&paramChar[0], &paramImg[y * outSize.width + x]);
      minInd = 0;
      for (int i = 1; i < strlen(useChar); i++) {
        if (min > calcDistance(&paramChar[i], &paramImg[y * outSize.width + x])){
          min = calcDistance(&paramChar[i], &paramImg[y * outSize.width + x]);
          minInd = i;
        }
      }
      outStr[y * (outSize.width + 1) + x] = useChar[minInd];
    }
    outStr[y * (outSize.width + 1) + outSize.width] = '\n';
  }
  outStr[outSize.height * (outSize.width + 1)] = '\0';

  return outStr;
}


// �t�@�C���ւ̌��ʏ����o��
void writeFile(char *outStr, Param* paramImg, CvSize outSize, char* imgFilename) {
  FILE *fp;
  char filename[256];
  char *ext_p;

  strcpy_s(filename, 256, imgFilename); // �ǂݍ��񂾉摜�̃t�@�C�������R�s�[
  ext_p = strrchr(filename, '.');
  *ext_p = '\0';   // �t�@�C��������g���q������

  strcat_s(filename, ".txt");
  if (fopen_s(&fp, filename, "w") == 0) {   // fopen()���Ɠ{����̂� fopen_s()�ɁB�Ԃ�l�̎d�l���Ⴄ�̂Œ��ӁB
    fprintf_s(fp, "%s", outStr);
    fclose(fp);
  }
}