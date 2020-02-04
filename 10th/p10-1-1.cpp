#include <stdio.h>
#include <opencv/highgui.h>


IplImage *complexInput;          // cvDFT()�̓��͂̂��߂ɁA���͉摜�ɋ�����(=0)��t�����������f���摜

CvMat *dft_img;                  // DFT����(���f��)
CvMat *dft_img_masked;           // DFT����(dft_img)���� mask_img_shift �ɂ���Ĉꕔ�𒊏o��������

IplImage *image_Re;              // DFT���ʂ̎����B(���̌�DFT��Βl�ɏ�����������)
IplImage *image_Re_masked;       // DFT�̐�Βl�摜��ɁA�}�E�X�ō쐬���̃}�X�N�̏��(mask_img)��ԐF�ŕ\�����邽�߂̃J���[�摜
IplImage *image_Re_bgr;          // DFT�̐�Βl�摜�Bimage_Re_masked�̏�����Ԃł�����B
// ���ۂ̓O���C�X�P�[���摜�����AcvCopy()��image_Re_masked�ɃR�s�[���s�����߂ɁA�J���[�摜�Ƃ��ĕێ����Ă���B

IplImage *mask_img;              // DFT �摜�̃}�X�N(0=�s�g�p,0�ȊO=�g�p)
IplImage *mask_img_shift;        // mask_img �̑�1�ی�<==>��3�ی�, ��2�ی�<==>��4�ی� �����ւ������́BcvDFT()�̈����͂��̌`���œ����K�v������

IplImage *idft_img;              // �t���U�t�[���G�ϊ�(IDFT)�̌���
IplImage *idft_img_8U;           // �t���U�t�[���G�ϊ�(IDFT)�̌��ʂ�\�����邽�߂ɁA8bit�O���C�X�P�[���摜�ɕϊ���������


int pen = 5;        // �}�E�X�Ń}�X�N���쐬����ۂɁACtrl�������Ă��鎞�̃y���̃T�C�Y


/* �v���g�^�C�v�錾 */
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr);  // �摜�̑�1�ی�<==>��3�ی�, ��2�ی�<==>��4�ی� �����ւ��� 
void onMouse(int event, int x, int y, int flags, void *param); // �}�E�X���쎞�̃R�[���o�b�N�֐�

void fillImage(int x, int y);      // DFT�摜���� x,y ���W�_���u�g�p����v(�}�X�N�摜 mask_image, image_Re_masked ��(x,y)�_��ON�ɂ��� )
void showIdft();                   // DFT�摜���}�X�N�������ʂ���AIDFT���s���A��ԉ摜�Ƃ��ĕ\������

void setMaskImg();               // DFT�摜�̃}�X�N mask_img ��ݒ肷��B(0:�s�g�p, 0�ȊO:�g�p)
void showMaskImg();                // mask_img �̏�Ԃ�\������



// DFT�摜���� x,y ���W�_���u�g�p����v(�}�X�N�摜 mask_image, image_Re_masked ��(x,y)�_��ON�ɂ��� )
void fillImage(int x, int y) {
  if (x > mask_img->width - 1) x = mask_img->width - 1;
  if (x < 0) x = 0;
  if (y > mask_img->height - 1) y = mask_img->height - 1;
  if (y < 0) y = 0;

  // �}�X�N��ON��
  mask_img->imageData[y * mask_img->widthStep + x] = 255;

  // �}�X�N�̏�Ԃ�\������image_Re_masked��ݒ�B(�g�p������ԐF(B,G,R)=(0,0,255)�ɂ���)
  image_Re_masked->imageData[y * image_Re_masked->widthStep + x *image_Re_masked->nChannels + 0] = 0;
  image_Re_masked->imageData[y * image_Re_masked->widthStep + x *image_Re_masked->nChannels + 1] = 0;
  image_Re_masked->imageData[y * image_Re_masked->widthStep + x *image_Re_masked->nChannels + 2] = 255;
}


// DFT�摜���}�X�N�������ʂ���AIDFT���s���A��ԉ摜�Ƃ��ĕ\������
void showIdft() {
  cvShiftDFT(mask_img, mask_img);                    // ��1�ی�<==>��3�ی�, ��2�ی�<==>��4�ی� �����ւ���(cvDFT()�̈����̎d�l�ɍ��킹��)
  cvSetZero(dft_img_masked);                         // �������� 0 �ŏ�����
  cvCopy(dft_img, dft_img_masked, mask_img);         // mask_img ��ON�̕���(��0�̕���)��dft_img�A����ȊO��0�� DFT �摜 dft_img_masked �𐶐�
  cvShiftDFT(mask_img, mask_img);                    // mask_img �����ɖ߂�

  cvDFT(dft_img_masked, idft_img, CV_DXT_INVERSE_SCALE, complexInput->height);  // dft_img_masked ���t�t�[���G�ϊ�
  cvConvertScaleAbs(idft_img, idft_img_8U, 1.0, 0);  // ���ʕ\���p�� 8bit�O���C�X�P�[���摜 idft_img_8U �ɕϊ�
  cvShowImage("IDFT", idft_img_8U);                  // ���ʂ�\��

}


// �}�E�X�̃R�[���o�b�N�֐�
void onMouse(int event, int x, int y, int flags, void *param){

  unsigned char size = pen;

  if (flags & CV_EVENT_FLAG_LBUTTON) {    // ���{�^����������Ă��āA
    if (flags & CV_EVENT_FLAG_CTRLKEY) {               // ����Ctrl�L�[��������Ă��鎞
      for (int j = y - size; j < y + size; j++) {
        for (int i = x - size; i < x + size; i++) {
          fillImage(i, j);
        }
      }
    }
    else {                                           // Ctrl�L�[��������Ă��Ȃ���
      fillImage(x, y);
    }
    cvShowImage("Magnitude", image_Re_masked);

    showIdft();    // ���݂̃}�X�N�ł� IDFT ���ʂ�\������

  }


  switch (event) {
  case CV_EVENT_RBUTTONDOWN:             // �E�{�^�����������Ƃ�
    cvSet(mask_img, cvScalar(0));            // �}�X�N��������
    cvCopy(image_Re_bgr, image_Re_masked);         // �\������idft�摜��������Ԃɂ��ǂ�
    cvShowImage("Magnitude", image_Re_masked);
    showIdft();
    break;
  }

}


// DFT�摜�̃}�X�N mask_img ��ݒ肷��B(0:�s�g�p, 0�ȊO:�g�p)
// ��mode = 1 �̏ꍇ�̂ݎg�p
void setMaskImg() {
  int wsize = 50;

  cvSetZero(mask_img);   // 0�ł������񏉊���(�S�Ďg�p���Ȃ�)

  // ���̃T���v���́A���[�p�X�t�B���^�[�I�ȓ���ƂȂ�
  for (int y = mask_img->height / 2 - wsize; y < mask_img->height / 2 + wsize; y++) {
    for (int x = mask_img->width / 2 - wsize; x < mask_img->width / 2 + wsize; x++){
      mask_img->imageData[y * mask_img->widthStep + x] = 255;
    }
  }

  // ���]
  for (int y = 0; y < mask_img->height; y++){
    for (int x = 0; x < mask_img->width; x++){
      mask_img->imageData[y * mask_img->widthStep + x] = 255 - (unsigned char)mask_img->imageData[y * mask_img->widthStep + x];
    }
  }
}


// mask_img �̏�Ԃ�\������
// ��mode = 1 �̏ꍇ�̂ݎg�p
void showMaskImg() {
  cvNamedWindow("Mask Pattern");
  cvShowImage("Mask Pattern", mask_img);
}



int main(int argc, char **argv)
{
  int mode = 1;  // mode = 0: �}�E�X�ɂ��C���^���N�e�B�u�ȃ}�X�N�쐬
  // mode = 1: setMaskImg() �Ń}�X�N���쐬
  // �����̒l�̓R�}���h���C�������Ŏw�肵���ق����ǂ�����

  IplImage *src_img;
  IplImage *realInput;
  IplImage *imaginaryInput;
  IplImage *image_Im;
  IplImage *image_Re_8U;
  int dft_M, dft_N;
  CvMat tmp;
  double m, M;

  src_img = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  if (!src_img)
    return -1;

  realInput = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_64F, 1);
  imaginaryInput = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_64F, 1);
  complexInput = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_64F, 2);

  // (1)���͉摜�������z��ɃR�s�[���C�����z��ƃ}�[�W���ĕ��f�����ʂ��\��
  cvScale(src_img, realInput, 1.0, 0.0);
  cvZero(imaginaryInput);
  cvMerge(realInput, imaginaryInput, NULL, NULL, complexInput);

  // (2)DFT�p�̍œK�T�C�Y���v�Z���C���̃T�C�Y�ōs����m�ۂ���
  dft_M = cvGetOptimalDFTSize(src_img->height - 1);
  dft_N = cvGetOptimalDFTSize(src_img->width - 1);
  dft_img = cvCreateMat(dft_M, dft_N, CV_64FC2);
  dft_img_masked = cvCreateMat(dft_M, dft_N, CV_64FC2);
  image_Re = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_64F, 1);
  image_Im = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_64F, 1);

  image_Re_masked = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_8U, 3);   // bgr�J���[�摜


  mask_img = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_8U, 1);
  mask_img_shift = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_8U, 1);


  // (3)���f�����ʂ�dft_img�ɃR�s�[���C�c��̍s��E��������0�Ŗ��߂�
  cvGetSubRect(dft_img, &tmp, cvRect(0, 0, src_img->width, src_img->height));

  cvCopy(complexInput, &tmp, NULL);
  if (dft_img->cols > src_img->width) {
    cvGetSubRect(dft_img, &tmp, cvRect(src_img->width, 0, dft_img->cols - src_img->width, src_img->height));
    cvZero(&tmp);
  }

  // (4)���U�t�[���G�ϊ����s���C���̌��ʂ����������Ƌ��������ɕ���
  cvDFT(dft_img, dft_img, CV_DXT_FORWARD, complexInput->height);
  cvSplit(dft_img, image_Re, image_Im, 0, 0);


  /*
  // �ʑ������߂�
  IplImage *image_Phase = cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_8U, 1);

  for (int y = 0; y < image_Re->height; y++) {
  for (int x = 0; x < image_Re->width; x++) {
  image_Phase->imageData[y * image_Phase->widthStep + x] = atan2(image_Re->imageData[y*image_Re->widthStep + x], image_Im->imageData[y*image_Im->widthStep + x])/CV_PI/2*255+127;
  }
  }
  cvSaveImage("DFT_Phase.png", image_Phase);
  cvShiftDFT(image_Phase, image_Phase);
  cvSaveImage("DFT_Phase_Shifted.png", image_Phase);
  */



  // (5)�X�y�N�g���̐�Βl(�U��)���v�Z Mag = sqrt(Re^2 + Im^2)
  cvPow(image_Re, image_Re, 2.0);
  cvPow(image_Im, image_Im, 2.0);
  cvAdd(image_Re, image_Im, image_Re, NULL);
  cvPow(image_Re, image_Re, 0.5);

  // (6)�U���̑ΐ����Ƃ� log(1 + Mag)   �c �l�̃����W���L���̂ŁA���₷�����邽��
  cvAddS(image_Re, cvScalarAll(1.0), image_Re, NULL);  // �S�v�f��1�𑫂�
  cvLog(image_Re, image_Re);

  // (7)���_�i���������j���摜�̒��S�ɂ���悤�ɁC�摜�̏ی������ւ���
  cvShiftDFT(image_Re, image_Re);

  // (8)�U���摜�̃s�N�Z���l��0.0-1.0�ɕ��z����悤�ɃX�P�[�����O
  cvMinMaxLoc(image_Re, &m, &M, NULL, NULL, NULL);
  cvScale(image_Re, image_Re, 1.0 / (M - m), 1.0 * (-m) / (M - m));

  image_Re_8U = cvCreateImage(cvGetSize(image_Re), IPL_DEPTH_8U, 1);
  image_Re_bgr = cvCreateImage(cvGetSize(image_Re), IPL_DEPTH_8U, 3);
  cvConvertScaleAbs(image_Re, image_Re_8U, 255, 0);

  for (int y = 0; y < image_Re->height; y++) {
    for (int x = 0; x < image_Re->width; x++){
      image_Re_bgr->imageData[y * image_Re_bgr->widthStep + x * 3 + 0] = (unsigned char)image_Re_8U->imageData[y * image_Re_8U->widthStep + x];
      image_Re_bgr->imageData[y * image_Re_bgr->widthStep + x * 3 + 1] = (unsigned char)image_Re_8U->imageData[y * image_Re_8U->widthStep + x];
      image_Re_bgr->imageData[y * image_Re_bgr->widthStep + x * 3 + 2] = (unsigned char)image_Re_8U->imageData[y * image_Re_8U->widthStep + x];
    }
  }

  cvSet(mask_img, cvScalar(0));                       // �}�X�N��������
  cvCopy(image_Re_bgr, image_Re_masked);    // �\������idft�摜��������Ԃ�


  cvNamedWindow("Original");
  cvShowImage("Original", src_img);
  cvNamedWindow("Magnitude");
  cvShowImage("Magnitude", image_Re_bgr);



  cvSaveImage("DFT_Shifted.png", image_Re_8U);

  cvShiftDFT(image_Re_8U, image_Re_8U);  // ���ɏی��̓���ւ���Ȃ̂ŁA��������ēx����ւ��邱�ƂŁA����ւ��O�̏�Ԃ�
  cvSaveImage("DFT.png", image_Re_8U);
  cvShiftDFT(image_Re_8U, image_Re_8U);  // ���ɖ߂��Ă���



  ///////
  idft_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_64F, 1);
  idft_img_8U = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
  cvDFT(dft_img, idft_img, CV_DXT_INVERSE_SCALE, complexInput->height);      // IDFT �����s
  cvConvertScaleAbs(idft_img, idft_img_8U, 1.0, 0);         // �\���̂��߂� 8bit �O���C�X�P�[���摜�ɕϊ�

  cvNamedWindow("IDFT");
  cvShowImage("IDFT", idft_img_8U);



  if (mode == 0) {   // mode==0 �̏ꍇ�́A�}�E�X�Ń}�X�N���쐬���郂�[�h�ƂȂ�
    cvSetMouseCallback("Magnitude", onMouse, (void *)0);
    cvCreateTrackbar("Pen size", "Magnitude", &pen, 100);
  }
  else {             // mode==1 �̏ꍇ�́AsetMasImg() �֐��Ń}�X�N�𐶐����A���ʂ�\�����ďI��
    setMaskImg();
    showMaskImg();
  }

  showIdft();    // mask_img �ɂ���ă}�X�N����DFT�摜 dft_img ������ IDFT ���s�������ʂ�\������ 
  ////////

  cvWaitKey(0);

  return 0;
}

/* ���_�i���������j���摜�̒��S�ɂ���悤�ɁC�摜�̏ی������ւ���֐��D
src_arr, dst_arr �͓����T�C�Y�C�^�C�v�̔z�� */
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr)
{
  CvMat *tmp = 0;
  CvMat q1stub, q2stub;
  CvMat q3stub, q4stub;
  CvMat d1stub, d2stub;
  CvMat d3stub, d4stub;
  CvMat *q1, *q2, *q3, *q4;
  CvMat *d1, *d2, *d3, *d4;

  CvSize size = cvGetSize(src_arr);
  CvSize dst_size = cvGetSize(dst_arr);
  int cx, cy;

  if (dst_size.width != size.width || dst_size.height != size.height) {
    cvError(CV_StsUnmatchedSizes, "cvShiftDFT", "Source and Destination arrays must have equal sizes", __FILE__,
      __LINE__);
  }
  // (9)�C���v���[�X���[�h�p�̃e���|�����o�b�t�@
  if (src_arr == dst_arr) {
    tmp = cvCreateMat(size.height / 2, size.width / 2, cvGetElemType(src_arr));
  }
  cx = size.width / 2;          /* �摜���S */
  cy = size.height / 2;

  // (10)1?4�ی���\���z��ƁC���̃R�s�[��
  q1 = cvGetSubRect(src_arr, &q1stub, cvRect(0, 0, cx, cy));
  q2 = cvGetSubRect(src_arr, &q2stub, cvRect(cx, 0, cx, cy));
  q3 = cvGetSubRect(src_arr, &q3stub, cvRect(cx, cy, cx, cy));
  q4 = cvGetSubRect(src_arr, &q4stub, cvRect(0, cy, cx, cy));
  d1 = cvGetSubRect(src_arr, &d1stub, cvRect(0, 0, cx, cy));
  d2 = cvGetSubRect(src_arr, &d2stub, cvRect(cx, 0, cx, cy));
  d3 = cvGetSubRect(src_arr, &d3stub, cvRect(cx, cy, cx, cy));
  d4 = cvGetSubRect(src_arr, &d4stub, cvRect(0, cy, cx, cy));

  // (11)���ۂ̏ی��̓���ւ�
  if (src_arr != dst_arr) {
    if (!CV_ARE_TYPES_EQ(q1, d1)) {
      cvError(CV_StsUnmatchedFormats, "cvShiftDFT", "Source and Destination arrays must have the same format",
        __FILE__, __LINE__);
    }
    cvCopy(q3, d1, 0);
    cvCopy(q4, d2, 0);
    cvCopy(q1, d3, 0);
    cvCopy(q2, d4, 0);
  }
  else {                        /* �C���v���[�X���[�h */
    cvCopy(q3, tmp, 0);
    cvCopy(q1, q3, 0);
    cvCopy(tmp, q1, 0);
    cvCopy(q4, tmp, 0);
    cvCopy(q2, q4, 0);
    cvCopy(tmp, q2, 0);
  }
}