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

#include "m3__multi_threaded_server.h"

#include "utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#define MAX_QUEUE 128
#define DEFAULT_THREADS 4
#define MAX_THREADS 64
#define BUFFER_SIZE 1024

typedef struct {
    char *method;
    char *body;
    size_t size;
} MtHttpRequest;

typedef struct ThreadPool {
    int conn_queue[MAX_QUEUE];
    int queue_head, queue_tail, queue_size;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_cond;
    pthread_t threads[MAX_THREADS];
    int num_threads;
    volatile sig_atomic_t running;
    int server_fd;
} ThreadPool;

typedef struct WorkerArgs {
    V8Engine *engine;
    struct ThreadPool *pool;
} WorkerArgs;

struct WorkerRequestData {
    V8Engine *engine;
    char *buffer;
    char *response_buffer;
    size_t response_size;
};

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3
 *
 * Adds a connection file descriptor to the thread pool's connection queue.
 * If the queue is full, the function waits until space is available.
 *
 * Useful APIs and system calls:
 *   - pthread_mutex_lock()   : Lock a mutex for exclusive access.
 *   - pthread_cond_wait()    : Wait for a condition variable to be signaled.
 *   - pthread_mutex_unlock() : Unlock a mutex.
 *   - pthread_cond_signal()  : Signal a condition variable.
 */
static void enqueue_conn(ThreadPool *pool, int connfd) {
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
 *  |_|  |_| M3
 *
 * Removes and returns a connection file descriptor from the thread pool's connection queue.
 * If the queue is empty, the function waits until a connection is available or the server is stopped.
 *
 * Useful APIs and system calls:
 *   - pthread_mutex_lock()   : Locks a mutex for exclusive access.
 *   - pthread_cond_wait()    : Wait for a condition variable to be signaled.
 *   - pthread_mutex_unlock() : Unlock a mutex.
 *   - pthread_cond_signal()  : Signal a condition variable.
 */
static int dequeue_conn(ThreadPool *pool) {
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
 *  |_|  |_| M3
 *
 * Parses a raw HTTP request string and fills an MtHttpRequest structure with the HTTP method and body.
 *
 * Implementation hints:
 *   1. Allocate memory as needed (e.g., for the HTTP method and body).
 *   2. Parse the HTTP method from the raw request (e.g., using sscanf). Useful steps:
 *        a. Search for the end of the HTTP headers ("\r\n\r\n") to locate the start of the body.
 *        b. If a body is present, calculate the body length.
 *        c. If a "Content-Length" header is present, use its value as the body length.
 *
 * Useful APIs and system calls:
 *   - Memory management: memset(), malloc(), free()
 *   - String manipulation and parsing: sscanf(), strstr(), strncpy(), strlen(), strtol()
 */
void parse_http_request_url(V8Engine *engine, const char *raw_request, MtHttpRequest *request) {
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
  * Creates a JS object for a given request                   *
  *************************************************************
*/
JSObject create_js_request_object(V8Engine *engine, const MtHttpRequest *request) {
    JSObject req_obj = v8_create_object(engine);
    if (!req_obj) {
        return NULL;
    }
    if (!v8_set_string_property(engine, req_obj, "method", request->method)) {
        v8_free_object(req_obj);
        return NULL;
    }
    if (request->body) {
        if (!v8_set_string_property(engine, req_obj, "body", request->body)) {
            v8_free_object(req_obj);
            return NULL;
        }
        if (!v8_set_number_property(engine, req_obj, "size", request->size)) {
            v8_free_object(req_obj);
            return NULL;
        }
    } else {
        if (!v8_set_number_property(engine, req_obj, "size", 0)) {
            v8_free_object(req_obj);
            return NULL;
        }
    }
    return req_obj;
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3
 *
 * Handles an HTTP request by invoking a registered JavaScript handler and prepares an HTTP response.
 * This function bridges the C server and the JavaScript handler, using the V8 engine to process the request
 * and generate a response object, which is then serialized into a proper HTTP response string.
 *
 * Note: This method uses functions from `v8_api_utils` such as `create_js_request_object` to read the response data from JavaScript.
 *
 * Implementation hints:
 *   1. Create a JavaScript request object from the parsed HTTP request using `create_js_request_object`.
 *   2. Call the registered JavaScript handler with the request object.
 *   3. Check if the handler call was successful and returned an object.
 *       a. Extract the HTTP status code from the response object.
 *       b. Extract the Content-Type from the response headers.
 *       c. Extract the response body from the response object.
 *  4. Format the HTTP response string with status, headers, and body:
 *       a. Make sure the HTTP response is well-formed (e.g., map status codes to reason phrases).
 *       b. Include necessary headers like Content-Length, Date, Server, and Connection.
 *       c. Handle special cases like HEAD requests (no body).
 *       d. Provide default error bodies for common status codes (e.g., 404, 500).
 *       e. The server name should be 'asp-v8/1.0'
 *
 * Useful APIs and system calls used:
 *   - create_js_request_object()         : Converts the C request to a JS object (from v8_api_utils).
 *   - v8_get_registered_handler_func()   : Gets the registered JS handler.
 *   - v8_call_registered_handler_obj()   : Calls the JS handler with the request object.
 *   - v8_free_object()                   : Frees JS objects handles.
 *   - v8_get_number_property()           : Reads a numeric property from a JS object.
 *   - v8_has_property()                  : Checks if a JS object has a property.
 *   - v8_get_object_property()           : Gets an object property from a JS object.
 *   - v8_get_string_property()           : Gets a string property from a JS object.
 *   - snprintf(), strdup()               : String manipulation.
 */
void handle_request_url(V8Engine *engine, const MtHttpRequest *request, char **response_buffer, size_t *response_size) {
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
  * Initiates request processing                              *
  *************************************************************
*/
int process_request(void *data) {
    struct WorkerRequestData *d = (struct WorkerRequestData *)data;
    V8Engine *engine = d->engine;
    MtHttpRequest request;
    parse_http_request_url(engine, d->buffer, &request);
    handle_request_url(engine, &request, &d->response_buffer, &d->response_size);
    return 0;
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3
 *
 * Creates, configures, binds, and listens on a TCP socket for the multi-threaded server.
 *
 * Implementation hint:
 *   1. Create and initialize a new socket.
 *   2. Listen for incoming connections.
 *   3. Return the file descriptor for the listening socket, or -1 on error.
 *
 * Useful APIs and system calls used:
 *   - socket()         : Create a new socket.
 *   - setsockopt()     : Set socket options.
 *   - bind()           : Bind the socket to an address and port.
 *   - listen()         : Mark the socket as passive to accept connections.
 *   - close()          : Close the socket on error.
 *
 * Returns:
 *   On success: file descriptor of the listening socket.
 *   On failure: -1.
 */
int create_and_bind_socket_mt(int port) {
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
 *  |_|  |_| M3
 *
 * Reads the full HTTP request from a client socket until the end of headers is detected
 * or the connection is closed. Dynamically resizes the buffer as needed to accommodate
 * the incoming data.
 *
 * Implementation hints:
 *   1. Repeatedly read from the socket into a buffer. Handle memory accordingly.
 *   2. Read the HTTP headers.
 *   3. Handle "Content-Length" if present.
 *   4. Implement a timeout mechanism to avoid hanging on slow or unresponsive clients.
 *   5. Ensure the entire request (headers + body) is read.
 *   6. Handle errors and disconnections gracefully.
 *   7. Return the complete request data and its length.
 *
 * Useful APIs and system calls that you may need:
 *   - Memory management: malloc(), realloc(), free()
 *   - Socket: read()
 *   - Strings: strstr(), strlen(), strcpy(), strncpy()
 *
 * Returns:
 *   On success: pointer to the buffer containing the request (must be freed by caller).
 *   On failure: NULL.
 */
char *read_full_request(int connfd, size_t *out_len) {
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
    return NULL;
}

/*
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3
 *
 * Sends an HTTP response to the client and cleans up resources. Handles error cases gracefully.
 *
 * Implementation hints:
 *   1. If a valid response buffer is present, write the response to the client.
 *   2. If no response buffer is present, send a generic HTTP 500 Internal Server Error response.
 *
 * Useful APIs and system calls that you may need:
 *   - Socket : write(), close()
 *   - Memory management: free()
 *   - Strings: strlen(), strcpy(), strcat()
 */
void create_response(int connfd, char *req_buf, struct WorkerRequestData d) {
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
  * Handle a new connection                                   *
  *************************************************************
*/
void handle_connection_mt(V8Engine *engine, int connfd) {
    size_t req_len = 0;
    char *req_buf = read_full_request(connfd, &req_len);
    if (!req_buf) { close(connfd); return; }
    struct WorkerRequestData d = { engine, req_buf, NULL, 0 };
    process_request(&d);
    create_response(connfd, req_buf, d);
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
  * Thread main loop                                          *
  *************************************************************
*/
static void *worker_thread(void *arg) {
    WorkerArgs *args = (WorkerArgs *)arg;
    V8Engine *engine = args->engine;
    ThreadPool *pool = args->pool;
    while (pool->running) {
        int connfd = dequeue_conn(pool);
        if (connfd == -1) break;
        handle_connection_mt(engine, connfd);
    }
    free(args);
    return NULL;
}

/**
  *   __  __
  *  |  \/  |
  *  | \  / |
  *  | |\/| |
  *  | |  | |
  *  |_|  |_| M3
  *
  * Creates a new thread pool for handling connections in the multi-threaded server.
  *
  * Returns:
  *   Pointer to the newly created ThreadPool structure.
*/
ThreadPool *create_thread_pool(int num_threads) {
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
    return NULL;
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
  *                                                           *
  *************************************************************
*/
int start_server_mt(V8Engine *engine, int port, int num_threads) {
    if (num_threads <= 0) num_threads = DEFAULT_THREADS;
    ThreadPool *pool = create_thread_pool(num_threads);
    int server_fd = create_and_bind_socket_mt(port);
    pool->server_fd = server_fd;
    if (server_fd < 0) { free(pool); return 1; }
    for (int i = 0; i < num_threads; ++i) {
        WorkerArgs *args = malloc(sizeof(WorkerArgs));
        args->engine = engine;
        args->pool = pool;
        pthread_create(&pool->threads[i], NULL, worker_thread, args);
    }
    while (pool->running) {
        int new_socket = accept(server_fd, NULL, NULL);
        if (new_socket < 0) {
            if (!pool->running) break;
            perror("Accept failed");
            continue;
        }
        enqueue_conn(pool, new_socket);
    }
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(pool->threads[i], NULL);
    }
    pthread_mutex_destroy(&pool->queue_mutex);
    pthread_cond_destroy(&pool->queue_cond);
    free(pool);
    printf("Multi-threaded server stopped.\n");
    return 0;
}
