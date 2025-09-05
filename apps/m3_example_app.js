/*
============================================================
 ASP M3 Example App: Multi-Threaded HTTP Server
============================================================

  This example demonstrates the use of the ASP M3 multi-threaded HTTP server.

  Features & Protocol:
  --------------------
  - The server uses a thread pool to handle multiple HTTP requests concurrently.
  - Each incoming HTTP request is parsed and passed as a JS object to the callback.
  - The server must correctly handle the 'Content-Length:' header to determine the size of the request body.
  - The server must handle arbitrary incoming request sizes (not limited to a fixed buffer size).
  - The JS callback returns an object with the following structure:
      {
        status: <number>,           // HTTP status code (e.g., 200)
        headers: <object>,          // HTTP headers as key-value pairs
        body: <string>              // Response body (string)
      }
  - The server serializes this object into a proper HTTP response.
  - The request object passed to the callback contains:
      - method: HTTP method (e.g., 'GET', 'POST')
      - size: size of the request body (if any, as determined by 'Content-Length')
      - body: the request body as a string (if any)

  Example incoming request:
  ------------------------
    POST /foo/bar HTTP/1.1\n
    Content-Length: 11\n
    hello world
    The callback will receive:
      {
        method: 'POST',
        size: 11,
        body: 'hello world'
      }

  Example callback response:
  -------------------------
    {
      status: 200,
      headers: { 'Content-Type': 'application/json' },
      body: '{"message": "...", ...}'
    }

  Concurrency:
  ------------
    - The server uses a thread pool to process requests in parallel.
    - Each request is handled in a separate thread.
    - JavaScript execution does not happen in parallel. Access to V8 must be synchronized and managed carefully.

  To test with curl:
  ------------------
    curl -v -d 'hello world' http://localhost:8080/foo/bar
    (The server will respond with a JSON object containing the request info and a counter value.)
============================================================
*/

let counter = 0;

function getBody(request) {
    return `
        <html>
        <head><title>Hello from MT JS! </title></head>
            <body>
                <h1>Counter: ${counter++}</h1>
                <p>Method: ${request.method}</p>
                <p>Size: ${request.size}</p>
            </body>
        </html>
    `;
};

ASP.createThreadPoolServer(request => {
    return {
        status: 200,
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            message: getBody(request),
            method: request.method,
            body: request.body
        })
    };
}, 8080);

print(`Multi-threaded server started on port 8080`);