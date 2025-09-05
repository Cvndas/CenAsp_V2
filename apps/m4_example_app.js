/*
============================================================
 ASP M4 Example App: Event-Driven HTTP Server
============================================================

  This example demonstrates the use of the ASP M4 event-driven (event loop) HTTP server.

  Features & Protocol:
  --------------------
  - The server is implemented using an event loop (reactor pattern), not threads.
  - Multiple concurrent HTTP connections are handled via non-blocking I/O and event polling.
  - Each incoming HTTP request is parsed and passed as a JS object to the callback.
  - The server must correctly handle the 'Content-Length:' header to determine the size of the request body.
  - The server must handle arbitrary incoming request sizes (not limited to a fixed buffer size).
  - The server must support HTTP/1.1 persistent (keep-alive) connections according to the HTTP/1.1 specification:
      * If the client sends 'Connection: keep-alive' (or omits the Connection header in HTTP/1.1), the server should keep the connection open for further requests.
      * If the client sends 'Connection: close', the server should close the connection after the response.
      * The server should properly parse and handle multiple requests on the same connection (pipelining not required!).
  - The JS callback returns an object with the following structure:
      {
        status: <number>,           // HTTP status code (e.g., 200)
        headers: <object>,          // HTTP headers as key-value pairs
        body: <string>              // Response body (string)
      }
  - The server serializes this object into a proper HTTP response.
  - The request object passed to the callback contains:
      - path: the URL path (e.g., '/foo/bar')
      - method: HTTP method (e.g., 'GET', 'POST')
      - size: size of the request body (if any, as determined by 'Content-Length')
      - body: the request body as a string (if any)

  Example incoming request:
  ------------------------
    POST /index.html HTTP/1.1\n
    Content-Length: 11\n
    hello world
    The callback will receive:
      {
        path: '/index.html',
        method: 'POST',
        size: 11,
        body: 'hello world'
      }

  Example callback response:
  -------------------------
    {
      status: 200,
      headers: { 'Content-Type': 'text/html', 'Connection': 'keep-alive' },
      body: '<html>...</html>'
    }

  Event Loop & Concurrency:
  ------------------------
    - The server uses an event loop (e.g., epoll) to multiplex I/O.
    - No threads are used for request handling; all concurrency is via non-blocking events.
    - Shared state (like the 'counter' variable) is safe to use as all JS callbacks run on the event loop thread.

  To test with curl:
  ------------------
    curl -v http://localhost:8080/index.html
    curl -v -d 'hello world' http://localhost:8080/index.html
    (The server will respond with an HTML page containing the request info and a counter value.)
    curl -v --header 'Connection: keep-alive' http://localhost:8080/index.html
    (The server should keep the connection open for further requests.)
============================================================
*/

let counter = 0;

function getBody(request) {
    return `
        <html>
        <head><title>Hello from MT JS! </title></head>
            <body>
                <h1>Counter: ${counter++}</h1>
                <p>Path: ${request.path}</p>
                <p>Method: ${request.method}</p>
                <p>Size: ${request.size}</p>
            </body>
        </html>
    `;
};

ASP.createEventLoopServer(request => {
    if (request.path === '/favicon.ico') {
        return {
            status: 404,
            headers: {
                'Content-Type': 'text/plain',
                'Connection': 'keep-alive'
            },
            body: 'Not Found'
        };
    } else if (request.path === '/index.html') {
        return {
            status: 200,
            headers: {
                'Content-Type': 'text/html',
                'Connection': 'keep-alive'
            },
            body: getBody(request)
        };
    }
    return {
        status: 200,
        headers: {
            'Content-Type': 'application/json',
            'Connection': 'close'
        },
        body: JSON.stringify({
            path: request.path,
            method: request.method,
            body: request.body
        })
    };
}, 8080);

/*
============================================================
 setInterval API (Event Loop Timers)
============================================================

  The setInterval function allows scheduling a callback to be executed repeatedly
  at a specified interval (in milliseconds). In the M4 event-driven server, this
  must be implemented using timer file descriptors (e.g., timerfd on Linux) and
  integrated with the event loop using epoll.

  Requirements:
  -------------
  - setInterval(fn, ms) should schedule fn to be called every `ms` milliseconds.
  - The implementation must use non-blocking timers and integrate them with the
    event loop (e.g., by adding the timer fd to epoll).
  - Multiple intervals can be scheduled and must be handled concurrently.
  - The callback fn must always run on the event loop thread, never in a separate thread.

  Example usage:
  --------------
    let tick = 0;
    setInterval(function () {
        print(`Event loop is running... ${tick++}`);
    }, 1000)

  Expected behavior:
  ------------------
    - The message 'Event loop is running... N' is printed every second (1000 ms).
    - The event loop remains responsive to HTTP requests while timers are firing.
    - Timers and I/O events are multiplexed in the same event loop.

  Implementation hint:
  --------------------
    - On Linux, use timerfd_create() to create a timer file descriptor.
    - Add the timer fd to the epoll set.
    - When the timer expires, epoll will signal the event, and the callback can be invoked.
    - This approach allows timers and sockets to be handled in a single event loop.
============================================================
*/

let tick = 0;

setInterval(function () {
    print(`Event loop is running... ${tick++}`);
}, 1000)

print('Event-based server started on port 8080');
