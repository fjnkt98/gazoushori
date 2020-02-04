#include <stdio.h>
#include <opencv/highgui.h>

void main()
{
  IplImage* img;

  img = cvCreateImage(cvSize(200, 200), IPL_DEPTH_8U, 3);

  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if (0 < x && x <= img->width / 8){ // white
        img->imageData[img->widthStep*y + x * 3 + 2] = 255; // R
        img->imageData[img->widthStep*y + x * 3 + 1] = 255; // G
        img->imageData[img->widthStep*y + x * 3 + 0] = 255; // B
      }
      else if (img->width / 8 < x && x <= img->width / 8 * 2){ // magenta
        img->imageData[img->widthStep*y + x * 3 + 2] = 255; // R
        img->imageData[img->widthStep*y + x * 3 + 1] = 0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] = 255; // B
      }
      else if (img->width / 8 * 2 < x && x <= img->width / 8 * 3){ // blue
        img->imageData[img->widthStep*y + x * 3 + 2] = 0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] = 0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] = 255; // B
      }
      else if (img->width / 8 * 3 < x && x <= img->width / 8 * 4){ // cyan
        img->imageData[img->widthStep*y + x * 3 + 2] = 0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] = 255; // G
        img->imageData[img->widthStep*y + x * 3 + 0] = 255; // B
      }
      else if (img->width / 8 * 4 < x && x <= img->width / 8 * 5){ // green
        img->imageData[img->widthStep*y + x * 3 + 2] = 0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] = 255; // G
        img->imageData[img->widthStep*y + x * 3 + 0] = 0; // B
      }
      else if (img->width / 8 * 5< x && x <= img->width / 8 * 6){ // yellow
        img->imageData[img->widthStep*y + x * 3 + 2] = 255; // R
        img->imageData[img->widthStep*y + x * 3 + 1] = 255; // G
        img->imageData[img->widthStep*y + x * 3 + 0] = 0; // B
      }
      else if (img->width / 8 * 6 < x && x <= img->width / 8 * 7){ // red
        img->imageData[img->widthStep*y + x * 3 + 2] = 255; // R
        img->imageData[img->widthStep*y + x * 3 + 1] = 0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] = 0; // B
      }
      else if (img->width / 8 * 7 < x && x <= img->width){ // black
        img->imageData[img->widthStep*y + x * 3 + 2] = 0; // R
        img->imageData[img->widthStep*y + x * 3 + 1] = 0; // G
        img->imageData[img->widthStep*y + x * 3 + 0] = 0; // B
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