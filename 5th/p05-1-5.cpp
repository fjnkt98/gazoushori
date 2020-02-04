#include <stdio.h>
#include <opencv/highgui.h>

// �J���[�摜���O���C�X�P�[����  
void bgr2gray(IplImage* gray, IplImage* bgr) {
  for (int y = 0; y < gray->height; y++) {
    for (int x = 0; x < gray->width; x++) {
      gray->imageData[gray->widthStep * y + x] = (unsigned char)(0.298912 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 2]
                                                               + 0.586611 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 1]
                                                               + 0.114478 * (unsigned char)bgr->imageData[bgr->widthStep*y + x * 3 + 0]);
    }
  }
}

// �^���J���[��
void bgr2pseudo(unsigned char* bgr, unsigned char gray) {
  // COOL
  bgr[2] = 255 - gray;  // R�l
  bgr[1] = gray;        // G�l
  bgr[0] = 255;         // B�l
}

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* imgPseudo;
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
  imgGray = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 1);
  if (img1->nChannels != 1) {
    bgr2gray(imgGray, img1);
    cvNamedWindow("Gray");
    cvShowImage("Gray", imgGray);
  }
  else {
    cvCopy(img1, imgGray);
  }

  // �ǂݍ��񂾉摜�Ɠ����T�C�Y�̃J���[�摜�𐶐�
  imgPseudo = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, 3);

  // �^���J���[�摜�𐶐�
  for (int y = 0; y < imgPseudo->height; y++) {
    for (int x = 0; x < imgPseudo->width; x++) {
      bgr2pseudo((unsigned char *)&imgPseudo->imageData[imgPseudo->widthStep * y + x * 3], (unsigned char)imgGray->imageData[imgGray->widthStep * y + x]);
    }
  }

  cvNamedWindow("Pseudo Color");
  cvShowImage("Pseudo Color", imgPseudo);
  cvSaveImage("p05-1-5.bmp", imgPseudo);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&imgGray);
  cvReleaseImage(&imgPseudo);
}