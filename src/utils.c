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

#include "utils.h"

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "m1_2__simple_server.h"
#include "m3__multi_threaded_server.h"
#include "m4_5__event_based_server.h"
#include "v8_api_access.h"

#if __has_include(<sys/sdt.h>)
#include <sys/sdt.h>
#else
#define DTRACE_PROBE1(provider, name, arg1) ((void)0)
#define DTRACE_PROBE2(provider, name, arg1, arg2) ((void)0)
#endif

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
 * Execute a JS script via V8                                *
 *************************************************************
 */
JSResult v8_execute_script_file(V8Engine* engine, const char* filename)
{
    FILE* file = fopen(filename, "rb");
    JSResult result = {0};
    if (!file)
        return result;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return result;
    }
    size_t read_size = fread(buffer, 1, size, file);
    fclose(file);
    if (read_size != (size_t)size) {
        free(buffer);
        return result;
    }
    buffer[size] = '\0';
    result = v8_execute_script(engine, buffer);
    free(buffer);
    return result;
}





int server_fd_global = -1;
volatile sig_atomic_t server_running = 1;
static int telemetry_request_count = 0;
static int telemetry_200_responses = 0;

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M5
 *
 * Initializes telemetry counters.
 *
 * You can declare counters as static variables within this file, or use a
 * context struct to hold multiple counters as needed.
 */
void telemetry_init()
{
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
 *  |_|  |_| M5
 *
 * Increments the request count telemetry counter.
 */
void telemetry_increment_request_count()
{
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
 *  |_|  |_| M5
 *
 * Retrieves the current request count telemetry counter.
 */
int telemetry_get_request_count()
{
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
 * Increments the count of HTTP 200 responses sent by the server.
 */
void telemetry_increment_200_responses()
{
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
 *  |_|  |_| M5
 *
 * Retrieves the count of HTTP 200 responses sent by the server.
 */
int telemetry_get_200_responses()
{
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
 * Start the server                                          *
 *************************************************************
 */
void start_server(V8Engine* engine)
{
    HTTPServerType server_type = v8_get_server_type(engine);
    int port = 8080;
    switch (server_type) {
    case HTTPServerTypeSingleThreaded:
        start_single_threaded_server(engine, port);
        break;
    case HTTPServerTypeThreadPool:
        start_server_mt(engine, port, 4);
        break;
    case HTTPServerTypeEventLoop:
        start_server_eb(engine, port);
        break;
    default:
        // ::: Empty template is hitting this line.
        break;
    }
}
