#include <opencv2/core/core.hpp>
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
	
	// Public-domain function by Darel Rex Finley, 2006.
	double polygonArea(double *X, double *Y, int points) {
		double  area=0. ;
		int i, j=points-1  ;
		for (i=0; i<points; i++) {
			area+=(X[j]+X[i])*(Y[j]-Y[i]); j=i; 
			}
		return area*.5; 
	};

	void detectFeatures(Mat &img, vector<KeyPoint> &kpts, int lvls = 4, int cuantos = 500) {
	  OrbFeatureDetector detector(cuantos, 1.2f, lvls, 10, 0, 4, ORB::HARRIS_SCORE, 6);
	  detector.detect(img, kpts);
	};

	void extractFeatures(Mat &img, vector<KeyPoint> &kpts, Mat &descriptors){
	  FREAK extractor;
	  extractor.compute( img, kpts, descriptors );
	}

	Mat matchFeatures(Mat img_objeto, vector<KeyPoint> keypoints_objeto, Mat descriptor_objeto, Mat img_escena, bool debug = true){

		vector<KeyPoint> keypoints_escena;
		vector<DMatch> matches, good_matches;
		Mat descriptor_escena, imgMatch;
		MTCH matcher;
		double t_1, t_2, t_3 = 0;
    	double max_dist = 0; 
	    double min_dist = 100;
    	double avg_dist = 0, g_avg_dist = 0;    
	    double factor_tolerancia = 0.85;
    
		// Detección
		if(debug) { t_1 = (double)getTickCount(); }
		detectFeatures(img_escena,keypoints_escena,16,1000);
		if(debug) { t_1 = ((double)getTickCount() - t_1)/getTickFrequency(); }

		// Extracción
		if(debug) { t_2 = (double)getTickCount(); }   
		extractFeatures(img_escena,keypoints_escena,descriptor_escena);
		if(debug) { t_2 = ((double)getTickCount() - t_2)/getTickFrequency(); }


		// Debe existir un descriptor valido de la escena para continuar
		if(!descriptor_escena.empty()){

		  int ij = clock();
		  // Matching
		  matcher.match(descriptor_objeto, descriptor_escena, matches);
		  cout << "MATCHES! " << matches.size() << endl;
		  // Eliminar outliers calculando distancia entre keypoints
		  for( int i = 0; i < descriptor_objeto.rows; i++ ){ 
		  double dist = matches[i].distance;
		  if( dist < min_dist ) min_dist = dist;
		  if( dist > max_dist ) max_dist = dist;
		  avg_dist += dist;
		  }
		  avg_dist /= descriptor_objeto.rows;
		  sort(matches.begin(),matches.end());	//nos quedaremos con el mejor tercio
		  cout << "asdf: " << (double)(clock()-ij)/CLOCKS_PER_SEC << endl;
		  
		  // Guardar el primer tercio ordenado x distancia
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
      //drawMatches(img_objeto, keypoints_objeto, img_escena, keypoints_escena, 
      //            good_matches, imgMatch, Scalar::all(-1), Scalar::all(-1),
      //             vector<char>(), 0);

      /// Primer filtro basado en matches
      /// Si hay un porcentaje de matches, entonces calcular homography
      if(g_avg_dist < 85){
        Mat H;
        if(debug) { t_3 = (double)getTickCount(); }
        H = findHomography(objeto,escena,CV_RANSAC,9);
        if(debug) {
		    t_3 = ((double)getTickCount() - t_3)/getTickFrequency();
		    cout << "Tiempos     [Det/Ext/Hmg] " << t_1 << " " << t_2 << " " << t_3 << endl; 
		}
        vector<Point2f> obj_corners(4);
        obj_corners[0] = cvPoint(0,0); 
        obj_corners[1] = cvPoint(img_objeto.cols,0);
        obj_corners[2] = cvPoint(img_objeto.cols,img_objeto.rows); 
        obj_corners[3] = cvPoint(0,img_objeto.rows);
        vector<Point2f> scene_corners(4);
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
          //line( imgMatch, scene_corners[i] + Point2f( img_objeto.cols, 0), scene_corners[n] + Point2f( img_objeto.cols, 0), Scalar(255, 0, 0), 6 );
          line( imgMatch, scene_corners[i], scene_corners[n], Scalar(255, 0, 0), 6 );

          // Descomposición de esquinas para evaluar área
          object_x[i] = obj_corners[i].x;
          object_y[i] = obj_corners[i].y;
          scene_x[i] = scene_corners[i].x;
          scene_y[i] = scene_corners[i].y;
        }

        double area_objeto = abs(polygonArea(object_x,object_y,4));
        double area_objeto_detectado = abs(polygonArea(scene_x,scene_y,4));
        if(debug) { cout << "Area        [Obj/Detect]  " << area_objeto << " " << area_objeto_detectado << endl; }
        double tolerance = area_objeto * pow(factor_tolerancia,2);
        double area_min_aceptado = area_objeto - tolerance;
        double area_max_aceptado = area_objeto + tolerance;
        
        if ( !(area_min_aceptado < area_objeto_detectado && area_objeto_detectado < area_max_aceptado)){
          if(debug) { 
          cout << "[No Detectado] " << endl << endl; 
          }
        } else{
          cout << "[ DETECTADO  ] " << endl << endl;
        }

      }
      else {	//si no vale la pena intentar homography
        if(debug) { 
          cout << "[Bajo threshold] " << endl << endl; 
        }
      }
      } else {	//si el descriptor esta vacio
        drawMatches(img_objeto, keypoints_objeto, img_escena, keypoints_escena, 
            good_matches, imgMatch, Scalar::all(-1), Scalar::all(-1),
            vector<char>(), 4);
            cout << "[SIN DESCRIPTOR]" << endl << endl;
      }
      return imgMatch;
	}
}


