#include <stdio.h>
#include <opencv/highgui.h>

void main(int argc, char* argv[]){
  IplImage* img1;
  IplImage* img2;

  printf("argc = %d\n", argc);
  for (int i = 0; i < argc; i++){
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("\n\n");

  if (argc < 2){
    printf("Choose file.");
    return;
  }

  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL){
    printf("Failed to load image file.\n");
    return;
  }

  printf("nChannels = %d\n", img1->nChannels);
  if (img1->nChannels != 3){
    printf("Choose color image.");
    return;
  }

  cvNamedWindow("8bit Color");
  cvShowImage("8bit Color", img1);

  img2 = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);

  for (int y = 0; y < img2->height; y++){
    for (int x = 0; x < img2->width; x++){
      // ビットシフトで桁落ちさせて4段階にして，各区間の中央値にする
      img2->imageData[img2->widthStep*y + x * 3 + 0] = ((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 0]) >> 6) << 6) + (256 >> 2) / 2 - 1;
      img2->imageData[img2->widthStep*y + x * 3 + 1] = ((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 1]) >> 6) << 6) + (256 >> 2) / 2 - 1;
      img2->imageData[img2->widthStep*y + x * 3 + 2] = ((((unsigned char)img1->imageData[img1->widthStep*y + x * 3 + 2]) >> 6) << 6) + (256 >> 2) / 2 - 1;
    }
  }

  cvNamedWindow("2bit Color");
  cvShowImage("2bit Color", img2);
  cvSaveImage("p05-1-2.bmp", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}