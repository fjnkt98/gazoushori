#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>

// ���[�p�X�t�B���^�W��
double iir_coefficient = 0.0;

void trackbarCb(int coefficient);

// IIR�t�B���^(�f�B�W�^�����[�p�X�t�B���^)
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
  CvCapture *capture = NULL;      // �r�f�I�L���v�`���\����

  int c;
  int n = 0;
  IplImage *grab = NULL;          // cvQueryFrame() �̕Ԃ�l���ꎞ�I�ɕۑ����邽�߂̕ϐ�
  IplImage *frame = NULL;         // ���݂̃t���[��(�����̕ϐ��̓��e�����������Ă͂����Ȃ�)
  IplImage *outputFrame = NULL;   // �\������摜(�摜�������ʂ�����)
  IplImage *oldframe = NULL;      // ��O�̃t���[��
  IplImage *oldOutput = NULL;     // ��O�̏o�̓t���[��

  int mode = 0;                   // �N�����̃��[�h
  int width = 640;                // �f�t�H���g�̉摜�T�C�Y
  int height = 480;

  int pos = 0;  // �g���b�N�o�[�ϐ�

  // �N���I�v�V�����̐��� 0 (argc==1)�̏ꍇ�@�c �L���v�`���T�C�Y: 640x480�A�J�����ԍ�:0
  // �N���I�v�V�����̐��� 1 (argc==2)�̏ꍇ�@�c �w�肵������t�@�C����ǂݍ���
  // �N���I�v�V�����̐��� 2 (argc==3)�̏ꍇ�@�c �L���v�`���T�C�Y: arg[1]�~arg[2]�A�J�����ԍ�:0
  // �N���I�v�V�����̐��� 3 (argc==4)�̏ꍇ�@�c �L���v�`���T�C�Y: arg[1]�~arg[2]�A�J�����ԍ�:arg[3]
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

  // �捞�T�C�Y�̐ݒ�
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width);
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height);

  // �E�B���h�E�̕\��
  cvNamedWindow("Output");
  // �g���b�N�o�[�̐���
  cvCreateTrackbar("�W��", "Output", &pos, 100, trackbarCb);

  // �J�����̏������ł���܂ő҂�
  while (cvQueryFrame(capture) == NULL){
    c = cvWaitKey(2);    // esc ���������ꍇ�͏I��(���݂��Ȃ��J�����ԍ����w�肵���ꍇ�Ȃǂ̂���)
    if (c == '\x1b') {
      return -1;
    }
  }

  // ���݂̃t���[�����L���v�`�����Aframe�ɃR�s�[(�����t�H�[�}�b�g�̉摜�𐶐����ď�����)
  frame = cvCloneImage(cvQueryFrame(capture));
  outputFrame = cvCloneImage(frame);  // �L���v�`���摜�Ɠ����t�H�[�}�b�g�̉摜�𐶐����ď�����
  oldframe = cvCloneImage(frame);     // �L���v�`���摜�Ɠ����t�H�[�}�b�g�̉摜�𐶐����ď�����
  oldOutput = cvCloneImage(frame);

  // ���摜�����̃��[�v ========================================
  while ((grab = cvQueryFrame(capture)) != NULL) {   // NULL���Ԃ����ꍇ(����t�@�C���̏ꍇ�͖����ɒB�����ꍇ)�͏I��
    // �J��������摜�t���[�����捞��(grab)�A��Ɨp�̕ϐ�(frame)�ɃR�s�[
    cvCopy(grab, frame);  // ��grab �̒��g�͏��������Ă͂����܂���!!

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
      iir(frame, oldOutput, outputFrame);  // IIR�t�B���^
      break;
    case 0:
    default:
      cvCopy(frame, outputFrame);   // �L���v�`���摜�����̂܂ܕ\��
      break;
    }

    // �������ʂ̕\��
    cvShowImage("Output", outputFrame);

    //�L�[���͑҂�
    c = cvWaitKey(2);
    if (c == '\x1b') {   // esc�L�[�ŏI��
      break;
    }
    else if (c >= '0' && c <= '9') {  // 0�`9 ���������ꍇ�ɁAmode = 0�`9 �ɃZ�b�g
      mode = c - '0';     // ��ASCII�R�[�h�̕��т��Q��
      printf("mode = %d\n", mode);
    }
    else if (c == ' ') {   // �摜�t�@�C���Ƃ��ĕۑ�
      n++;
      char buf[10];
      sprintf_s(buf, 10, "%d.png", n);
      cvSaveImage(buf, outputFrame);
      puts(buf);
    }

    cvCopy(frame, oldframe);  // ��O�̃t���[���Ƃ��ĕۑ�
    cvCopy(outputFrame, oldOutput);
  }

  // ��n��
  cvReleaseCapture(&capture);
  cvDestroyWindow("Output");
}

void trackbarCb(int coefficient){
  iir_coefficient = (double)coefficient / 100;
}