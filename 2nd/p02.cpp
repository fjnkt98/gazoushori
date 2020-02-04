#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <opencv/highgui.h>

void createA(IplImage *img);
void createB(IplImage *img);
void createC(IplImage *img);
void createD(IplImage *img);
void createE(IplImage *img);
void createF(IplImage *img);
void createG(IplImage *img);
void createH(IplImage *img);
void createI(IplImage *img);

void main(int argc, char* argv[]){
  CvSize size;
  int pattern;

  switch (argc){
  // When the pattern selected
  case 2:
    if (argv[1][0] == 'a'){
      pattern = 0;
    }
    else if (argv[1][0] == 'b'){
      pattern = 1;
    }
    else if (argv[1][0] == 'c'){
      pattern = 2;
    }
    else if (argv[1][0] == 'd'){
      pattern = 3;
    }
    else if (argv[1][0] == 'e'){
      pattern = 4;
    }
    else if (argv[1][0] == 'f'){
      pattern = 5;
    }
    else if (argv[1][0] == 'g'){
      pattern = 6;
    }
    else if (argv[1][0] == 'h'){
      pattern = 7;
    }
    else if (argv[1][0] == 'i'){
      pattern = 8;
    }
    else{
      pattern = 0;
    }
    size.width = 256;
    size.height = 256;
    break;

  // When pixel size selected
  case 3:
    pattern = 0;
    size.width = atoi(argv[1]);
    size.height = atoi(argv[2]);
    break;
  // When the pattern and pixel size selected 
  case 4:
    if (argv[1][0] == 'a'){
      pattern = 0;
    }
    else if (argv[1][0] == 'b'){
      pattern = 1;
    }
    else if (argv[1][0] == 'c'){
      pattern = 2;
    }
    else if (argv[1][0] == 'd'){
      pattern = 3;
    }
    else if (argv[1][0] == 'e'){
      pattern = 4;
    }
    else if (argv[1][0] == 'f'){
      pattern = 5;
    }
    else if (argv[1][0] == 'g'){
      pattern = 6;
    }
    else if (argv[1][0] == 'h'){
      pattern = 7;
    }
    else if (argv[1][0] == 'i'){
      pattern = 8;
    }
    else{
      pattern = 0;
    }

    size.width = atoi(argv[2]);
    size.height = atoi(argv[3]);
    break;

  // When No argment selected
  default:
    pattern = 0;
    size.width = 256;
    size.height = 256;
  }

  IplImage* img = cvCreateImage(size, IPL_DEPTH_8U, 1);

  cvSetZero(img);

  switch (pattern){
  case 0:
    createA(img);
    break;
  case 1:
    createB(img);
    break;
  case 2:
    createC(img);
    break;
  case 3:
    createD(img);
    break;
  case 4:
    createE(img);
    break;
  case 5:
    createF(img);
    break;
  case 6:
    createG(img);
    break;
  case 7:
    createH(img);
    break;
  case 8:
    createI(img);
    break;
  }

  cvNamedWindow("p02");
  cvShowImage("p02", img);
  cvSaveImage("p02.png", img);

  cvWaitKey(0);

  cvDestroyAllWindows();
  cvReleaseImage(&img);
}

void createA(IplImage *img){
  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      if (y < (img->height / 2)){
        if (x < (img->width / 2)){
          img->imageData[img->widthStep * y + x] = (unsigned char)255; // Upper left
        }
        else{
          img->imageData[img->widthStep * y + x] = 0;   // Upper right
        }
      }
      else{
        if (x < (img->width / 2)){
          img->imageData[img->widthStep * y + x] = 0;   // Bottom left
        }
        else{
          img->imageData[img->widthStep * y + x] = (unsigned char)255; // Bottom right
        }
      }
    }
  }
}

void createB(IplImage *img){
  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      if (y < (img->height / 2)){
        if (x < (img->width / 2)){
          img->imageData[img->widthStep * y + x] = (unsigned char)255; // Upper left
        }
        else{
          img->imageData[img->widthStep * y + x] = (unsigned char)192;   // Upper right
        }
      }
      else{
        if (x < (img->width / 2)){
          img->imageData[img->widthStep * y + x] = 80;   // Bottom left
        }
        else{
          img->imageData[img->widthStep * y + x] = 0; // Bottom right
        }
      }
    }
  }
}

void createC(IplImage *img){
  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      if (x % 2){
        img->imageData[img->widthStep * y + x] = 0;
      }
      else{
        img->imageData[img->widthStep * y + x] = (unsigned char)255;
      }
    }
  }
}
void createD(IplImage *img){
  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      if (y % 2){
        if (x % 2){
          img->imageData[img->widthStep * y + x] = 0;
        }
        else{
          img->imageData[img->widthStep * y + x] = (unsigned char)255;
        }
      }
      else{
        if (x % 2){
          img->imageData[img->widthStep * y + x] = (unsigned char)255;
        }
        else{
          img->imageData[img->widthStep * y + x] = 0;
        }
      }
    }
  }
}
void createE(IplImage *img){
  double r;

  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      r = sqrt(pow(img->width/2 - x, 2) + pow(img->height/2 - y, 2));
      if (r < img->width / 2){
        img->imageData[img->widthStep * y + x] = (unsigned char)255;
      }
      else{
        img->imageData[img->widthStep * y + x] = 0;
      }
    }
  }
}
void createF(IplImage *img){
  double r;

  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      if (y < (img->height / 2)){
        if (x < (img->width / 2)){ // Upper left
          r = sqrt(pow(img->width / 4 - x, 2) + pow(img->height / 4 - y, 2));
          if (r < img->width / 4){
            img->imageData[img->widthStep * y + x] = (unsigned char)255;
          }
          else{
            img->imageData[img->widthStep * y + x] = 0;
          }
        }
        else{ // Upper right
          r = sqrt(pow(img->width * 3 / 4 - x, 2) + pow(img->height / 4 - y, 2));
          if (r < img->width / 4){
            img->imageData[img->widthStep * y + x] = (unsigned char)255;
          }
          else{
            img->imageData[img->widthStep * y + x] = 0;
          }
        }
      }
      else{
        if (x < (img->width / 2)){ // Bottom left
          r = sqrt(pow(img->width / 4 - x, 2) + pow(img->height * 3 / 4 - y, 2));
          if (r < img->width / 4){
            img->imageData[img->widthStep * y + x] = (unsigned char)255;
          }
          else{
            img->imageData[img->widthStep * y + x] = 0;
          }
        }
        else{ // Bottom right
          r = sqrt(pow(img->width * 3 / 4 - x, 2) + pow(img->height * 3 / 4 - y, 2));
          if (r < img->width / 4){
            img->imageData[img->widthStep * y + x] = (unsigned char)255;
          }
          else{
            img->imageData[img->widthStep * y + x] = 0;
          }
        }
      }
    }
  }
}
void createG(IplImage *img){
  const double r_max = sqrt(pow(img->width / 2, 2) + pow(img->height / 2, 2));
  double r;

  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      r = sqrt(pow(img->width / 2 - x, 2) + pow(img->height / 2 - y, 2));

      img->imageData[img->widthStep * y + x] = (unsigned char)(- 255*(r/r_max) + 255);
    }
  }
}

void createH(IplImage *img){
  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      img->imageData[img->widthStep * y + x] = (unsigned char)(127 * sin(x * (2*M_PI/img->width)) + 127);
    }
  }
}
void createI(IplImage *img){
  for (int y = 0; y < img->height; y++){
    for (int x = 0; x < img->width; x++){
      img->imageData[img->widthStep * y + x] = (unsigned char)(127 * cos(y * (2 * M_PI / img->height)) + 127);
    }
  }
}