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

#include <asm-generic/socket.h>
#include <assert.h>
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
// TODO: Implement M2
static int create_and_bind_socket(int port)
{
   dprintFuncEntry();

   int serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
   if (serverSocketFd == -1) {
      perror("Failed to create socket");
      return serverSocketFd;
   }

   // ::: The SO_REUSEADDR allows reuse of the entire address, so both ipv4 and port.
   // --- source: https://www.baeldung.com/linux/socket-options-difference
   int enabled = 1;
   int sockOptResult =
      setsockopt(serverSocketFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&enabled, sizeof(enabled));
   if (sockOptResult == -1) {
      perror("Failed to set sockOpt for SO_REUSEADDR");
      goto CLEANUP_SOCKET_ON_ERROR;
   }

   // ::: Building the address struct
   struct sockaddr_in serverAddress = {
      .sin_addr.s_addr = htonl(INADDR_ANY), // ::: sin_addr is another struct with a single entry: s_addr
      .sin_port = htons((u16)port),
      .sin_family = AF_INET,
   };

   // ::: Binding the socket to the address
   int bindResult = bind(serverSocketFd, (const struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in));
   if (bindResult == -1) {
      perror("Failed to bind socket to the address");
      goto CLEANUP_SOCKET_ON_ERROR;
   }

   // ::: Marking the socket as passive listener
   int listenResult = listen(serverSocketFd, 1);
   if (listenResult == -1) {
      perror("Failed to mark socket as passive listener");
      goto CLEANUP_SOCKET_ON_ERROR;
      return -1;
   }

   // ::: -------------------------:: Success! ::------------------------- ::: //
   dprintFuncExit();
   return serverSocketFd;




CLEANUP_SOCKET_ON_ERROR:
   // ::: -------------------------:: Failure... ::------------------------- ::: //
   fprintf(stderr, "Closing socket...");
   close(serverSocketFd);
   dprintFuncExit();
   return -1;
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
// TODO: Implement M2
static char* read_until_42(int socket_fd, size_t* out_len)
{
   dprintFuncEntry();

   // ::: NULL by default, only updated on success
   char* ret = NULL;

   size_t bytesRead = 0;
   size_t bufferSize = 128;
   size_t readIndex = 0;

   char* readBuffer = (char*)malloc(bufferSize);
   if (!readBuffer) {
      fprintf(stderr, "Malloc failed");
      goto RETURN;
   }

   // ::: TODO: Keep reading for as long as data is available, or until 2 is read. Then check previous char.
   // If it's
   // --- 4, copy to new string with appropriate size, and put a null terminator at the end.
   while (true) {
      bytesRead = read(socket_fd, readBuffer + bytesRead, bufferSize);

      // ::: EOF, which means disconnection in the case of TCP socket.
      if (bytesRead == 0) {
         dprint("The client disconnected.");
         goto FREE_BUFFER;
      }
      // ::: Error
      else if (bytesRead == -1) {
         perror("An error occured while reading from socket");
         goto FREE_BUFFER;
      }

      { // ::: Search for the string "42" in the read data
         // TODO: Debug for off-by-one error. < or <=, not sure which right now.
         while (readIndex < bufferSize) {
            bool fourtwoPatternFound =
               (readBuffer[readIndex] == '2') && (readIndex >= 1) && (readBuffer[readIndex - 1] == '4');
            if (fourtwoPatternFound) {

               // ::: We have read everything there is to read. Whatever was left in the buffer is garbage.
               size_t actualStringLength =
                  readIndex + 1 + NULL_TERMINATOR_SIZE; // + 1 because readIndex starts at 0.
               ret = (char*)malloc(sizeof(char) * actualStringLength);
               if (!ret) {
                  fprintf(stderr, "Malloc failed");
                  goto FREE_BUFFER;
               }

               assert(actualStringLength == (readIndex + 2));
               assert(bufferSize >= (readIndex + 1));

               // ::: Memsetting to 99 so I can inspect memory and verify that ever 99 is overwritten
               memset(ret, 99, sizeof(char) * actualStringLength);
               memcpy(ret, readBuffer, readIndex + 1);

               ret[actualStringLength - 1] = '\0';

               goto FREE_BUFFER;
            }


            readIndex++;
         }
      }

      // ::: There's PROBABLY still data in the socket. Let's realloc so we have enough space to read it.
      if (bytesRead == bufferSize) {
         bufferSize *= 2;
         char* reallocPtr = (char*)realloc(readBuffer, bufferSize);
         if (!reallocPtr) {
            fprintf(stderr, "realloc() failed");
            goto FREE_BUFFER;
         }
         readBuffer = reallocPtr;
      }
   }


FREE_BUFFER:
   free(readBuffer);

RETURN:
   dprintFuncExit();
   return ret;
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
// TODO implement m2
static void client_response(int new_socket, JSResult* result, char* buffer)
{
   dprintFuncEntry();

   dprintFuncExit();
   return;
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
static void handle_client(V8Engine* engine, int new_socket)
{
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
static int accept_client_connection(int server_fd)
{
   dprintFuncEntry();
   struct sockaddr_in clientAddress = {0};
   socklen_t addressLength = sizeof(struct sockaddr_in);

   int acceptResult = accept(server_fd, (struct sockaddr*)&clientAddress, &addressLength);
   if (acceptResult == -1) {
      perror("Failed to accept connection");
      return -1;
   }
   // ::: No error, so acceptResult represents the fd of the new socket for this connection.
   dprint("Accepted a connection.");

   dprintFuncExit();
   return acceptResult;
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
int start_single_threaded_server(V8Engine* engine, int port)
{
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
