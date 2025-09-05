/**
* The MIT License (MIT)
*
* Copyright © 2025 <The VU Amsterdam ASP teaching team>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the “Software”), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute,
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL <The VU Amsterdam ASP teaching team> BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "m4_5__event_based_server.h"

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/timerfd.h>
#include <ctype.h>

#define MAX_EVENTS 64
#define READ_BUFFER_SIZE 1024

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Convenience globals for the event-based server.
 * Ideally, they should not be static, but exposed via a context struct.
 * For simplicity, we keep them static here. Feel free to refactor.
 */
static volatile sig_atomic_t server_running_eb = 1;
static int server_fd_global_eb = -1;
static JSObject interval_callback = NULL;
static int timer_fd = -1;
static int interval_ms = 1000;


/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Sets or updates the interval timer for the event-based server using a timer file descriptor.
 *
 * APIs and system calls used:
 *   - `timerfd_settime`: Linux system call to set the expiration interval of a timer file descriptor.
 *   - `struct itimerspec`: POSIX structure to specify timer intervals and initial expiration.
 */
void initialize_timer(int ms) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}

/*
  *************************************************************
  *                                                           *
  *    █████╗ ███████╗██████╗                                 *
  *   ██╔══██╗██╔════╝██╔══██╗                                *
  *   ███████║███████╗██████╔╝                                *
  *   ██╔══██║╚════██║██╔═══╝                                 *
  *   ██║  ██║███████║██║                                     *
  *   ╚═╝  ╚═╝╚══════╝╚═╝                                     *
  *                                                           *
  * Called from SetIntervalImpl to register the callback      *
  * provided to V8                                            *
  *************************************************************
*/
void register_js_interval_callback(int ms, JSObject cb) {
    interval_callback = cb;
    interval_ms = ms;
    initialize_timer(ms);
}


/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Parses a raw HTTP request string and fills an EvHttpRequest struct with the method, path, and body.
 * This method is analogous to the one in m3__multi_threaded_server.c, but also parses headers.
 *
 * Implementation hints:
 *   1. Parse the HTTP method and path from the raw request (e.g., using sscanf).
 *   2. Parse HTTP headers and store them in the EvHttpRequest struct.
 *   3. Handle headers, e.g. "Content-Length".
 *   4. Handle errors and clean up allocated memory in case of failures.
 *   5. Ensure all allocated memory is freed in case of errors.
 *   6. The function should be robust against malformed requests.
 *   7. The function should handle requests with no/invalid headers, no body (etc.) gracefully.
 *
 * Useful APIs and system calls that you may need:
 *   - Memory management: malloc(), free()
 *   - String manipulation and parsing: sscanf(), strstr(), strncpy(), strlen(), strtol(), etc
 */
void parse_http_request_with_header(const char *raw_request, EvHttpRequest *request) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Handles an HTTP request with keep-alive support by creating a JS object from the request,
 * calling a registered JavaScript handler, and preparing the response.
 * This method is analogous to the one in m3__multi_threaded_server.c, but also handles keep-alive.
 * You may reuse code from m3__multi_threaded_server.c as needed.
 *
 * Implementation hints:
 *   1. Create a JSObject from the request data (method, path, body).
 *   2. Retrieve the registered JavaScript handler function pointer.
 *   3. Call the handler with the request object and get the response object.
 *   4. Extract status, content type, and body from the response object.
 *   5. Construct an HTTP response string with appropriate headers and body.
 *   6. Handle keep-alive settings in the "Connection" header.
 *   7. Ensure proper error handling and default values.
 *
 * Useful APIs and system calls that you may need:
 *   - v8_create_object: to create a new JS object.
 *   - v8_set_string_property: to set properties on the JS object.
 *   - v8_get_registered_handler_func: to get the registered handler function pointer.
 *   - v8_call_registered_handler_obj: to call the handler with the request object.
 *   - v8_get_number_property: to retrieve numeric properties from the response object.
 */
static void handle_request(V8Engine *engine, const EvHttpRequest *request, char **response_buffer, size_t *response_size, int keep_alive) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}


/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Parses the HTTP request headers to determine keep-alive settings.
 *
 * Implementation hints:
 *   1. Tokenize the input buffer to extract the HTTP method, path, and version.
 *   2. Iterate through the headers to find "Connection" and "Keep-Alive" headers.
 *   3. Set keep-alive settings based on the HTTP version and connection
 *   4. Parse the keep-alive header for timeout and max values if present.
 *   5. Handle errors and malformed headers gracefully.
 *
 * APIs and system calls that you may want to use:
 * - String manipulation functions: strtok(), strncasecmp(), atoi()
 */
static void parse_keep_alive_headers(const char *buffer, char **http_version, char **connection_hdr, char **keep_alive_hdr, int *keep_alive, int *keep_alive_timeout, int *keep_alive_max) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}


/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Handles timer events by reading the timer file descriptor and calling the interval callback.
 *
 * Implementation hints:
 *  1. Read the number of expirations from the timer file descriptor.
 *  2. If an interval callback is registered, call it using the V8 engine.
 *
 * APIs and system calls used:
 *   - `v8_call_function_no_arguments`: to call the JavaScript function registered as the interval callback.
 */
static void handle_timer_event(V8Engine *engine) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Accepts a new client connection and adds it to the epoll instance.
 *
 * Implementation hints:
 *   1. Accept a new client connection on the server socket.
 *   2. Set the client socket to non-blocking mode.
 *   3. Initialize an epoll event structure for the client socket.
 *   4. Add the client socket to the epoll instance.
 *   5. Handle errors appropriately, ensuring resources are cleaned up if any step fails.
 *   6. Ensure the server can handle multiple simultaneous connections efficiently.
 *   7. Consider edge cases, such as reaching the maximum number of file descriptors.
 *
 * APIs and system calls that you may need:
 * - Socket operations: accept(), fcntl()
 * - Epoll operations: epoll_ctl()
 */
static void handle_new_connection(int server_fd, int epoll_fd, struct epoll_event *ev) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Reads data from a client socket and validates the request.
 *
 * Implementation hints:
 *   1. Reads data from the client socket into a buffer.
 *   2. If the read operation fails or returns zero, closes the socket and removes it from the epoll instance.
 *
 * APIs and system calls that you may need:
 * - Socket operations: read(), close()
 * - Epoll operations: epoll_ctl()
 */
static int read_and_validate_client_request(int fd, char *buffer, struct epoll_event *ev, int epoll_fd) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
    return 0;
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M5
 *
 * Handles HTTP requests to the `/telemetry` endpoint, returning server telemetry data as a JSON response.
 *
 * Implementation hints:
 *  1. Check if the HTTP method is "GET" and the path is "/telemetry".
 *  2. If so, retrieve the current request count. Note: the counter will have to be updated somewhere else.
 *  3. Format the telemetry data as a JSON string.
 *  4. Construct an HTTP response with the JSON body, appropriate headers, and connection handling.
 *  5. Write the response to the client socket.
 *  6. If the connection is not keep-alive, close the client socket and remove it from the epoll instance.
 *  7. Return 1 if the request was handled, 0 otherwise.
 *  8. Ensure proper error handling and resource management.
 *
 * APIs and system calls that you may need:
 * - String manipulation: snprintf(), strcmp()
 * - Socket operations: write(), close()
 * - Epoll operations: epoll_ctl()
 * - telemetry_get_request_count: to get the number of handled requests.
 * - telemetry_get_start_time: to get the server's start time.
 */
static int handle_telemetry_endpoint(int fd, int epoll_fd, EvHttpRequest *request, int keep_alive) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
    return 0;
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Writes an HTTP response to the given file descriptor, handling both successful and error responses.
 *
 * Implementation hints:
 *   1. Check if the provided response buffer is not NULL.
 *   2. If the buffer is valid, write the response to the client and frees the buffer.
 *   3. If the buffer is NULL, write a generic HTTP 500 Internal Server Error response to the client.
 *
 */
static void write_response(int fd, char *response_buffer, size_t response_size) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}


/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Handles a generic HTTP request by validating headers, invoking the request handler,
 * and sending the appropriate response back to the client.
 *
 * Implementation hints:
 *   1. Validate the presence of the "Host" header; if missing, respond with a 400 Bad Request.
 *   2. Determine if the request method is "HEAD" to handle it accordingly.
 *   3. Call `handle_request` to process the request and obtain the response buffer and size.
 *   4. If a response buffer is returned, add a "Date" header to it.
 *   5. If the request method is "HEAD", ensure that only headers are sent in the response.
 *   6. Write the response to the client using `write_response`.
 *   7. If keep-alive is not requested, close the client socket and remove it from the epoll instance.
 *
 * APIs and system calls that you may need:
 * - String manipulation: strcmp(), strstr(), snprintf(), memcpy()
 * - Socket operations: write(), close()
 * - Epoll operations: epoll_ctl()
 *
 */
static void handle_generic_request(V8Engine *engine, int fd, int epoll_fd, EvHttpRequest *request, int keep_alive) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}

/*
  *************************************************************
  *                                                           *
  *    █████╗ ███████╗██████╗                                 *
  *   ██╔══██╗██╔════╝██╔══██╗                                *
  *   ███████║███████╗██████╔╝                                *
  *   ██╔══██║╚════██║██╔═══╝                                 *
  *   ██║  ██║███████║██║                                     *
  *   ╚═╝  ╚═╝╚══════╝╚═╝                                     *
  *                                                           *
  *************************************************************
*/
static void cleanup_request(EvHttpRequest *request) {
    for (size_t i = 0; i < request->header_count; i++) {
        free(request->headers[i].name);
        free(request->headers[i].value);
    }

    free(request->method);
    free(request->path);
    free(request->body);
}

/*
  *************************************************************
  *                                                           *
  *    █████╗ ███████╗██████╗                                 *
  *   ██╔══██╗██╔════╝██╔══██╗                                *
  *   ███████║███████╗██████╔╝                                *
  *   ██╔══██║╚════██║██╔═══╝                                 *
  *   ██║  ██║███████║██║                                     *
  *   ╚═╝  ╚═╝╚══════╝╚═╝                                     *
  *                                                           *
  * Initial handling of client requests                       *
  *************************************************************
*/
static void handle_client_event(V8Engine *engine, int fd, struct epoll_event *ev, int epoll_fd) {
    char buffer[READ_BUFFER_SIZE] = { 0 };
    if (!read_and_validate_client_request(fd, buffer, ev, epoll_fd)) return;
    EvHttpRequest request = { 0 };
    parse_http_request_with_header(buffer, &request);
    int keep_alive, keep_alive_timeout, keep_alive_max;
    char *http_version = NULL;
    char *connection_hdr = NULL;
    char *keep_alive_hdr = NULL;
    parse_keep_alive_headers(buffer, &http_version, &connection_hdr, &keep_alive_hdr, &keep_alive, &keep_alive_timeout, &keep_alive_max);
    if (handle_telemetry_endpoint(fd, epoll_fd, &request, keep_alive)) {
        cleanup_request(&request);
        return;
    }
    handle_generic_request(engine, fd, epoll_fd, &request, keep_alive);
    cleanup_request(&request);
}


/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Sets up a timer file descriptor for periodic events in the event-based server.
 *
 * Steps performed:
 *   1. Creates a timer file descriptor using `timerfd_create`.
 *   2. Configures the timer to trigger at specified intervals using `timerfd_settime`.
 *
 * APIs and system calls used:
 *   - `timerfd_create`: to create a timer file descriptor.
 *   - `timerfd_settime`: to set the timer's expiration interval.
 */
static int setup_timer_fd() {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}


/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Sets up a server socket file descriptor for listening on the specified port.
 *
 * Implementation hints:
 *   1. Create a socket and set the socket options to allow address reuse and non-blocking mode.
 *   2. Bind the socket to the specified port and address and listen for incoming connections.
 *
 * APIs and system calls that you may need:
 * - Socket operations: socket(), setsockopt(), fcntl(), bind(), listen()
 */
static int setup_server_fd(int port) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4
 *
 * Sets up an epoll file descriptor and adds the server and timer file descriptors to it.
 *
 * Implementation hints:
 *   1. Create an epoll instance using and initialize an epoll event structure for the server socket.
 *   3. Add the server socket to the epoll instance.
 *   4. Initialize another epoll event structure for the timer file descriptor.
 *   5. Add the timer file descriptor to the epoll instance.
 *
 * APIs and system calls that you may need:
 * - Epoll operations: epoll_create1(), epoll_ctl()
 */
static int setup_epoll_fd(int server_fd, int timer_fd, struct epoll_event *ev, struct epoll_event *events) {
    /**
        ┏━━━━┓┏━━━┓━┏━━━┓┏━━━┓
        ┃┏┓┏┓┃┃┏━┓┃━┗┓┏┓┃┃┏━┓┃
        ┗┛┃┃┗┛┃┃━┃┃ ━┃┃┃┃┃┃━┃┃
        ━━┃┃━━┃┃━┃┃━━┃┃┃┃┃┃━┃┃
        ━┏┛┗┓━┃┗━┛┃━┏┛┗┛┃┃┗━┛┃
        ━┗━━┛━┗━━━┛━┗━━━┛┗━━━┛
        ━━━━━━━━━━━━━━━━━━━━━
        ━━━ Your code here...
        ━━━━━━━━━━━━━━━━━━━━━
        */
}

/*
  *************************************************************
  *                                                           *
  *    █████╗ ███████╗██████╗                                 *
  *   ██╔══██╗██╔════╝██╔══██╗                                *
  *   ███████║███████╗██████╔╝                                *
  *   ██╔══██║╚════██║██╔═══╝                                 *
  *   ██║  ██║███████║██║                                     *
  *   ╚═╝  ╚═╝╚══════╝╚═╝                                     *
  *                                                           *
  * Main event loop                                           *
  *************************************************************
*/
static void event_loop(V8Engine *engine, int server_fd, int timer_fd, int epoll_fd, struct epoll_event *ev, struct epoll_event *events) {
    while (server_running_eb) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
        if (nfds == -1) {
            if (!server_running_eb) break;
            perror("epoll_wait");
            continue;
        }
        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == timer_fd) {
                handle_timer_event(engine);
                continue;
            }
            if (events[n].data.fd == server_fd) {
                handle_new_connection(server_fd, epoll_fd, ev);
            } else {
                handle_client_event(engine, events[n].data.fd, ev, epoll_fd);
            }
        }
    }
}


/*
  *************************************************************
  *                                                           *
  *    █████╗ ███████╗██████╗                                 *
  *   ██╔══██╗██╔════╝██╔══██╗                                *
  *   ███████║███████╗██████╔╝                                *
  *   ██╔══██║╚════██║██╔═══╝                                 *
  *   ██║  ██║███████║██║                                     *
  *   ╚═╝  ╚═╝╚══════╝╚═╝                                     *
  *                                                           *
  * Server startup                                            *
  *************************************************************
*/
int start_server_eb(V8Engine *engine, int port) {
    telemetry_init();
    timer_fd = setup_timer_fd();
    if (timer_fd == -1) return 1;
    int server_fd = setup_server_fd(port);
    server_fd_global_eb = server_fd;
    if (server_fd < 0) return 1;
    struct epoll_event ev, events[MAX_EVENTS];
    int epoll_fd = setup_epoll_fd(server_fd, timer_fd, &ev, events);
    if (epoll_fd == -1) return 1;
    event_loop(engine, server_fd, timer_fd, epoll_fd, &ev, events);
    if (timer_fd != -1) close(timer_fd);
    if (server_fd != -1) close(server_fd);
    if (epoll_fd != -1) close(epoll_fd);
    printf("Event-based server stopped.\n");
    return 0;
}
