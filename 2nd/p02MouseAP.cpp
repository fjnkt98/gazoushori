#include <stdio.h>
#include <opencv/highgui.h>

void mouseCallback(int event, int x, int y, int flags, void *param);

void main(int argc, char* argv[]){
  IplImage* img;
  IplImage* img2;

  printf("argc = %d\n", argc);
  for (int k = 0; k < argc; k++) {
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc > 1 && (img = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) != NULL) {
    img2 = cvCloneImage(img);

    cvNamedWindow("p02-2_Mouse_AP");
    cvSetMouseCallback("p02-2_Mouse_AP", mouseCallback, img2);
    cvShowImage("p02-2_Mouse_AP", img2);

    cvWaitKey(0);
    cvSetMouseCallback("p02-2_Mouse_AP", NULL);

    cvDestroyAllWindows();
    cvReleaseImage(&img);
    cvReleaseImage(&img2);
  }
  else {
    printf("ファイル名を指定してください。\n");
  }
}

void mouseCallback(int event, int x, int y, int flags, void *param){
  if (param != NULL){
    IplImage *img = (IplImage*)param; // Cast param to IplImage type

    if ((event == CV_EVENT_MOUSEMOVE && flags == CV_EVENT_FLAG_LBUTTON) || (event == CV_EVENT_LBUTTONDOWN)){
      printf("X : %d, Y : %d, Value : %d\n", x, y, (unsigned char)img->imageData[img->widthStep * y + x]);

      // Inverse mouse clicked pixel value
      img->imageData[img->widthStep * y + x] = ~(unsigned char)img->imageData[img->widthStep * y + x];

      cvShowImage("p02-2_Mouse_AP", img);
    }
  }
}