// Emular envio de frames
// Esto lee frames de una carpeta "sample_data",
// Para poder trabajar sin un drone

var http    = require('http');
var fs		= require('fs');
var lastPng;
var base = 100051;
var frms = 100127 - base;
var count = 1;
var server = http.createServer(function(req, res) {
  res.writeHead(200, {'Content-Type': 'image/png'});
  res.writeContinue();
  res.end(fs.readFileSync("sample_data/"+(base+(count%frms))+".png")); 
  console.log('Sirviendo imagen n°'+(base+(count%frms))+'.');
  count = count + 1;
});

server.listen(8080, function() {
  console.log('Serving latest png on port 8080 ...');
});
