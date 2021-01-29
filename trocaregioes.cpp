#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int, char**){
    Mat image, imageout;
    int largura, altura;

    image = imread("biel.png",CV_LOAD_IMAGE_GRAYSCALE);
    imageout = imread("biel.png",CV_LOAD_IMAGE_GRAYSCALE);
    if(!image.data){
        cout << "Nao abriu a imagem" << endl;
    }
    largura=image.size().width;
    altura=image.size().height;

    for(int i = 0; i < largura/2; i++){
		for(int j = 0; j < altura/2; j++){
			imageout.at<uchar>(i,j) = image.at<uchar>(largura/2 + i,altura/2 + j);
			imageout.at<uchar>(i,altura/2 + j) = image.at<uchar>(largura/2 + i,j);
			imageout.at<uchar>(largura/2 + i,j) = image.at<uchar>(i,altura/2 + j);
			imageout.at<uchar>(largura/2 + i,altura/2 + j) = image.at<uchar>(i,j);
		}
    }

    imshow("Resultado da imagem",imageout);
	imwrite("trocaregioes.png",imageout);

    waitKey();
    return 0;
}