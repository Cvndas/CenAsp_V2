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

#ifndef EVENT_BASED_SERVER_H
#define EVENT_BASED_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

#include "v8_api_access.h"

    int start_server_eb(V8Engine *engine, int port);

#ifdef __cplusplus
}
#endif


typedef struct {
    char *name;
    char *value;
} EvHttpHeader;

#define MAX_HEADERS 32

typedef struct {
    char *method;
    char *path;
    char *body;
    EvHttpHeader headers[MAX_HEADERS];
    int header_count;
} EvHttpRequest;

void register_js_interval_callback(int ms, JSObject cb);

#endif // EVENT_BASED_SERVER_H
