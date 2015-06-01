#include "drone_comm.hpp"
#include <time.h>
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define  DELAY 0
#define  IMTYPE CV_LOAD_IMAGE_GRAYSCALE

using namespace std;
using namespace cv;

//g++ -lcurl -lcurlpp -lopencv_core -lopencv_highgui -lopencv_imgproc -o procesar procesar.cpp
//filtros tipo sobel con propósito netamente demostrativo


int main(int argc, char *argv[])
{
  if(argc != 2) {
    std::cerr << "Uso" << argv[0] << " direccion:puerto" << std::endl;
    return EXIT_FAILURE;
  }

  dcom::setUrl(argv[1]);
  Mat frame, framei, dx, dy, idx, idy;
  
  int its = 0;

  	while(its < 2000){
		int t = clock();    	
		imdecode(*dcom::getFrame(),IMTYPE,&frame);
		Sobel(frame, dx, frame.depth(), 2, 0, 3);
		Sobel(frame, dy, frame.depth(), 0, 2, 3);
		frame = dx + dy;
		
    	imshow("Frame", frame);
    	waitKey(1+DELAY);
    	its++;
    	cout << ((double)(clock()-t)/CLOCKS_PER_SEC)*1000 << "ms" << endl;
	}
    return EXIT_SUCCESS;

}
