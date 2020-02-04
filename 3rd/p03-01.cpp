#include <stdio.h>
#include <opencv/highgui.h>

typedef struct {
  double a, b, c, d, e, f;
  int x0, y0, u0, v0;
} AffineParam;

void affineTransBasic(IplImage* input, IplImage* output, AffineParam param);
void affineTrans(IplImage* input, IplImage* output, AffineParam param);

void main(int argc, char* argv[]){
  IplImage* img1;
  IplImage* img2;
  
  AffineParam param;

  printf("argc = %d\n", argc);

  for (int k = 0; k < argc; k++){
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc < 2) {
    printf("Choose file name.");
    return;
  }

  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {
    printf("Failed to load image file.");
    return;
  }

  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);
  cvSetZero(img2);

  param.a = cos(CV_PI / 4.0);
  param.b = sin(CV_PI / 4.0);
  param.c = 0;
  param.d = -sin(CV_PI / 4.0);
  param.e = cos(CV_PI / 4.0);
  param.f = 0;
  param.x0 = img1->width / 2;
  param.y0 = img1->height / 2;
  param.u0 = img2->width / 2;
  param.v0 = img2->height / 2;

  if (argc > 2) {
    param.a = atof(argv[2]); param.b = atof(argv[3]); param.c = atof(argv[4]);
    param.d = atof(argv[5]); param.e = atof(argv[6]); param.f = atof(argv[7]);
  }
  if (argc > 11) {
    param.x0 = atoi(argv[8]); param.y0 = atoi(argv[9]);
    param.u0 = atoi(argv[10]); param.v0 = atoi(argv[11]);
  }
  else {
    param.x0 = img1->width / 2; param.y0 = img1->height / 2;
    param.u0 = img2->width / 2; param.v0 = img2->height / 2;
  }
  affineTransBasic(img1, img2, param);
  cvNamedWindow("Affine Transform 1");
  cvShowImage("Affine Transform 1", img2);
  cvSaveImage("basic_affine_transform.bmp", img2);

  affineTrans(img1, img2, param);
  cvNamedWindow("Affine Transform 2");
  cvShowImage("Affine Transform 2", img2);
  cvSaveImage("true_affine_transform.bmp", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}

void affineTransBasic(IplImage* input, IplImage* output, AffineParam param){
  int u, v;

  for (int y = 0; y < input->height; y++){
    for (int x = 0; x < input->width; x++){
      u = param.a * x + param.b * y + param.c + 0.5;
      v = param.d * x + param.e * y + param.f + 0.5;

      if (u < output->width && u > 0 && v < output->height && v > 0){
        output->imageData[output->widthStep * v + u] = input->imageData[input->widthStep * y + x];
      }
    }
  }
}

void affineTrans(IplImage* input, IplImage* output, AffineParam param){
  int x, y;

  for (int v = 0; v < output->height; v++){
    for (int u = 0; u < output->width; u++){
      x = (param.e * (u - param.u0 - param.c) - param.b * (v - param.v0 - param.f)) / (param.a * param.e - param.b * param.d) + param.x0 + 0.5;
      y = (-param.d * (u - param.u0 - param.c) + param.a * (v - param.v0 - param.f)) / (param.a * param.e - param.b * param.d) + param.y0 + 0.5;

      if (x > input->width || x < 0 || y > input->height || y < 0) {
        output->imageData[output->widthStep * v + u] = 0;
      }
      else {
        output->imageData[output->widthStep * v + u] = input->imageData[input->widthStep * y + x];
      }
    }
  }
}