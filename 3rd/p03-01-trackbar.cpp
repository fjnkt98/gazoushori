#include <stdio.h>
#include <opencv/highgui.h>

typedef struct {
  double rotate, skew, sx, sy, dx, dy;
  int x0, y0, u0, v0;
} AffineParam;

double deg2rad(double degree);
void affineTrans(IplImage* input, IplImage* output, AffineParam param);

void rotationCallback(int rotation);
void skewCallback(int skew);
void translationCallback(int translation);
void scaleCallback(int scale);

// Declarate as global variable
IplImage* img1;
IplImage* img2;
AffineParam param;

void main(int argc, char* argv[]){
  // Track bar variables
  int rotation = 180;
  int skew = 90;
  int translation = 50;
  int scale = 50;

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

  // Create transformed image
  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);
  cvSetZero(img2);

  // Affine parameter Initialization
  param.rotate = 0;
  param.skew = 0.0;
  param.sx = 1.0;
  param.sy = 1.0;
  param.dx = 0.0;
  param.dy = 0.0;
  param.x0 = img1->width / 2;
  param.y0 = img1->height / 2;
  param.u0 = img2->width / 2;
  param.v0 = img2->height / 2;

  // Affine Transform
  affineTrans(img1, img2, param);
  cvNamedWindow("Real Time Affine Transform");

  // Create track bar
  cvCreateTrackbar("Rotation", "Real Time Affine Transform", &rotation, 360, rotationCallback);
  cvCreateTrackbar("Skew", "Real Time Affine Transform", &skew, 180, skewCallback);
  cvCreateTrackbar("Translation", "Real Time Affine Transform", &translation, 100, translationCallback);
  cvCreateTrackbar("Scale", "Real Time Affine Transform", &scale, 100, scaleCallback);
  cvShowImage("Real Time Affine Transform", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}

void affineTrans(IplImage* input, IplImage* output, AffineParam p){
  int x, y;

  double affine_matrix[2][3];

  affine_matrix[0][0] = p.sx * cos(p.rotate);
  affine_matrix[0][1] = p.sx * sin(p.rotate);
  affine_matrix[0][2] = p.sx * p.dx * cos(p.rotate) + p.sx * p.dy * sin(p.rotate);
  affine_matrix[1][0] = tan(p.skew) * cos(p.rotate) - p.sy * sin(p.rotate);
  affine_matrix[1][1] = tan(p.skew) * sin(p.rotate) + p.sy * cos(p.rotate);
  affine_matrix[1][2] = tan(p.skew) * (-p.dx * cos(p.rotate) + p.dy * sin(p.rotate))
    + p.sy * (-p.dx * sin(p.rotate) + p.dy * cos(p.rotate));

  for (int v = 0; v < output->height; v++){
    for (int u = 0; u < output->width; u++){
      x = (affine_matrix[1][1] * (u - p.u0 - affine_matrix[0][2])
        - affine_matrix[0][1] * (v - p.v0 - affine_matrix[1][2]))
        / (affine_matrix[0][0] * affine_matrix[1][1] - affine_matrix[0][1] * affine_matrix[1][0])
        + p.x0 + 0.5;
      y = (-affine_matrix[1][0] * (u - p.u0 - affine_matrix[0][2])
        + affine_matrix[0][0] * (v - p.v0 - affine_matrix[1][2]))
        / (affine_matrix[0][0] * affine_matrix[1][1] - affine_matrix[0][1] * affine_matrix[1][0])
        + p.y0 + 0.5;

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

void rotationCallback(int rotation){
  // from -pi to +pi
  param.rotate = deg2rad((double)rotation - 180.0);
  affineTrans(img1, img2, param);
  cvShowImage("Real Time Affine Transform", img2);
}

void skewCallback(int skew){
  // from -pi/2 to +pi/2
  param.skew = deg2rad((double)skew - 90.0);
  affineTrans(img1, img2, param);
  cvShowImage("Real Time Affine Transform", img2);
}

void translationCallback(int translation){
  // from -50 to +50
  param.dx = translation - 50;
  affineTrans(img1, img2, param);
  cvShowImage("Real Time Affine Transform", img2);
}

void scaleCallback(int scale){
  // from 50% to 150%
  param.sx = (double)((scale + 50) / 100.0);
  param.sy = (double)((scale + 50) / 100.0);
  affineTrans(img1, img2, param);
  cvShowImage("Real Time Affine Transform", img2);
}