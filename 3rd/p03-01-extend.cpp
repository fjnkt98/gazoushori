#include <stdio.h>
#include <opencv/highgui.h>

typedef struct {
  double rotate, skew, sx, sy, dx, dy;
  int x0, y0, u0, v0;
} AffineParam;

double deg2rad(double degree);
void affineTrans(IplImage* input, IplImage* output, AffineParam param);

void main(int argc, char* argv[]){
  IplImage* img1;
  IplImage* img2;

  int width, height;

  AffineParam param;

  // Argument estimation
  printf("argc = %d\n", argc);

  for (int k = 0; k < argc; k++){
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc < 2) {
    printf("Choose file name.");
    return;
  }

  // Load original image
  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {
    printf("Failed to load image file.");
    return;
  }
  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  // Second argument is transformed image width, third one is its height
  if (argc > 2) {
    width = atof(argv[2]);
    height = atof(argv[3]);
  }
  else { // When No argument, transformed image size is same as original
    width = img1->width;
    height = img1->height;
  }

  // Create transformed image
  img2 = cvCreateImage(cvSize(width, height), img1->depth, img1->nChannels);
  cvSetZero(img2);

  // Affine parameter setting
  param.rotate = CV_PI / 4.0;
  param.skew = 0.0;
  param.sx = double(img2->width / img1->width);
  param.sy = double(img2->height / img1->height);
  param.dx = 0.0;
  param.dy = 0.0;
  param.x0 = img1->width / 2;
  param.y0 = img1->height / 2;
  param.u0 = img2->width / 2;
  param.v0 = img2->height / 2;

  // Affine Transform
  affineTrans(img1, img2, param);
  cvNamedWindow("Variable Image Size");
  cvShowImage("Variable Image Size", img2);
  cvSaveImage("variable_image_size.bmp", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}

void affineTrans(IplImage* input, IplImage* output, AffineParam param){
  int x, y;

  double affine_matrix[2][3];

  affine_matrix[0][0] = param.sx * cos(param.rotate);
  affine_matrix[0][1] = param.sx * sin(param.rotate);
  affine_matrix[0][2] = param.sx * param.dx * cos(param.rotate) + param.sx * param.dy * sin(param.rotate);
  affine_matrix[1][0] = tan(param.skew) * cos(param.rotate) - param.sy * sin(param.rotate);
  affine_matrix[1][1] = tan(param.skew) * sin(param.rotate) + param.sy * cos(param.rotate);
  affine_matrix[1][2] = tan(param.skew) * (-param.dx * cos(param.rotate) + param.dy * sin(param.rotate))
                        + param.sy * (-param.dx * sin(param.rotate) + param.dy * cos(param.rotate));

  for (int v = 0; v < output->height; v++){
    for (int u = 0; u < output->width; u++){
      x = (affine_matrix[1][1] * (u - param.u0 - affine_matrix[0][2])
          - affine_matrix[0][1] * (v - param.v0 - affine_matrix[1][2]))
          / (affine_matrix[0][0] * affine_matrix[1][1] - affine_matrix[0][1] * affine_matrix[1][0])
          + param.x0 + 0.5;
      y = (-affine_matrix[1][0] * (u - param.u0 - affine_matrix[0][2])
          + affine_matrix[0][0] * (v - param.v0 - affine_matrix[1][2]))
          / (affine_matrix[0][0] * affine_matrix[1][1] - affine_matrix[0][1] * affine_matrix[1][0])
          + param.y0 + 0.5;

      if (x > input->width || x < 0 || y > input->height || y < 0) {
        output->imageData[output->widthStep * v + u] = 0;
      }
      else {
        output->imageData[output->widthStep * v + u] = input->imageData[input->widthStep * y + x];
      }
    }
  }
}

double deg2rad(double degree){
  return (CV_PI * degree / 180.0);
}