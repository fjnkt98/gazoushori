#include <stdio.h>
#include <opencv/highgui.h>

void main()
{
  IplImage* img;

  img = cvCreateImage(cvSize(200, 200), IPL_DEPTH_8U, 3);

  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if (x < img->width / 2) {
        if (y < img->height / 2) { // left side up : cyan
          img->imageData[img->widthStep*y + x * 3 + 2] = 0; // R
          img->imageData[img->widthStep*y + x * 3 + 1] = 255; // G
          img->imageData[img->widthStep*y + x * 3 + 0] = 255; // B
        }
        else { // left side down : magenta
          img->imageData[img->widthStep*y + x * 3 + 2] = 255; // R
          img->imageData[img->widthStep*y + x * 3 + 1] = 0; // G
          img->imageData[img->widthStep*y + x * 3 + 0] = 255; // B
        }
      }
      else {
        if (y < img->height / 2) { // right side up : yellow
          img->imageData[img->widthStep*y + x * 3 + 2] = 255; // R
          img->imageData[img->widthStep*y + x * 3 + 1] = 255; // G
          img->imageData[img->widthStep*y + x * 3 + 0] = 0; // B
        }
        else { // right side down : black
          img->imageData[img->widthStep*y + x * 3 + 2] = 0; // R
          img->imageData[img->widthStep*y + x * 3 + 1] = 0; // G
          img->imageData[img->widthStep*y + x * 3 + 0] = 0; // B
        }
      }

    }
  }
  cvNamedWindow("Color");
  cvShowImage("Color", img);
  cvSaveImage("color.bmp", img);


  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img);
}