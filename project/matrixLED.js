var socket;
var firstconnect = true;
disp = [];


//socket.emit('i2cset', {i2cNum: i2cNum, i: 2*i, 
//			 disp: '0x'+disp[i].toString(16)});

function connect() {
  if(firstconnect) {
	socket = io.connect(null);

	// See https://github.com/LearnBoost/socket.io/wiki/Exposed-events
	// for Exposed events
	socket.on('message', function(data)
		{ status_update("Received: message " + data);});
	socket.on('connect', function()
		{ status_update("Connected to Server"); });
	socket.on('disconnect', function()
		{ status_update("Disconnected from Server"); });
	socket.on('reconnect', function()
		{ status_update("Reconnected to Server"); });
	socket.on('reconnecting', function( nextRetry )
		{ status_update("Reconnecting in " + nextRetry/1000 + " s"); });
	socket.on('reconnect_failed', function()
		{ message("Reconnect Failed"); });
	firstconnect = false;
  }
  else {
	socket.socket.reconnect();
  }
}

function disconnect() {
  socket.disconnect();
}

function status_update(txt){
$('#status').html(txt);
}

connect();
