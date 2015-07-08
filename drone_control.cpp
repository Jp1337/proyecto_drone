#include "drone_comm.hpp"
#include "drone_improc.hpp"
#include <time.h>
#include <cstdlib>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define  DELAY 4
#define  IMTYPE CV_LOAD_IMAGE_GRAYSCALE

using namespace std;
using namespace cv;

//g++ -lcurl -lcurlpp -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_calib3d drone_control.cpp


int main(int argc, char *argv[]){

  if(argc != 3) {
    std::cerr << "Uso" << argv[0] << " direccion:puerto /ruta/foto.png" << std::endl;
    return EXIT_FAILURE;
  }

  dcom::setUrl(argv[1]);
  Mat frame, framei, descr;
  vector<KeyPoint> feats;
  Mat sample = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE );
  if( !sample.data ) {
  	cout << " Error leyendo el archivo de muestra"<< endl;
  	return -1;
  }

  iproc::detectFeatures(sample,feats,16,400);
  iproc::extractFeatures(sample,feats,descr);  
  
  int its = 0;
  int kstroke = 0;

  	while(kstroke!=32){ // deberia ser espacio
		int t = clock();    	
		imdecode(*dcom::getFrame(),IMTYPE,&frame);
		cout << "decode:" <<((double)(clock()-t)/CLOCKS_PER_SEC)*1000 << "ms" << endl;
		t = clock();
		// do stuff here
    	imshow("Frame", iproc::matchFeatures(sample,feats,descr,frame));
    	kstroke = waitKey(1+DELAY);
    	its++;
    	cout << "process:" << ((double)(clock()-t)/CLOCKS_PER_SEC)*1000 << "ms" << endl;
	}
    return EXIT_SUCCESS;
}

