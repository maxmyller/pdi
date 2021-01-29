#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image;
  int width, height;
  int numbolhas;
  int numburacos;
  
  CvPoint p;
  image = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
  
  if(!image.data){
    cout << "Imagem nao carregou\n";
    return(-1);
  }
  width=image.size().width;
  height=image.size().height;

  p.x = 0;
  p.y = 0;

  numbolhas = 0;
  numburacos = 0;

  //retirando as bolhas das bordas

  for(int i = 0; i < height; i++){
    
    int primeiraLinha = image.at<uchar>(0,i);
    int ultimaLinha = image.at<uchar>(255,i);
    
    if(primeiraLinha == 255){
      p.x = i;
      p.y = 0;
      floodFill(image,p,0);
    }
    if(ultimaLinha == 255){
      p.x = i;
      p.y = 255;
      floodFill(image,p,0);
    }
  }
  for(int i = 0; i < width; i++){
    
    int primeiraColuna = image.at<uchar>(i,0);
    int ultimaColuna = image.at<uchar>(i,255);
    
    if(primeiraColuna == 255){
      p.x = 0;
      p.y = i;
      floodFill(image,p,0);
    }
    if(ultimaColuna == 255){
      p.x = 255;
      p.y = i;
      floodFill(image,p,0);
    }
  }

  //Definicao do fundo da imagem
  floodFill(image, p, 1);

  // busca de bolhas com e sem buraco - rotulação e contagem
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++){
			if(image.at<uchar>(i, j) == 255){
				numbolhas++;
        p.x=j;
				p.y=i;
				floodFill(image,p,50);
			}
			if(image.at<uchar>(i,j) == 0){
				numburacos++;
				p.x=j;
				p.y=i;
				floodFill(image,p,200);
			}
		}

	imshow("image",image);
	imwrite("labeling.png",image);
	cout << "Numero total de bolhas: " << numbolhas << endl;
  cout << "Numero de bolhas com buraco: " << numburacos << endl;
  cout << "Numero de bolhas sem buraco: " << numbolhas - numburacos << endl;

  waitKey();
  return 0;
}
