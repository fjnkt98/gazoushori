#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>

IplImage* img;

int circle_radius = 60;
int triangle_radius = 40;

int red_center_x;
int red_center_y;
int green_center_x;
int green_center_y;
int blue_center_x;
int blue_center_y;

void rCallback(int r);
void gCallback(int g);
void bCallback(int b);

void main()
{
  int r_concentration = 255;
  int g_concentration = 255;
  int b_concentration = 255;

  img = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 3);

  red_center_x = img->width / 2;
  red_center_y = img->height / 2 - triangle_radius;
  green_center_x = img->width / 2 - triangle_radius * sin(CV_PI / 3);
  green_center_y = img->height / 2 + triangle_radius / 2;
  blue_center_x = img->width / 2 + triangle_radius * sin(CV_PI / 3);
  blue_center_y = img->height / 2 + triangle_radius / 2;

  // Reset image
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      img->imageData[img->widthStep*y + x * 3 + 2] = (unsigned char)0; // R
      img->imageData[img->widthStep*y + x * 3 + 1] = (unsigned char)0; // G
      img->imageData[img->widthStep*y + x * 3 + 0] = (unsigned char)0; // B
    }
  }

  // Draw default image
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if ((pow((x - red_center_x), 2) + pow((y - red_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 2] = (unsigned char)(255); // R
      }
      if ((pow((x - green_center_x), 2) + pow((y - green_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 1] = (unsigned char)(255); // G
      }
      if ((pow((x - blue_center_x), 2) + pow((y - blue_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 0] = (unsigned char)(255); // B
      }
    }
  }

  cvNamedWindow("p04-2-3");

  cvCreateTrackbar("R", "p04-2-3", &r_concentration, 255, rCallback);
  cvCreateTrackbar("G", "p04-2-3", &g_concentration, 255, gCallback);
  cvCreateTrackbar("B", "p04-2-3", &b_concentration, 255, bCallback);

  cvShowImage("p04-2-3", img);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img);
  cvReleaseImage(&img);
}

void rCallback(int r){
  // Update red circle
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if ((pow((x - red_center_x), 2) + pow((y - red_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 2] = (unsigned char)(r); // R
      }
    }
  }
  cvShowImage("p04-2-3", img);
}

void gCallback(int g){
  // Update green circle
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if ((pow((x - green_center_x), 2) + pow((y - green_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 1] = (unsigned char)(g); // G
      }
    }
  }
  cvShowImage("p04-2-3", img);
}

void bCallback(int b){
  // Update blue circle
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if ((pow((x - blue_center_x), 2) + pow((y - blue_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 0] = (unsigned char)(b); // B
      }
    }
  }
  cvShowImage("p04-2-3", img);
}