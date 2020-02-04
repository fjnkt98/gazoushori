#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>

#define N_FRAME 3

IplImage* historyFrame[N_FRAME];      // ���[�V�����u���[�p�̉摜

// �摜�t���[���ւ̐}�`�`��
void drawShape(IplImage *frame, IplImage *outputFrame){
  cvCopy(frame, outputFrame);
  cvLine(outputFrame, cvPoint(10, 10), cvPoint(100, 100), cvScalar(0, 0, 255), 5, 1);             // ����
  cvRectangle(outputFrame, cvPoint(110, 110), cvPoint(200, 200), cvScalar(255, 0, 255), 5);        // �l�p�`
  cvRectangle(outputFrame, cvPoint(210, 110), cvPoint(300, 220), cvScalar(0, 255, 0), -1);        // �l�p�`(�����h��Ԃ�)
  cvCircle(outputFrame, cvPoint(160, 120), 30, cvScalar(0, 255, 255), 2);                         // �~
  cvEllipse(outputFrame, cvPoint(120, 80), cvSize(60, 30), 15, 0, 270, cvScalar(255, 255, 0), 2); // �ȉ~
}

// ��f�l�̓���ւ�
void swap(char* p1, char *p2) {
  char tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

// �摜�t���[���̍��E����ւ�(����)
void mirror(IplImage *frame, IplImage *outputFrame){
  cvCopy(frame, outputFrame);
  for (int y = 0; y < outputFrame->height; y++) {
    for (int x = 0; x < outputFrame->width; x++) {
      for (int ch = 0; ch < outputFrame->nChannels; ch++) {
        outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch] = (unsigned char)frame->imageData[y * frame->widthStep + (frame->width - x)  * frame->nChannels + ch];
      }
    }
  }
}

// �摜�t���[���̍��E�Ώۉ摜�̐���
void mirror2(IplImage *frame, IplImage *outputFrame){
  cvCopy(frame, outputFrame);
  for (int y = 0; y < outputFrame->height; y++) {
    for (int x = outputFrame->width/2; x < outputFrame->width; x++) {
      for (int ch = 0; ch < outputFrame->nChannels; ch++) {
        outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch] = (unsigned char)frame->imageData[y * frame->widthStep + (frame->width - x)  * frame->nChannels + ch];
      }
    }
  }
}

// 1���C�������ɍ��E���]
void scrumble(IplImage *frame, IplImage *outputFrame){
  cvCopy(frame, outputFrame);
  for (int y = 0; y < outputFrame->height - 1; y += 2) {
    for (int x = 0; x < outputFrame->width / 2; x++) {
      for (int ch = 0; ch < outputFrame->nChannels; ch++) {
        swap(&outputFrame->imageData[y * outputFrame->widthStep + (outputFrame->width - x) * outputFrame->nChannels + ch]
          , &outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch]);
      }
    }
  }
}

// R�`���l���̂ݒ��o
void chR(IplImage *frame, IplImage *outputFrame){
  cvCopy(frame, outputFrame);
  for (int y = 0; y < outputFrame->height; y++) {
    for (int x = 0; x < outputFrame->width; x++) {
      outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + 0] = 0;
      outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + 1] = 0;
    }
  }
}


// �J���[�`���l���̓���ւ�
void chShift(IplImage *frame, IplImage *outputFrame){
  for (int y = 0; y < frame->height; y++) {
    for (int x = 0; x < frame->width; x++) {
      for (int ch = 0; ch < frame->nChannels; ch++) {
        outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch]
          = frame->imageData[y * frame->widthStep + x * frame->nChannels + ((ch + 1) % 3)];
      }
    }
  }
}

// 0-255 �̒l�ɃN���b�v
unsigned char clip(int v) {
  return (v < 0) ? 0 : (v > 255) ? 255 : v;
}

// ���[�V�����u���[
void motionBlur(IplImage *frame, double w, IplImage *outputFrame){
  IplImage *p = historyFrame[N_FRAME - 1];

  for (int k = N_FRAME - 1; k > 0; k--) {     // �|�C���^���ЂƂ��炷
    historyFrame[k] = historyFrame[k - 1];
  }
  historyFrame[0] = p;    // ��Ԍ�����ԑO�Ɏ����Ă���(�|�C���^�̂Ȃ��ς�)
  cvCopy(frame, historyFrame[0]);  // ��ԑO�� historyFrame �����݂̃t���[���ŏ㏑��

  cvSetZero(outputFrame);

  for (int k = 0; k < N_FRAME; k++) {
    for (int y = 0; y < outputFrame->height; y++) {
      for (int x = 0; x < outputFrame->width; x++) {
        for (int ch = 0; ch < outputFrame->nChannels; ch++) {
          outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch]
            = clip((unsigned char)outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch]
            + (unsigned char)historyFrame[k]->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch] / N_FRAME);
        }
      }
    }
  }
}


// �w�肵��x,y ���摜���ł���΂��̋P�x�l��Ԃ��A�摜�O�ł����0��Ԃ�   
int chk(IplImage *frame, int x, int y) {

  // �摜�O�̏ꍇ�� 0 ��Ԃ�
  if (x > frame->width - 1 || x < 0 || y > frame->height - 1 || y < 0) return 0;

  // �P�x�l��Ԃ�
  return 0.298912 * (unsigned char)frame->imageData[y * frame->widthStep + x * frame->nChannels + 2]
    + 0.586611 * (unsigned char)frame->imageData[y * frame->widthStep + x * frame->nChannels + 1]
    + 0.114478 * (unsigned char)frame->imageData[y * frame->widthStep + x * frame->nChannels + 0];
}

// �摜���ɓ����{�[��������
void ball(IplImage *frame, IplImage *outputFrame){
  // �����l��K���Ɏw��
  // (��static �C���q��������̂́A�֐��𔲂��Ă��l���ێ������B�܂��ŏ��Ɉ�x���������������)
  static float x = frame->width / 2;
  static float y = 30;
  static float xx = 8;  // x�����̉����x
  static float yy = 0;  // y�����̉����x
  const float g = 9.8;  // �d�͉����x
  const float m = 0.1;  // ����
  const int r = 10;     // �{�[���̔��a

  int next_x = x + xx;  // ����x���W
  int next_y = y + yy;  // ����y���W

  mirror(frame, outputFrame);   // ���삵�₷���悤�ɁA�摜�������ɂ��Ă���
  //cvCopy(frame, outputFrame); // �����ɂ��Ȃ��ꍇ�͂�����ɒu��������

  // ��ʂ��͂ݏo���ꍇ�̏���
  if (next_x > outputFrame->width) next_x = 0;
  if (next_x < 0) next_x = outputFrame->width - 1;
  if (next_y > outputFrame->height) next_y = outputFrame->height - 1;
  if (next_y < 0) next_y = 0;


  // �e�������˕Ԃ�ꍇ�̏���
  if (chk(outputFrame, next_x, next_y + r) < 128){
    yy = -abs(yy)*0.98;
  }

  if (chk(outputFrame, next_x + r, next_y) < 128){
    xx = -abs(xx);
  }
  else if (chk(outputFrame, next_x - r, next_y) < 128){
    xx = abs(xx);
  }

  x = next_x;   // x���W�̍X�V
  y = next_y;   // y���W�̍X�V
  yy += m*g;    // y�����x�̍X�V

  cvCircle(outputFrame, cvPoint(x, y), r, cvScalar(0, 255, 0), -1);
}

// 0-255�ɃN���b�v����B�������A���̒l�͐�Βl�Ƃ���
unsigned char clipAbs(int v) {
  return (v < 0) ? -v : (v > 255) ? 255 : v;
}

// �����摜�̐��� 
void diffim(IplImage *frame, IplImage *oldframe, IplImage *outputFrame) {
  for (int y = 0; y < frame->height; y++) {
    for (int x = 0; x < frame->width; x++) {
      for (int ch = 0; ch < frame->nChannels; ch++) {
        outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch]
          = clipAbs((unsigned char)frame->imageData[y * frame->widthStep + x * frame->nChannels + ch] - (unsigned char)oldframe->imageData[y * oldframe->widthStep + x * oldframe->nChannels + ch]);
      }
    }
  }
}

// �A�t�B���ϊ�
void affineTrans(IplImage *frame, IplImage *outputFrame){
  cvCopy(frame, outputFrame);

  double rotate = CV_PI / 4.0;
  double skew = 0;
  double sx = double(outputFrame->width / frame->width);
  double sy = double(outputFrame->height / frame->height);
  double dx = 0;
  double dy = 0;
  int x0 = frame->width / 2;
  int y0 = frame->height / 2;
  int u0 = outputFrame->width / 2;
  int v0 = outputFrame->height / 2;

  double affine_matrix[2][3];

  affine_matrix[0][0] = sx * cos(rotate);
  affine_matrix[0][1] = sx * sin(rotate);
  affine_matrix[0][2] = sx * dx * cos(rotate) + sx * dy * sin(rotate);
  affine_matrix[1][0] = tan(skew) * cos(rotate) - sy * sin(rotate);
  affine_matrix[1][1] = tan(skew) * sin(rotate) + sy * cos(rotate);
  affine_matrix[1][2] = tan(skew) * -(dx * cos(rotate) + dy * sin(rotate)) + sy * sy * (-dx * sin(rotate) + dy * cos(rotate));

  int x, y;

  for (int v = 0; v < outputFrame->height; v++) {
    for (int u = 0; u < outputFrame->width; u++) {
      for (int ch = 0; ch < frame->nChannels; ch++){
        x = (affine_matrix[1][1] * (u - u0 - affine_matrix[0][2]) - affine_matrix[0][1] * (v - v0 - affine_matrix[1][2])) / (affine_matrix[0][0] * affine_matrix[1][1] - affine_matrix[0][1] * affine_matrix[1][0]) + x0 + 0.5;
        y = (-affine_matrix[1][0] * (u - u0 - affine_matrix[0][2]) + affine_matrix[0][0] * (v - v0 - affine_matrix[1][2])) / (affine_matrix[0][0] * affine_matrix[1][1] - affine_matrix[0][1] * affine_matrix[1][0]) + y0 + 0.5;

        if (x > frame->width || x < 0 || y > frame->height || y < 0){
          outputFrame->imageData[outputFrame->widthStep * v + u * outputFrame->nChannels + ch] = 0;
        }
        else{
          outputFrame->imageData[outputFrame->widthStep * v + u * outputFrame->nChannels + ch] = frame->imageData[frame->widthStep * y + x * frame->nChannels + ch];
        }
      }
    }
  }
}

// �摜�t���[���̍��E����ւ�(����)
void vmirror(IplImage *frame, IplImage *outputFrame){
  cvCopy(frame, outputFrame);
  for (int y = 0; y < outputFrame->height; y++) {
    for (int x = 0; x < outputFrame->width; x++) {
      for (int ch = 0; ch < outputFrame->nChannels; ch++) {
        outputFrame->imageData[y * outputFrame->widthStep + x * outputFrame->nChannels + ch] = (unsigned char)frame->imageData[(frame -> height - y) * frame->widthStep + x  * frame->nChannels + ch];
      }
    }
  }
}

// �^���J���[�摜
void pseudo(IplImage *frame, IplImage *outputFrame){
  cvCopy(frame, outputFrame);
  
  // �����̂��߂̃O���C�X�P�[���摜�쐬
  IplImage* gray;
  gray = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, 1);
  // �O���C�X�P�[���摜�̏�����
  for (int y = 0; y < gray->height; y++) {
    for (int x = 0; x < gray->width; x++) {
      gray->imageData[gray->widthStep * y + x] = (unsigned char)(0.298912 * (unsigned char)frame->imageData[frame->widthStep*y + x * 3 + 2]
                                                               + 0.586611 * (unsigned char)frame->imageData[frame->widthStep*y + x * 3 + 1]
                                                               + 0.114478 * (unsigned char)frame->imageData[frame->widthStep*y + x * 3 + 0]);
    }
  }

  for (int y = 0; y < frame->height; y++) {
    for (int x = 0; x < frame->width; x++) {
      if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 96){
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 2] = (unsigned char)gray->imageData[gray->widthStep * y + x] * 8 / 3;
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 1] = 0;
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 0] = 0;
      }
      else if ((unsigned char)gray->imageData[gray->widthStep * y + x] < 192){
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 2] = 255;
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 1] = (unsigned char)gray->imageData[gray->widthStep * y + x] * 8 / 3;
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 0] = 0;
      }
      else{
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 2] = 255;
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 1] = 255;
        outputFrame->imageData[outputFrame->widthStep * y + x * 3 + 0] = (unsigned char)gray->imageData[gray->widthStep * y + x] * 4;
      }
    }
  }
}

int main(int argc, char *argv[]){

  CvCapture *capture = NULL;          // �r�f�I�L���v�`���\����

  int c;
  int n = 0;
  IplImage *grab = NULL;            // cvQueryFrame() �̕Ԃ�l���ꎞ�I�ɕۑ����邽�߂̕ϐ�
  IplImage *frame = NULL;            // ���݂̃t���[��(�����̕ϐ��̓��e�����������Ă͂����Ȃ�)
  IplImage *outputFrame = NULL;       // �\������摜(�摜�������ʂ�����)
  IplImage *oldframe = NULL;          // ��O�̃t���[��

  int mode = 0;                       // �N�����̃��[�h
  int width = 640;                    // �f�t�H���g�̉摜�T�C�Y
  int height = 480;

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

  //�J�����̏������i�J�����̑I���j
  //   CvCapture *capture = cvCreateCameraCapture(0);
  //   CvCapture *capture = cvCaptureFromFile("C:\\Users\\kobayashi\\Desktop\\���[���̏������-5�C���`�Q�[�W�v���W�F�N�g.mp4");



  //�捞�T�C�Y�̐ݒ�
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width);
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height);

  //�E�B���h�E�̕\��
  //   cvNamedWindow("Capture");
  cvNamedWindow("Output");

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

  // ���[�V�����u���[�p�̉摜�Q�̏�����
  for (int k = 0; k < N_FRAME; k++){
    historyFrame[k] = cvCloneImage(frame);
  }



  // ���摜�����̃��[�v ========================================
  while ((grab = cvQueryFrame(capture)) != NULL) {   // NULL���Ԃ����ꍇ(����t�@�C���̏ꍇ�͖����ɒB�����ꍇ)�͏I��
    // �J��������摜�t���[�����捞��(grab)�A��Ɨp�̕ϐ�(frame)�ɃR�s�[
    cvCopy(grab, frame);  // ��grab �̒��g�͏��������Ă͂����܂���!!

    switch (mode) {
    case 1:
      chR(frame, outputFrame);  // �}�`�`��̃f��
      break;

    case 2:
      vmirror(frame, outputFrame);  // ����(�c)
      break;

    case 3:
      mirror2(frame, outputFrame);  // ����2
      break;

    case 4:
      affineTrans(frame, outputFrame); // �A�t�B���ϊ�
      break;

    case 5:
      pseudo(frame, outputFrame); // �^���J���[��
      break;

    case 6:
      mirror(frame, outputFrame);  // �J���[�`���l���̓���ւ�
      break;

    case 7:
      diffim(frame, oldframe, outputFrame);  // �����摜(��Βl)
      break;

    case 8:
      motionBlur(frame, 0.5, outputFrame);  // ���[�V�����u���[
      break;

    case 9:
      ball(frame, outputFrame);   // �����{�[���摜�̍���
      break;

    case 0:
    default:
      cvCopy(frame, outputFrame);   // �L���v�`���摜�����̂܂ܕ\��
      break;
    }

    // �������ʂ̕\��
    //      cvShowImage("Capture", frame);
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
  }

  // ��n��
  cvReleaseCapture(&capture);
  //   cvDestroyWindow("Capture");
  cvDestroyWindow("Output");
}