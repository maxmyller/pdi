#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int, char**){
  Mat image;

  image = imread("biel.png",CV_LOAD_IMAGE_GRAYSCALE);
  if(!image.data){
      cout << "Nao abriu a imagem" << endl;
  }

  int p1[2], p2[2];

  cout << "Coordenadas de P1: " << endl;
  cin >> p1[0] >> p1[1];
  cout << "Coordenadas de P2: " << endl;
  cin >> p2[0] >> p2[1];

//Convertendo a área selecionada da imagem em negativo
  for (int i = p1[0]; i < p2[0]; i++){
      for (int j = p1[1]; j < p2[1]; j++){
          image.at<uchar>(i,j) = 255 - image.at<uchar>(i,j);
      }
  }

  imshow("janela", image);
  waitKey();
  return 0;
        

    
}
