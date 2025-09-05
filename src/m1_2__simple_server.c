/**
 * The MIT License (MIT)
 *
 * Copyright © 2025 <The VU Amsterdam ASP teaching team>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL <The
 * VU Amsterdam ASP teaching team> BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "m1_2__simple_server.h"
#include "v8_api_access.h"

#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"

#include <strings.h>

#define BUFFER_SIZE 1024

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M2
 *
 * Creates a TCP server socket, binds it to the specified port, and starts
 * listening for connections.
 *
 * This function performs the following steps:
 * 1. Creates a new socket using the IPv4 address family and TCP protocol.
 * 2. Sets an option to allow the socket to be quickly reused.
 * 3. Binds the socket to all available network interfaces on the given port.
 * 4. Starts listening for incoming client connections.
 * If any step fails, it prints an error message, closes the socket if
 * necessary, and returns -1. On success, it returns the file descriptor of the
 * listening server socket.
 *
 * Relevant API and system calls used:
 * - socket(2): Creates a new socket.
 * - setsockopt(2): Sets options.
 * - bind(2): Binds the socket to an address and port.
 * - listen(2): Marks the socket as passive to accept incoming connections.
 * - close(2): Closes the socket file descriptor.
 *
 * @param port The port number to bind the server socket to.
 * @return The file descriptor of the listening server socket, or -1 on error.
 */
static int create_and_bind_socket(int port) {
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
 *  |_|  |_| M2
 *
 * Reads data from a socket until the sequence '42' (ASCII characters '4' and
 * '2') is found.
 *
 * This function reads data from the given socket file descriptor. It continues
 * reading and manages memory as needed until the sequence '42' is detected in
 * the incoming data or the connection is closed. The function then
 * null-terminates the data at the position immediately after '42', sets the
 * output length (if provided), and returns the data. The caller is responsible
 * for freeing the returned memory.
 *
 * Relevant API and system calls used:
 * - read(2): Reads data from the socket file descriptor.
 * - malloc(3), realloc(3), free(3): Manages memory allocation.
 *
 * @param socket_fd The file descriptor of the socket to read from.
 * @param out_len Pointer to a size_t to store the length of the data read
 * (excluding the null terminator).
 * @return Pointer to the data up to and including '42', or NULL on error.
 */
static char* read_until_42(int socket_fd, size_t* out_len) {
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

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M2
 *
 * Sends a response to the connected client and performs cleanup.
 *
 * This function writes the result of a JavaScript handler execution to the
 * client socket if the result is a successful string. It then frees the buffer
 * used for reading the client request and closes the client socket to complete
 * the connection.
 *
 * Relevant API and system calls used:
 * - write(2): Writes data to the client socket.
 * - free(3): Frees dynamically allocated memory.
 * - close(2): Closes the client socket file descriptor.
 *
 * @param new_socket The file descriptor for the connected client socket.
 * @param result Pointer to a JSResult structure containing the handler
 * execution result.
 * @param buffer Pointer to the buffer holding the client request data (to be
 * freed).
 */
static void client_response(int new_socket, JSResult* result, char* buffer) {
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
 * Handles requests calling V8                               *
 *************************************************************
 */
static void handle_client(V8Engine* engine, int new_socket) {
    size_t data_len = 0;
    char* buffer = read_until_42(new_socket, &data_len);
    if (!buffer) {
        perror("Failed to read from socket");
        close(new_socket);
        return;
    }
    void* handler_ptr = v8_get_registered_handler_func(engine);
    if (!handler_ptr) {
        fprintf(stderr,
                "No JS handler registered. Did you call ASP.createServer in "
                "your script?\n");
        free(buffer);
        close(new_socket);
        return;
    }
    JSResult result = v8_call_registered_handler_string(engine, buffer);
    client_response(new_socket, &result, buffer);
    if (result.type == JS_STRING) {
        free(result.value.str_result);
    }
}










/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M2
 *
 * Accepts an incoming client connection on the given server socket.
 *
 * This function waits for a new connection attempt on the specified server file
 * descriptor (`server_fd`). When a client attempts to connect, it accepts the
 * connection and returns a new socket file descriptor for communication with
 * the client. If the accept operation fails, it prints an error message and
 * returns -1.
 *
 * Relevant API and system calls used:
 * - accept(2): Accepts a connection on a socket.
 * - perror(3): Prints a description for the last error that occurred.
 *
 * @param server_fd The file descriptor of the listening server socket.
 * @return The file descriptor for the accepted client socket, or -1 on error.
 */
static int accept_client_connection(int server_fd) {
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
 * Starts the server and accepts incoming requests           *
 *************************************************************
 */
int start_single_threaded_server(V8Engine* engine, int port) {
    int server_fd = create_and_bind_socket(port);
    server_fd_global = server_fd;
    if (server_fd < 0)
        return 1;
    while (server_running) {
        int new_socket;
        if ((new_socket = accept_client_connection(server_fd)) < 0) {
            if (!server_running)
                break;
            continue;
        }
        handle_client(engine, new_socket);
    }
    printf("Server stopped.\n");
    return 0;
}
