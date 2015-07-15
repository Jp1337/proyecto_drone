/** @file	drone_improc.cpp
*	@brief	Libreria de funciones para el reconocimiento del objeto
*	@author	Juan Pablo Alvarez
*	@author	Renato Aguilar
*/

//nota: para la cámara, a 20cm de distancia, 9.5 cm son aprox. 373px
//cachando... como 40px por cm de plano, a 20cm del lente...

#include <opencv2/core/core.hpp>
#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <time.h>
#include <opencv2/calib3d/calib3d.hpp>
#define EXTR FREAK
#define MTCH BruteForceMatcher<Hamming>

using namespace cv;
using namespace std;

namespace iproc{
	
	bool found = false;
	int cx, cy;	//centroide
	double m[4]; //pendientes
	
	// Public-domain function by Darel Rex Finley, 2006.
	/**
	 * @brief Función para calcular el área de un poligono dados sus vertices
	 * @param X Coordenadas en eje 'x' de los vertices
	 * @param Y Coordenadas en eje 'y' de los vertices
	 * @param points Cantidad de vertices
	 */
	double polygonArea(double *X, double *Y, int points) {
		double  area=0. ;
		int i, j=points-1  ;
		for (i=0; i<points; i++) {
			area+=(X[j]+X[i])*(Y[j]-Y[i]); j=i; 
			}
		return area*.5; 
	};
	
	/**
	 * @brief Detecta y almacena los keypoints de una imagen con el detector de ORB (FAST + piramide)
	 * @param img Imagen a procesar
	 * @param kpts Vector en el que se almacenaran los keypoints
	 * @param lvls Niveles que se usaran en la piramide
	 * @param cuantos Maximo de kpts a encontrar
	 */
	void detectFeatures(Mat &img, vector<KeyPoint> &kpts, int lvls = 16, int cuantos = 500) {
	  OrbFeatureDetector detector(cuantos, 1.1f, lvls, 4, 0, 4, ORB::HARRIS_SCORE, 3);
	  detector.detect(img, kpts);
	};
	
	/**
	 * @brief funcion que calcula y almacena los descriptores en base a los keypoints utilizando FREAK
	 * @param img Imagen a procesar
	 * @param kpts Vector que contiene los keypoints
	 * @param descriptors Matriz que almacenara los descriptores
	 */
	void extractFeatures(Mat &img, vector<KeyPoint> &kpts, Mat &descriptors){
	  FREAK extractor;
	  extractor.compute( img, kpts, descriptors );
	}
	
	/**
	 * @brief Realiza el pareo de features entre la imagen y la escena, grafica los keypoints de la escena, 
	 estima la mejor homografia entre los features del objeto y los de la escena, 
	 aplica dicha transformación a las esquinas y centroide del objeto, dibuja esto en la imagen de salida y
	 retorna dicha matriz. Su ejecución puede cambiar el flag "found" y las coordenadas del centroide.
	 * @param img_objeto Imagen del objeto (si, sabemos que esta de mas y se puede ahorrar esta parte).
	 * @param keypoints_objeto Los keypoints del objeto.
	 * @param descriptor_objeto Matriz que contiene los descriptores del objeto.
	 * @param img_escena Imagen de la escena a procesar.
	 * @param debug Flag que determinar si se imprime output de debugging o no.
	 */
	Mat matchFeatures(Mat img_objeto, vector<KeyPoint> keypoints_objeto, Mat descriptor_objeto, Mat img_escena, bool debug = 1){
		found = false;
		vector<KeyPoint> keypoints_escena;
		vector<DMatch> matches, good_matches;
		Mat descriptor_escena, imgMatch;
		MTCH matcher;
		double t_1, t_2, t_3 = 0;
    	double max_dist = 0; 
	    double min_dist = 100;
    	double avg_dist = 0, g_avg_dist = 0;    
	    double factor_tolerancia = 10;
	    double ratio_aprox = 0;
    	cx=cy=-100;
    	    	    
		if(debug) { t_1 = (double)getTickCount(); }
		detectFeatures(img_escena,keypoints_escena,20,1000);
		if(debug) { t_1 = ((double)getTickCount() - t_1)/getTickFrequency(); }

		if(debug) { t_2 = (double)getTickCount(); }   
		extractFeatures(img_escena,keypoints_escena,descriptor_escena);
		if(debug) { t_2 = ((double)getTickCount() - t_2)/getTickFrequency(); }

		if(!descriptor_escena.empty()){

		  matcher.match(descriptor_objeto, descriptor_escena, matches);
		  
		  for( int i = 0; i < descriptor_objeto.rows; i++ ){ 
		  double dist = matches[i].distance;
		  if( dist < min_dist ) min_dist = dist;
		  if( dist > max_dist ) max_dist = dist;
		  avg_dist += dist;
		  }
		  avg_dist /= descriptor_objeto.rows;
		  sort(matches.begin(),matches.end());
		  
		  for( int i = 0; i < matches.size()/3; i++ ) { 
		  g_avg_dist += matches[i].distance;
		  good_matches.push_back(matches[i]);
		  }
		  g_avg_dist /= good_matches.size();		  
		  if(debug) { cout <<"Distancias  [Min/Max/Avg/Good_avg]     "<<min_dist<<" "<<max_dist<<" "<<avg_dist<<" "<<g_avg_dist<< " | g.avg/avg: " <<g_avg_dist/avg_dist<< endl; }

		  vector<Point2f> objeto;
		  vector<Point2f> escena;
		  for( int i = 0; i < good_matches.size(); i++ ) {
		    objeto.push_back( keypoints_objeto[ good_matches[i].queryIdx ].pt );
		    escena.push_back( keypoints_escena[ good_matches[i].trainIdx ].pt );
		  }

      drawKeypoints(img_escena,keypoints_escena,imgMatch,Scalar::all(-1), DrawMatchesFlags::DEFAULT);

      if(g_avg_dist < 95){
        Mat H;
        if(debug) { t_3 = (double)getTickCount(); }
        H = findHomography(objeto,escena,CV_RANSAC,9);
        if(debug) {
		    t_3 = ((double)getTickCount() - t_3)/getTickFrequency();
		    cout << "Tiempos     [Det/Ext/Hmg] " << t_1 << " " << t_2 << " " << t_3 << endl; 
		}
        vector<Point2f> obj_corners(5);
        obj_corners[0] = cvPoint(0,0); 
        obj_corners[1] = cvPoint(img_objeto.cols,0);
        obj_corners[2] = cvPoint(img_objeto.cols,img_objeto.rows); 
        obj_corners[3] = cvPoint(0,img_objeto.rows);
        obj_corners[4] = cvPoint(img_objeto.cols/2,img_objeto.rows/2);
        vector<Point2f> scene_corners(5);
        perspectiveTransform(obj_corners,scene_corners,H);

        double object_x[4];
        double object_y[4];
        double scene_x[4];
        double scene_y[4];
        int n = 0;
        for(int i = 0; i<4; i++){
          // Recuadro azul
          n = i+1;
          if(i==3){ n = 0; }
          line( imgMatch, scene_corners[i], scene_corners[n], Scalar(255, 0, 0), 6 );
          object_x[i] = obj_corners[i].x;
          object_y[i] = obj_corners[i].y;
          scene_x[i] = scene_corners[i].x;
          scene_y[i] = scene_corners[i].y;
          if(scene_corners[i].x == scene_corners[n].x) m[i] = 2000;	// "infinito"
          else m[i] = (double)(scene_corners[n].y-scene_corners[i].y)/(scene_corners[n].x-scene_corners[i].x);
        }
        if(debug){cout << "Pendientes: m01=" << m[0] << "  m12=" << m[1] << "  m23=" << m[2] << "  m30=" << m[3] << endl;}

        double area_objeto = abs(polygonArea(object_x,object_y,4));
        double area_objeto_detectado = abs(polygonArea(scene_x,scene_y,4));
        if(debug) { cout << "Area        [Obj/Detect]  " << area_objeto << " " << area_objeto_detectado << endl; }
        double area_min_aceptado = area_objeto / factor_tolerancia;
        double area_max_aceptado = area_objeto * factor_tolerancia;
        
        if ( area_min_aceptado > area_objeto_detectado ){
          if(debug) { 
          cout << "[No Detectado] a_o:" << area_objeto_detectado << " a_min:" << area_min_aceptado << " ; " << (area_min_aceptado>area_objeto_detectado) << endl; 
          }
        } else{
          if(debug) {
          cout << "[ DETECTADO  ] " << endl;
          }
          found = true;
          cx=scene_corners[4].x ; cy=scene_corners[4].y; 
          circle( imgMatch, Point(cx,cy), 7, Scalar(0,0,255), -1);
        }

      }
      else {
        if(debug) { 
          cout << "[Bajo threshold] " << endl; 
        }
      }
      } else {
      	drawKeypoints(img_escena, keypoints_escena, imgMatch, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
            if( debug ){
            cout << "[SIN DESCRIPTOR]" << endl;
            }
      }
      return imgMatch;
	}
}


