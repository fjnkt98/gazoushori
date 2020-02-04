#include <stdio.h>
#include <opencv/highgui.h>

void polarTrans(IplImage* input, IplImage* output);

void main(int argc, char* argv[]){
  IplImage* img1;
  IplImage* img2;

  for (int k = 0; k < argc; k++){
    printf("argv[%d] = %s\n", k, argv[k]);
  }
  printf("\n\n");

  if (argc < 2) {
    printf("Choose file name.");
    return;
  }

  if ((img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED)) == NULL) {
    printf("Failed to load image file.");
    return;
  }

  cvNamedWindow("Original");
  cvShowImage("Original", img1);

  img2 = cvCreateImage(cvSize(1280, 640), img1->depth, img1->nChannels);
  cvSetZero(img2);

  polarTrans(img1, img2);
  cvNamedWindow("Polar Transform");
  cvShowImage("Polar Transform", img2);
  cvSaveImage("polar_transformed_image.jpg", img2);

  cvWaitKey(0);
  cvDestroyAllWindows();
  cvReleaseImage(&img1);
  cvReleaseImage(&img2);
}

void polarTrans(IplImage* input, IplImage* output){
  int x, y;

  for (int r = 0; r < output->height; r++) {
    for (int t = 0; t < output->width; t++) {
      x = (r - output->height) * sin(t * 2 * CV_PI / output->width - CV_PI / 2) + input->width / 2;
      y = (r - output->height) * cos(t * 2 * CV_PI / output->width - CV_PI / 2) + input->height / 2;

      if (x < 0 || x > input->width || y < 0 || y > input->height) {
        output->imageData[output->widthStep * r + t] = 0;
      }
      else {
        output->imageData[output->widthStep * r + t] = input->imageData[input->widthStep * y + x];
      }
    }
  }
}