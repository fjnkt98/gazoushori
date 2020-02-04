#include <stdio.h>
#include <opencv/highgui.h>

// �R�[���o�b�N�֐��̃v���g�^�C�v�錾
void trackCallback(int pos);

IplImage* img;
IplImage* img2;

void main(int argc, char* argv[])
{
  // �N���I�v�V�����̃`�F�b�N
  printf("argc = %d\n", argc);
  for (int k = 0; k < argc; k++) {
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc > 1 && (img = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) != NULL) {

    img2 = cvCloneImage(img); // �摜�f�[�^���R�s�[

    int pos = 100;

    cvNamedWindow("p02-3_Trackbar"); // �E�C���h�E�̐���
    cvCreateTrackbar("���邳", "p02-3_Trackbar", &pos, 200, trackCallback);
    cvShowImage("p02-3_Trackbar", img2); // �摜�̕\��

    // ----------------------------------------
    cvWaitKey(0); // �L�[���������܂őҋ@

    cvDestroyAllWindows(); // �S�E�C���h�E�̔j��
    cvReleaseImage(&img);
    cvReleaseImage(&img2);
  }
  else
    printf("�t�@�C�������w�肵�Ă��������B\n");
}

void trackCallback(int pos){
  printf("%d%%\n", pos);

  int p;
  for (int x = 0; x < img->width; x++) {
    for (int y = 0; y < img->height; y++) {
      p = (unsigned char)img->imageData[img->widthStep * y + x] * pos / 100;
      if (p>255) p = 255;

      img2->imageData[img->widthStep * y + x] = p;
    }
  }
  cvShowImage("p02-3_Trackbar", img2);
}