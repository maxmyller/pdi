#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define RADIUS 20

using namespace cv;
using namespace std;

double gamaH;
int sliderGamaH = 0;
int sliderGamaH_MAX = 100;

double gamaL;
int sliderGamaL = 0;
int sliderGamaL_MAX = 100;

double c;
int sliderC = 0;
int sliderC_MAX = 100;

double d0;
int sliderD0 = 0;
int sliderD0_MAX = 100;

char TrackbarName[50];

void slider(int, void*){
  //double c, gamaH, gamaL, d0;
	gamaH = sliderGamaH/10.0;
	gamaL = sliderGamaL/10.0;
	d0 = sliderD0/10.0;
  c = sliderC/1000.0;
}

//calculando o filtro homomórfico através do ajuste dos sliders da trackbar
Mat FiltroHomomorfico(int M, int N){
  Mat H = Mat(M, N, CV_32F), filter;
  for(int i=0; i<M; i++){
    for(int j=0; j<N; j++){
      double D = (i-M/2.0)*(i-M/2.0) + (j-N/2.0)*(j-N/2.0);
      H.at<float> (i,j) = (gamaH-gamaL)*(1.0-exp(-1.0*c*D/(d0*d0)))+ gamaL;
    }
  }
  Mat comps[]= {H, H};
  merge(comps, 2, filter);
  return filter;
}


// troca os quadrantes da imagem da DFT
void deslocaDFT(Mat& image ){
  Mat tmp, A, B, C, D;

  // se a imagem tiver tamanho impar, recorta a regiao para
  // evitar cópias de tamanho desigual
  image = image(Rect(0, 0, image.cols & -2, image.rows & -2));
  int cx = image.cols/2;
  int cy = image.rows/2;

  // reorganiza os quadrantes da transformada
  // A B   ->  D C
  // C D       B A
  A = image(Rect(0, 0, cx, cy));
  B = image(Rect(cx, 0, cx, cy));
  C = image(Rect(0, cy, cx, cy));
  D = image(Rect(cx, cy, cx, cy));

  // A <-> D
  A.copyTo(tmp);  D.copyTo(A);  tmp.copyTo(D);

  // C <-> B
  C.copyTo(tmp);  B.copyTo(C);  tmp.copyTo(B);
}

int main(int , char** argv){
  VideoCapture cap;
  Mat imaginaryInput, complexImage, multsp;
  Mat padded, filter, mag;
  Mat image, imagegray, tmp;
  Mat_<float> realInput, zeros;
  vector<Mat> planos;
  char key;
  namedWindow("original",1);

  sprintf(TrackbarName, "Gama H: ");
    createTrackbar(TrackbarName, "original", &sliderGamaH, sliderGamaH_MAX, slider);

  sprintf(TrackbarName, "Gama L: ");
    createTrackbar(TrackbarName, "original", &sliderGamaL, sliderGamaL_MAX, slider);

  sprintf(TrackbarName, "C: ");
    createTrackbar(TrackbarName, "original", &sliderC, sliderC_MAX, slider);

  sprintf(TrackbarName, "D0: ");
    createTrackbar(TrackbarName, "original", &sliderD0, sliderD0_MAX, slider);

  // valores ideais dos tamanhos da imagem
  // para calculo da DFT
  int dft_M, dft_N;

  image = imread(argv[1]);

  // identifica os tamanhos otimos para
  // calculo do FFT
  dft_M = getOptimalDFTSize(image.rows);
  dft_N = getOptimalDFTSize(image.cols);

  // realiza o padding da imagem
  copyMakeBorder(image, padded, 0,
                 dft_M - image.rows, 0,
                 dft_N - image.cols,
                 BORDER_CONSTANT, Scalar::all(0));

  // parte imaginaria da matriz complexa (preenchida com zeros)
  zeros = Mat_<float>::zeros(padded.size());

  // prepara a matriz complexa para ser preenchida
  complexImage = Mat(padded.size(), CV_32FC2, Scalar(0));

  // a função de transferência (filtro frequencial) deve ter o
  // mesmo tamanho e tipo da matriz complexa
  filter = complexImage.clone();

  // cria uma matriz temporária para criar as componentes real
  // e imaginaria do filtro ideal
  tmp = Mat(dft_M, dft_N, CV_32F);

  // cria a matriz com as componentes do filtro e junta
  // ambas em uma matriz multicanal complexa
  Mat comps[]= {tmp, tmp};
  merge(comps, 2, filter);

  for(;;){
    image = imread(argv[1]);
    cvtColor(image, imagegray, CV_BGR2GRAY);
    imshow("original", imagegray);

    // realiza o padding da imagem
    copyMakeBorder(imagegray, padded, 0,
                   dft_M - image.rows, 0,
                   dft_N - image.cols,
                   BORDER_CONSTANT, Scalar::all(0));

    // limpa o array de matrizes que vao compor a
    // imagem complexa
    planos.clear();
    
    // cria a compoente real
    realInput = Mat_<float>(padded);

    // insere as duas componentes no array de matrizes
    planos.push_back(realInput);
    planos.push_back(zeros);

    // combina o array de matrizes em uma unica
    // componente complexa
    merge(planos, complexImage);

    // calcula o dft
    dft(complexImage, complexImage);

    // realiza a troca de quadrantes
    deslocaDFT(complexImage);

    filter = FiltroHomomorfico(dft_M, dft_N);

    // aplica o filtro frequencial
    mulSpectrums(complexImage,filter,complexImage,0);

    // limpa o array de planos
    planos.clear();

    // separa as partes real e imaginaria para modifica-las
    split(complexImage, planos);

    // recompoe os planos em uma unica matriz complexa
    merge(planos, complexImage);

    // troca novamente os quadrantes
    deslocaDFT(complexImage);

    // calcula a DFT inversa
    idft(complexImage, complexImage);

    // limpa o array de planos
    planos.clear();

    // separa as partes real e imaginaria da
    // imagem filtrada
    split(complexImage, planos);

    // normaliza a parte real para exibicao
    normalize(planos[0], planos[0], 0, 1, CV_MINMAX);
    imshow("Imagem filtrada", planos[0]);

    key = (char) waitKey(10);
    if( key == 27 ) break;
  }

  return 0;
}