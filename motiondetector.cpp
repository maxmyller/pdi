#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image;
  int width, height;
  VideoCapture cap;
  Mat histAzul, histComp;
  vector<Mat> planes;
  int nbins = 64;
  float range[] = {0, 256};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;

  cap.open(0);
  
  if(!cap.isOpened()){
    cout << "cameras indisponiveis";
    return -1;
  }
  
  width  = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

  cout << "Largura = " << width << endl;
  cout << "Altura  = " << height << endl;

  int histw = nbins, histh = nbins/2;
  Mat histImgAzul(histh, histw, CV_8UC3, Scalar(0,0,0));

  cap >> image;
  split (image, planes);
   
  calcHist(&planes[2], 1, 0, Mat(), histAzul, 1,
           &nbins, &histrange,
           uniform, acummulate);

  while(1){
    cap >> image;
    split (image, planes);
   
    calcHist(&planes[2], 1, 0, Mat(), histComp, 1,
             &nbins, &histrange,
             uniform, acummulate);
    
    double comparador = compareHist(histAzul,histComp,CV_COMP_CORREL);
    
    if(comparador <= 0.995){
      putText(image, "Alerta! Movimento detectado!", cvPoint(190,250),
                    CV_FONT_HERSHEY_SIMPLEX, 0.5, cvScalar(0,0,255), 1, CV_AA);
    	calcHist(&planes[2], 1, 0, Mat(), histAzul, 1,
             	&nbins, &histrange,
             	uniform, acummulate);
    }
    
    imshow("Deteccao de movimento", image);
    imwrite("motiondetector.png", image);
    
    if(waitKey(30) >= 0) break;
  }
  return 0;
}
