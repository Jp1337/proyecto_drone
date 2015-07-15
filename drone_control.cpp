/** @file	drone_control.cpp
*	@brief	Rutina principal del programa de reconocimiento de objetos
*	@author	Juan Pablo Alvarez
*	@author	Renato Aguilar
*	@bug	"My code never has bugs, it just develops random features".
*/

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
#define PX 883
#define PY 500

using namespace std;
using namespace cv;

Mat frame, framei, descr;
vector<KeyPoint> feats;
Mat sample; 
int cont = 0, kstroke = 0, esta_seguro = 3;
bool spin_i=false, spin_d=false, go_up=false, go_dn=false;

/**
 * @brief	Una vez localizado el centroide del plano del objeto, intenta centrar la camara en el.
	Retorna una vez que el objeto esta centrado, o este sale de la escena.
 */

void focus(){

	while( (iproc::cx > 491 || iproc::cx < 391) || (iproc::cy > 300 || iproc::cy < 200) ){
		//cout << iproc::cx << " " << iproc::cy << endl;
		if ( iproc::cx<-99 ) break;
		if( (iproc::cx > 491 || iproc::cx < 391)){
			if(iproc::cx > 491){
				 if(!spin_d){ dcom::tell("vuelta_der"); spin_d = true; spin_i = false; }
			}
			else if(!spin_i){ dcom::tell("vuelta_izq"); spin_i = true; spin_d = false; }
		}
		else if(spin_d || spin_i){ dcom::tell("no_spin"); spin_d = spin_i = false;}
		
		if( iproc::cy > 300 || iproc::cy < 200 ){
			if(iproc::cy > 300){
				 if(!go_dn){ dcom::tell("baja"); go_dn = true; go_up = false; }
			}
			else if(!go_up){ dcom::tell("sube"); go_up = true; go_dn = false; }
		}
		else if(go_up || go_dn){ dcom::tell("no_updn"); go_up = false; go_dn = false; }	
	
		imdecode(*dcom::getFrame(),IMTYPE,&frame);
		imshow("Frame", iproc::matchFeatures(sample,feats,descr,frame));
		waitKey(1+DELAY);
	}	
}

/**
 * @brief Punto de inicio de la ejecucion. El programa termina una vez que el objeto fue localizado
 * @param argc Cantidad de argumentos del programa
 * @param argv Ejecutar sin argumentos para ver indicaciones.
 */

int main(int argc, char *argv[]){

  if(argc != 4) {
    std::cerr << "Uso" << argv[0] << " direccion:puerto /ruta/foto.png stbl|find" << std::endl;
    return EXIT_FAILURE;
  }
  sample = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE );
  dcom::setUrl(argv[1]);

  if( !sample.data ) {
  	cout << " Error leyendo el archivo de muestra"<< endl;
  	return -1;
  }

  iproc::detectFeatures(sample,feats,20,400);
  iproc::extractFeatures(sample,feats,descr);  
  //dcom::tell("takeoff");
  //dcom::tell("hold");
  waitKey(4000);
  dcom::tell("vuelta_der");
  
//  if(argv[3] == stbl){}

  	while(kstroke!=32){ 
  		
  		//int t = clock();
		imdecode(*dcom::getFrame(),IMTYPE,&frame);
		imshow("Frame", iproc::matchFeatures(sample,feats,descr,frame));
		//cout << "en: " << double(clock()-t)/CLOCKS_PER_SEC << " seg." << endl;
		
		if( iproc::found ) cont++;
		else cont = 0;
		if(cont >= esta_seguro){
			dcom::tell("hold");
			waitKey(50);
			focus();
			waitKey(50);
			if( iproc::found ){
				cout << "objeto encontrado! (" << iproc::cx << ", " << iproc::cy << ")" << endl;
			}
			else cont = 0;
			
		}
	
    	kstroke = waitKey(1+DELAY);

	}
    return EXIT_SUCCESS;
}

