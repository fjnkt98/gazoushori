#include <stdio.h>
#include <opencv/highgui.h>

// =================================================================================
void main()
{
  IplImage* img;

  // 画像サイズ 200×200, channels = 3 : カラー画像
  img = cvCreateImage(cvSize(100, 400), IPL_DEPTH_8U, 3);

  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
      if (x < img->width / 2) {
        if (y < img->height / 2) { // 白: (R,G,B)=(255,255,255)
          img->imageData[img->widthStep*y + x * 3 + 2] = 255; // R
          img->imageData[img->widthStep*y + x * 3 + 1] = 255; // G
          img->imageData[img->widthStep*y + x * 3 + 0] = 255; // B
        }
        else { // 赤: (R,G,B)=(255,0,0)
          img->imageData[img->widthStep*y + x * 3 + 2] = 255; // R
          img->imageData[img->widthStep*y + x * 3 + 1] = 0; // G
          img->imageData[img->widthStep*y + x * 3 + 0] = 0; // B
        }
      }
      else {
        if (y < img->height / 2) { // 緑: (R,G,B)=(0,255,0)
          img->imageData[img->widthStep*y + x * 3 + 2] = 0; // R
          img->imageData[img->widthStep*y + x * 3 + 1] = 255; // G
          img->imageData[img->widthStep*y + x * 3 + 0] = 0; // B
        }
        else { // 青: (R,G,B)=(0,0,255)
          img->imageData[img->widthStep*y + x * 3 + 2] = 0; // R
          img->imageData[img->widthStep*y + x * 3 + 1] = 0; // G
          img->imageData[img->widthStep*y + x * 3 + 0] = 255; // B
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