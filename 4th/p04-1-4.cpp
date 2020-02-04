#include <stdio.h>
#include <opencv/highgui.h>

void main()
{
  IplImage* img;

  img = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 3);

  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      img->imageData[img->widthStep*y + x * 3 + 2] = x; // R
      img->imageData[img->widthStep*y + x * 3 + 1] = -x + 255; // G
      img->imageData[img->widthStep*y + x * 3 + 0] = y; // B
    }
  }

  cvNamedWindow("Color");
  cvShowImage("Color", img);
  cvSaveImage("color.bmp", img);


  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img);
}