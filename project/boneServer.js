#!/usr/bin/node
// From Getting Started With node.js and socket.io 
// http://codehenge.net/blog/2011/12/getting-started-with-node-js-and-socket-io-v0-7-part-2/
// This is a general server for the various web frontends
// buttonBox, ioPlot, realtimeDemo
"use strict";

var port = 8080, // Port to listen on
	http = require('http'),
	url = require('url'),
	fs = require('fs'),
	//b = require('bonescript'),
	child_process = require('child_process'),
	server,
	connectCount = 0,	// Number of connections to server
	errCount = 0;	// Counts the AIN errors.

// Initialize various IO things.
function initIO() {
	// Make sure gpios 7 and 20 are available.
	//b.pinMode('P9_42', b.INPUT);
	//b.pinMode('P9_41', b.INPUT);
	//b.pinMode(pwm,	 b.INPUT);	// PWM
	
	// Initialize pwm
}

function send404(res) {
	res.writeHead(404);
	res.write('404');
	res.end();
}

initIO();

server = http.createServer(function (req, res) {
// server code
	var path = url.parse(req.url).pathname;
	console.log("path: " + path);
	if (path === '/') {
		path = '/boneServer.html';
	}

	fs.readFile(__dirname + path, function (err, data) {
		if (err) {return send404(res); }
//			console.log("path2: " + path);
		res.write(data, 'utf8');
		res.end();
	});
});

server.listen(port);
console.log("Listening on " + port);

// socket.io, I choose you
var io = require('socket.io').listen(server);
io.set('log level', 2);

// See https://github.com/LearnBoost/socket.io/wiki/Exposed-events
// for Exposed events

// on a 'connection' event
io.sockets.on('connection', function (socket) {

	console.log("Connection " + socket.id + " accepted.");

	socket.on('led', function (ledNum) {
		var ledPath = "/sys/class/leds/beaglebone:green:usr" + ledNum + "/brightness";
//		console.log('LED: ' + ledPath);
		fs.readFile(ledPath, 'utf8', function (err, data) {
			if(err) throw err;
			data = data.substring(0,1) === "1" ? "0" : "1";
//			console.log("LED%d: %s", ledNum, data);
			fs.writeFile(ledPath, data);
		});
	});
	
	// Send a packet of data every time a 'audio' is received.
	socket.on('matrix', function (i2cNum) {
//		console.log('Got i2c request:' + i2cNum);
		child_process.exec('i2cdump -y -r 0x00-0x0f 1 ' + i2cNum + ' b',
			function (error, stdout, stderr) {
//	  The LED has 8 16-bit values
//				console.log('i2cget: "' + stdout + '"');
		var lines = stdout.split("00: ");
		// Get the last line of the output and send the string
		lines = lines[1].substr(0,47);
		console.log("lines = %s", lines);
				socket.emit('matrix', lines);
				if(error) { console.log('error: ' + error); }
				if(stderr) {console.log('stderr: ' + stderr); }
			});
	});

	socket.on('disconnect', function () {
		console.log("Connection " + socket.id + " terminated.");
		connectCount--;
		console.log("connectCount = " + connectCount);
	});

	connectCount++;
	console.log("connectCount = " + connectCount);
});