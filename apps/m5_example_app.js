/*
============================================================
 ASP M5 Example App: /telemetry Path Handling
============================================================

  This server should be able to handle requests to the /telemetry URL
  internally, and therefore the JS code should not be executed.

============================================================
*/

ASP.createEventLoopServer(req => {
    if (req && req.url === "/telemetry") {
        return {
            status: 200,
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                requests: 42
            })
        };
    } else {
        return {
            status: 404,
            headers: { 'Content-Type': 'text/plain' },
            body: 'Not Found'
        };
    }
}, 8080);

print(`Server started on port 8080`);