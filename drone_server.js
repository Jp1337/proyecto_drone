//enviar frames con el drone y servirlos en localhost:8080
var http    = require('http');
var arDrone = require('ar-drone');
var cl = arDrone.createClient();
var pngStream = arDrone.createClient().getPngStream();
var count = 0;
var lastPng;

console.log('Conectando stream...');

cl.takeoff();
cl.stop();

pngStream
  .on('error', console.log)
  .on('data', function(pngBuffer) {
    lastPng = pngBuffer;
    count = count+1;
    if(count%100==0)
    	console.log(count);
  });
  
pngStream._frameRate = 15;
pngStream._imageSize = "640x480";

var server = http.createServer(function(req, res) {
  if (!lastPng) {
    res.writeHead(503);
    res.end('Esperate un cachito y aplica f5');
    return;
  }

  res.writeHead(200, {'Content-Type': 'image/png'});
  res.end(lastPng);
});

server.listen(8080, function() {
  console.log('Sirviendo...');
});

