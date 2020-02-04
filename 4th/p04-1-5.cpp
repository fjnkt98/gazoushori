// p04-2 : 1�̃J���[�`���l����0�ɂ����摜�𐶐�����
#include <stdio.h>
#include <opencv/highgui.h>

void main(int argc, char* argv[])
{
  IplImage* img1;
  IplImage* img2;

  printf("argc = %d\n", argc);                 // �N���I�v�V�����̃`�F�b�N
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

  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  printf("nChannels = %d\n", img1->nChannels);
  if (img1->nChannels != 3) {
    printf("�J���[�摜���w�肵�ĉ������B\n");
    return;
  }

  // �ǂݍ��񂾉摜�Ɠ����T�C�Y�̉摜�𐶐�
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);

  // R�`���l����0�� ----------------
  cvCopy(img1, img2);            // ���摜���R�s�[
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 1] = 0;
      img2->imageData[img2->widthStep*y + x * 3 + 3] = 0;
    }
  }
  cvNamedWindow("R");
  cvShowImage("R", img2);

  // G�`���l����0�� ----------------
  cvCopy(img1, img2);            // ���摜���R�s�[
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 0] = 0;
      img2->imageData[img2->widthStep*y + x * 3 + 2] = 0;
    }
  }
  cvNamedWindow("G");
  cvShowImage("G", img2);

  // B�`���l����0�� ----------------
  cvCopy(img1, img2);            // ���摜���R�s�[
  for (int y = 0; y < img2->height; y++) {
    for (int x = 0; x < img2->width; x++) {
      img2->imageData[img2->widthStep*y + x * 3 + 1] = 0;
      img2->imageData[img2->widthStep*y + x * 3 + 2] = 0;
    }
  }
  cvNamedWindow("B");
  cvShowImage("B", img2);


  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}