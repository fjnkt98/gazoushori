#include <stdio.h>
#include <opencv/highgui.h>

void hsv2rgb(int h, unsigned char s, unsigned char v, unsigned char *bgr){
  float max = float(v);
  float min = (float)(v - (((float)s / 255) * v));

  if (h < 60) {
    bgr[2] = max; // R
    bgr[1] = (h * (max - min)) / 60 + min; // G
    bgr[0] = min; // B
  }
  else if (h < 120) {
    bgr[2] = (120 - h) * (max - min) / 60 + min; // R
    bgr[1] = max; // G
    bgr[0] = min; // B
  }
  else if (h < 180) {
    bgr[2] = min; // R
    bgr[1] = max; // G
    bgr[0] = (h - 120) * (max - min) / 60 + min; // B
  }
  else if (h < 240) {
    bgr[2] = min; // R
    bgr[1] = (240 - h) * (max - min) / 60 + min; // G
    bgr[0] = max; // B
  }
  else if (h < 300) {
    bgr[2] = (h - 240) * (max - min) / 60 + min; // R
    bgr[1] = min; // G
    bgr[0] = max; // B
  }
  else if (h < 360) {
    bgr[2] = max; // R
    bgr[1] = min; // G
    bgr[0] = (360 - h) * (max - min) / 60 + min; // B
  }
}

void main(){
  IplImage* img;

  const int width = 360;
  const int height = 256;

  img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
  cvSetZero(img);

  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      hsv2rgb(x, -y , 255, (unsigned char *)&img->imageData[img->widthStep*y + x * 3]);
    }
  }
  cvNamedWindow("HSV2RGB");
  cvShowImage("HSV2RGB", img);
  cvSaveImage("HSV2RGB.bmp", img);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img);
}
