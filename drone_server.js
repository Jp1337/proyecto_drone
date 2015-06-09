var arDrone = require('ar-drone');
var http    = require('http');
var cl = arDrone.createClient();
var pngStream = arDrone.createClient().getPngStream();

console.log('Conectar stream png ...');

pngStream._frameRate = 12;
pngStream._imageSize = "960x544";

var lastPng;
pngStream
  .on('error', console.log)
  .on('data', function(pngBuffer) {
    lastPng = pngBuffer;
  });

var server = http.createServer(function(req, res) {
  if (!lastPng) {
    res.writeHead(503);
    res.end('No se ha recibido frames aun.');
    return;
  }
  if (req.url == "/takeoff"){
  	cl.takeoff();
  	cl.stop();
  }
  else if(req.url == "/land"){
  	cl.land();
  }
  res.writeHead(200, {'Content-Type': 'image/png'});
  res.writeContinue();
  res.end(lastPng);
});

server.listen(8080, function() {
  console.log('Sirviendo frames.');
});
