var arDrone = require('ar-drone');
var http    = require('http');
var cl = arDrone.createClient();
var pngStream = arDrone.createClient().getPngStream();

console.log('Conectar stream png ...');

pngStream._frameRate = 12;
pngStream._imageSize = "883x500";//"960x544";

var lastPng;
pngStream
  .on('error', console.log)
  .on('data', function(pngBuffer) {
    lastPng = pngBuffer;
  });

var server = http.createServer(function(req, res) {
  console.log(req.url);
  if(req.url == "/"){
  	if (lastPng){
	  	res.writeHead(200, {'Content-Type': 'image/png'});
  		res.end(lastPng);
  		return;
  	}
  	else res.end('No se ha recibido frames aun.');
  	return;
  }
  else if (req.url == "/hold"){
  	cl.stop();
  }
  else if(req.url == "/land"){
  	cl.land();
  }
  else if(req.url == "/vuelta_der"){
	cl.after(150, function() { this.clockwise(0.2); } );
  }
  else if(req.url == "/vuelta_izq"){
	cl.after(150, function() { this.counterClockwise(0.2); } );
  }  
  else if(req.url == "/sube"){
	cl.after(150, function() { this.up(0.2); } );
  }
  else if(req.url == "/baja"){
	cl.after(150, function() { this.down(0.2); } );
  }
  else if(req.url == "/no_spin"){
	cl.counterClockwise(0);
  }
  else if(req.url == "/no_updn"){
	cl.up(0);
  }
  else if (req.url == "/takeoff"){
  	cl.takeoff();
  	cl.after(250, function() { this.stop(); } );
  }
  res.end();
  return;
});

server.listen(8080, function() {
  console.log('Sirviendo frames.');
});


