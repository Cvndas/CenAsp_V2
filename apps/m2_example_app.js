/*
============================================================
 ASP M2 Example App: Simple Socket Server
============================================================

  This example demonstrates the use of the ASP M2 simple socket server.

  Protocol:
  ---------
  - The server reads raw data from a socket connection.
  - Reading stops as soon as the sequence '42' is found.
  - All data up to and including '42' is passed to the JS callback.

  Example incoming message:
  ------------------------
    'hello world42'
    (The server will call the callback with 'hello world42')

  To test with netcat:
  --------------------
    echo -n 'hello world42' | nc localhost 8080
    (The server will print 'Received request: hello world42' and respond with the counter message.)
============================================================
*/

let counter = 0;

ASP.createServer(data => {
    // 'data' is the string received from the socket, up to and including the first occurrence of '42'.
    print(`Received request: ${data}`);
    // Respond with a message including the received data and a counter.
    return `You asked '${data}', my answer is: ${counter++}\n`;
}, 8080);

/*
============================================================
 syncCallBack Functionality
============================================================

  The syncCallBack function is used to execute a callback synchronously.

  In this example, syncCallBack is used to print a message to the console
  once the server is up and listening on the specified port.

  Expected behavior:
  ------------------
    - The callback passed to syncCallBack should be called immediately.
    - The string message ("Socket server started on port 8080") should
      be passed as an argument to the callback.
    - In this case, the callback prints this message to the console.
    - Only one argument should be passed to the callback.

  Example output:
  ---------------
    Socket server started on port 8080
============================================================
*/

syncCallBack(s => {
    print(s);
}, "Socket server started on port 8080");
