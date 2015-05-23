#include <sstream>
#include <time.h>
#include <cstdlib>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
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

  string url(argv[1]);
  Mat frame, framei, dx, dy, idx, idy;
  std::stringstream bin_data;
  string wtf;
  vector<char> bin_frame;
  int its = 0;
  
  try {

    curlpp::Cleanup cleaner;
  	curlpp::Easy request;   	

  	while(its < 2000){
		int t = clock();
  		bin_data.str( std::string() );
  		bin_data.clear();
    	request.setOpt(new curlpp::options::Url(url));
		request.setOpt(new curlpp::options::WriteStream(&bin_data));
    	request.perform();
		wtf = bin_data.str();
		bin_frame.assign(wtf.c_str(), wtf.c_str()+wtf.size());
		// Aqui esta parte de la magia, se guarda la imagen servida por .js en
		// un stream de bytes, y se copia en un vector de bytes.		
		imdecode(bin_frame,IMTYPE,&frame);
		Sobel(frame, dx, frame.depth(), 2, 0, 3);	//luego hay que cambiar esto 
		Sobel(frame, dy, frame.depth(), 0, 2, 3);	//por un detector de features
		frame = dx + dy;
		
    	imshow("Frame", frame);
    	waitKey(1+DELAY);	//breve retraso para ver la pantalla
    	its++;
    	cout << ((double)(clock()-t)/CLOCKS_PER_SEC)*1000 << "ms" << endl; //cuanto me demoro en procesar cada frame? 
	}
    return EXIT_SUCCESS;
  }
  
  catch ( curlpp::LogicError & e ) {
    std::cout << e.what() << std::endl;
  }
  
  catch ( curlpp::RuntimeError & e ) {
    std::cout << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
