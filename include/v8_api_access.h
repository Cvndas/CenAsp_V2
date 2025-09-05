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

#ifndef V8_WRAPPER_H
#define V8_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct JSObjectHandle *JSObject;

    typedef struct V8EngineHandle V8Engine;

    typedef enum {
        JS_UNDEFINED,
        JS_NULL,
        JS_NUMBER,
        JS_STRING,
        JS_OBJECT
    } JSValueType;

    typedef struct {
        int success;
        JSValueType type;
        union {
            int int_result;
            char *str_result;
            JSObject obj_result;
        } value;
    } JSResult;

    typedef enum {
        HTTPServerTypeUnknown = -1,
        HTTPServerTypeSingleThreaded = 0,
        HTTPServerTypeThreadPool = 1,
        HTTPServerTypeEventLoop = 2
    } HTTPServerType;

    const char *v8_get_string_property(V8Engine *engine, JSObject obj, const char *key);

    int v8_get_number_property(V8Engine *engine, JSObject obj, const char *key, int *success);

    int v8_has_property(V8Engine *engine, JSObject obj, const char *key);

    JSObject v8_create_object(V8Engine *engine);

    int v8_set_string_property(V8Engine *engine, JSObject obj, const char *key, const char *value);

    int v8_set_number_property(V8Engine *engine, JSObject obj, const char *key, long value);

    int v8_set_object_property(V8Engine *engine, JSObject obj, const char *key, JSObject value);

    JSResult v8_call_function(V8Engine *engine, const char *function_name, JSObject arg);

    JSResult v8_call_function_no_arguments(V8Engine *engine, JSObject fun);

    void v8_free_object(JSObject obj);

    V8Engine *v8_initialize(int argc, char *argv[]);

    JSResult v8_execute_script(V8Engine *engine, const char *script);

    int v8_register_function(V8Engine *engine, const char *name, int (*func)(int));

    void v8_cleanup(V8Engine *engine);

    JSObject v8_get_object_property(V8Engine *engine, JSObject obj, const char *key);

    typedef int (*generic_func_t)(void *);

    int invoke_with_v8_locker(V8Engine *engine, generic_func_t fn, void *data);

    int v8_array_push_object(V8Engine *engine, JSObject arr, JSObject value);

    HTTPServerType v8_get_server_type(V8Engine *engine);

#ifdef __cplusplus
}

extern "C" {
#endif
    void *v8_get_registered_handler_func(V8Engine *engine);

    int v8_get_registered_port();

    JSResult v8_call_registered_handler_obj(V8Engine *engine, JSObject arg);

    JSResult v8_call_registered_handler_string(V8Engine *engine, const char *method);
#ifdef __cplusplus
}
#endif

#endif // V8_WRAPPER_H