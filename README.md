Este proyecto contempla la realización de un sistema para estabilización y detección de objetos simples con un Parrot Ar-Drone 2.0

Dependencias: 

 - curlpp 0.7.3
 - node.js
 - npm (Si se quiere instalar ar-drone a través de éste, recomendado)
 - ar-drone.js (https://github.com/felixge/node-ar-drone)


 
 
Para compilar en un sistema linux o unix-like con las dependencias mencionadas ya instaladas

 - g++ -lcurl -lcurlpp -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_calib3d drone_control.cpp
