#include <stdio.h>
#include <math.h>
#include <opencv/highgui.h>


void main()
{
  IplImage* img;
  int circle_radius = 60;
  int triangle_radius = 40;
  int red_center_x;
  int red_center_y;
  int green_center_x;
  int green_center_y;
  int blue_center_x;
  int blue_center_y;

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

  // Draw red circle
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if ((pow((x - red_center_x), 2) + pow((y - red_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 2] += (unsigned char)255; // R
        img->imageData[img->widthStep*y + x * 3 + 1] += (unsigned char)0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] += (unsigned char)0; // B
      }
      else {
        img->imageData[img->widthStep*y + x * 3 + 2] += (unsigned char)0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] += (unsigned char)0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] += (unsigned char)0; // B
      }
    }
  }

  // Draw green circle
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if ((pow((x - green_center_x), 2) + pow((y - green_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 2] += (unsigned char)0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] += (unsigned char)255; // G
        img->imageData[img->widthStep*y + x * 3 + 0] += (unsigned char)0; // B
      }
      else {
        img->imageData[img->widthStep*y + x * 3 + 2] += (unsigned char)0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] += (unsigned char)0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] += (unsigned char)0; // B
      }
    }
  }

  // Draw blue circle
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if ((pow((x - blue_center_x), 2) + pow((y - blue_center_y), 2)) <= pow(circle_radius, 2)) {
        img->imageData[img->widthStep*y + x * 3 + 2] += (unsigned char)0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] += (unsigned char)0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] += (unsigned char)255; // B
      }
      else {
        img->imageData[img->widthStep*y + x * 3 + 2] += (unsigned char)0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] += (unsigned char)0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] += (unsigned char)0; // B
      }
    }
  }

  cvNamedWindow("p04-2-1");
  cvShowImage("p04-2-1", img);
  cvSaveImage("p04-2-1.bmp", img);


  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img);
}