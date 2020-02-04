#include <stdio.h>
#include <opencv/highgui.h>

void main(int argc, char* argv[]){
  // The case of no argument
  if (argc == 1){
    printf("Choose file name!\n");
    return;
  }

  IplImage* img;      // Original image
  IplImage* img_inv;  // Inversed image

  // Load original image
  cvNamedWindow("Origin");
  img = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);

  if (img == NULL){ // load failed
    printf("Load Error\n");
    cvWaitKey(0);
  }
  else{
    cvShowImage("Origin", img); // Display original image

    img_inv = cvCloneImage(img);  // clone original image

    // Inversing via bit inversion
    for (int y = 0; y < img_inv->height; y++){
      for (int x = 0; x < img_inv->width; x++){
        img_inv->imageData[img->widthStep * y + x] = ~(unsigned char)(img_inv->imageData[img->widthStep * y + x]);
      }
    }

    // Display inversed image
    cvNamedWindow("Inversed");
    cvShowImage("Inversed", img_inv);

    cvWaitKey(0);
  }

  cvDestroyAllWindows();
  cvReleaseImage(&img);
  cvReleaseImage(&img_inv);
}