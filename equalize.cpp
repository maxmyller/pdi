#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image;
  int width, height;
  VideoCapture cap;
  Mat histCinza, histEqualize, imageCinza, imageEqualize;
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
  Mat histImgCinza(histh, histw, CV_8UC1, Scalar(0,0,0));
  Mat histImgEqualize(histh, histw, CV_8UC1, Scalar(0,0,0));

  while(1){
    cap >> image;
    cvtColor(image,imageCinza,CV_BGR2GRAY);

    equalizeHist(imageCinza,imageEqualize);
    
    calcHist(&imageCinza, 1, 0, Mat(), histCinza, 1,
             &nbins, &histrange,
             uniform, acummulate);
    calcHist(&imageEqualize, 1, 0, Mat(), histEqualize, 1,
             &nbins, &histrange,
             uniform, acummulate);

    normalize(histCinza, histCinza, 0, histImgCinza.rows, NORM_MINMAX, -1, Mat());
    normalize(histEqualize, histEqualize, 0, histImgEqualize.rows, NORM_MINMAX, -1, Mat());

    histImgCinza.setTo(Scalar(0));
    histImgEqualize.setTo(Scalar(0));
    
    for(int i = 0; i < nbins; i++){
      line(histImgCinza,
           Point(i, histh),
           Point(i, histh-cvRound(histCinza.at<float>(i))),
           Scalar(128), 1, 8, 0);
      line(histImgEqualize,
           Point(i, histh),
           Point(i, histh-cvRound(histEqualize.at<float>(i))),
           Scalar(128), 1, 8, 0);
    }
    histImgCinza.copyTo(imageCinza(Rect(0, 0, nbins, histh)));
    histImgEqualize.copyTo(imageEqualize(Rect(0, histh, nbins, histh)));
    
    imshow("Imagem cinza", imageCinza);
    imwrite("imagecinza.png", imageCinza);

    imshow("Imagem equalizada", imageEqualize);
    imwrite("imageEqualize.png", imageEqualize);
    
    if(waitKey(30) >= 0) break;
  }
  return 0;
}
