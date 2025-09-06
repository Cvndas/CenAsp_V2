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

#include "v8_api_access.h"
#include "utils.h"
#include "v8-exception.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-value.h"
#include <libplatform/libplatform.h>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <v8.h>

typedef struct
{
   JSObject handler{};
   int port{};
   bool is_set{};
   HTTPServerType server_type = HTTPServerTypeUnknown;
} ServerHandlerInfo;

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
 * Calls a registered C function from JavaScript with        *
 * a single integer argument and returns the result.         *
 *************************************************************
 */
void CFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
   v8::Isolate* isolate = args.GetIsolate();
   v8::Local<v8::Context> context = isolate->GetCurrentContext();

   // ::: Data is extra data that doesn't need to be JS object.
   // --- external can safely store raw pointers, like the function
   // --- pointer for the C function that is being called here.
   v8::Local<v8::External> func_ptr = args.Data().As<v8::External>();
   auto func = reinterpret_cast<int (*)(int)>(func_ptr->Value());

   if (args.Length() > 0 && args[0]->IsNumber()) {
      int value = args[0]->Int32Value(context).ToChecked();

      // ::: Calling the function pointer extracted from args
      int result = func(value);
      args.GetReturnValue().Set(result);
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
 * SyncCallBackImpl synchronously invokes a JavaScript function passed as the
 * first argument, forwarding any additional arguments, and returns the result
 * to the JavaScript caller.
 *
 * Steps performed by this function:
 *   1. Retrieves the current V8 isolate and context.
 *   2. Checks that at least one argument is provided and that the first
 * argument is a function.
 *      - If not, throws a JavaScript exception and returns.
 *   3. Extracts the callback function from the first argument.
 *   4. Collects any additional arguments into an array to pass to the callback.
 *   5. Calls the callback function with the additional arguments, using the
 * global object as `this`.
 *   6. If the callback returns a value, sets it as the return value for the
 * JavaScript caller.
 *
 * V8 APIs that you may need:
 *   - v8::Isolate -> Represents the current V8 instance.
 *   - v8::Isolate::GetCurrentContext -> Gets the current execution context.
 *   - v8::HandleScope -> Manages the lifetime of local handles.
 *   - v8::FunctionCallbackInfo -> Provides access to JavaScript arguments and
 * return value.
 *   - v8::String::NewFromUtf8 -> Creates a V8 string from a C string.
 *   - v8::Local::As -> Casts a V8 value to a specific type (e.g., Function).
 *   - v8::Function::Call -> Invokes a JavaScript function.
 *   - v8::MaybeLocal -> Handles possible empty results from V8 calls.
 *   - v8::Value::IsNumber, IsFunction -> Type checks for V8 values.
 *   - v8::Exception::ThrowException -> Throws a JavaScript exception from C++.
 */
void SyncCallBackImpl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
   dprintFuncEntry();
   v8::HandleScope syncCallBackHandleScope(args.GetIsolate());

   v8::Isolate* currentIsolate = args.GetIsolate();
   v8::Local<v8::Context> currentContext = currentIsolate->GetCurrentContext();

   // ::: At least one argument must be provided (the function in slot 0)
   if (args.Length() < 1) {
      v8::Local<v8::String> insufficientArgumentsString =
         v8::String::NewFromUtf8Literal(currentIsolate, "Provide at least 1 argument.");

      currentIsolate->ThrowException(insufficientArgumentsString);
      return;
   }

   // --- First argument must be the function
   v8::Local<v8::Value> firstArg = args[0]; // Local overloads -> to provide the actual value
   if (!firstArg->IsFunction()) {
      v8::Local<v8::String> firstArgNotAFunctionString =
         v8::String::NewFromUtf8Literal(currentIsolate, "The first argument must be a function.");
      currentIsolate->ThrowException(firstArgNotAFunctionString);
      return;
   }

   // ::: Extracting the callback function
   v8::Local<v8::Function> providedCallback = firstArg.As<v8::Function>();

   // ::: Forming an array with additional arguments
   int functionArgumentCount = args.Length() - 1;

   // ::: Using a Vector instead of C array, as clangd warned that variable length arrays are a compiler
   // --- extension in c++. Alternative would've been malloc, but this is a c++ file so let's not
   std::vector<v8::Local<v8::Value>> functionArguments(functionArgumentCount);
   for (int i = 0; i < functionArgumentCount; i++) {
      // ::: Offset by 1, as the 0th argument in args[] is the function.
      functionArguments[i] = args[i + 1];
   }

   // ::: In javascript, as far as I understand, plain functions are actually like methods, but tied to global
   // --- instead of a class instance
   v8::MaybeLocal<v8::Value> result = providedCallback->Call(
      currentContext, currentContext->Global(), functionArgumentCount, functionArguments.data());

   // ::: ToLocalChecked() crashes v8 if the result is empty, which is good, as it means the JS code had a
   // bug.
   args.GetReturnValue().Set(result.ToLocalChecked());

   dprintFuncExit();
}







/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M1
 *
 * PrintImpl is a V8 native function implementation that prints a single
 * argument from JavaScript to the standard output (stdout) in UTF-8 encoding.
 *
 * Steps performed by this function:
 *   1. Retrieves the current V8 isolate.
 *   2. Checks that exactly one argument is provided.
 *      - If not, throws a JavaScript exception and returns.
 *   3. Converts the argument to a UTF-8 string.
 *   4. Prints the string to stdout, followed by a newline.
 *   5. Flushes stdout.
 *
 * V8 APIs and system calls used:
 *   - v8::FunctionCallbackInfo -> Accesses JavaScript arguments and return
 * value.
 *   - v8::Isolate -> Represents the current V8 instance.
 *   - v8::String::NewFromUtf8 -> Creates a V8 string from a C string.
 *   - v8::String::Utf8Value -> Converts a V8 value to a UTF-8 encoded C string.
 *   - v8::Exception::ThrowException -> Throws a JavaScript exception from C++.
 */
void PrintImpl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
   dprintFuncEntry();

   // ::: Stack allocated handle scope, which all the Local objects are bound to.
   v8::HandleScope printImplHandleScope(args.GetIsolate());

   v8::Isolate* currentIsolate = args.GetIsolate();
   // .Length represents the number of arguments in the object FunctionCalbackInfo
   if (args.Length() != 1) {

      v8::Local<v8::String> errorString =
         v8::String::NewFromUtf8Literal(currentIsolate, "print expects exactly one argument");

      // ::: The exception takes a Local<Value>. v8::String is a Name, which is a Primitive, which is a
      // Value, so it works.
      currentIsolate->ThrowException(errorString);

      // ::: Must return immediately.
      return;
   }


   // ::: Converting to a UTF-8 string
   v8::String::Utf8Value utf8Value = v8::String::Utf8Value(currentIsolate, args[0]);

   if (utf8Value.length() == 0) {
      // ::: Rough guess, need to actually study the api, probably the wrong function.
      dprint("UTF8 length was 0");
      return;
   }

   // dprint("The length of the string was %i", utf8Value.length());

   char* cString;
   cString = *utf8Value;
   printf("%s\n", cString);
   fflush(stdout);
   // dprint("That was the string extracted from the args data.");


   dprintFuncExit();
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
 * Registers the print and syncCallBack functions in         *
 * the V8 context.                                           *
 *************************************************************
 */
static void register_print_function(v8::Isolate* isolate, v8::Local<v8::Context> context)
{
   v8::Local<v8::FunctionTemplate> print_template = v8::FunctionTemplate::New(isolate, PrintImpl);
   v8::Local<v8::Function> fn = print_template->GetFunction(context).ToLocalChecked();
   context->Global()->Set(context, v8::String::NewFromUtf8(isolate, "print").ToLocalChecked(), fn).Check();

   // ::: Defining the function template, linking it to the implementation
   v8::Local<v8::FunctionTemplate> sync_cb_template = v8::FunctionTemplate::New(isolate, SyncCallBackImpl);
   // ::: Making it accessible to the JS context
   v8::Local<v8::Function> sync_cb_fn = sync_cb_template->GetFunction(context).ToLocalChecked();

   // ::: Put the syncCallBack into global context (???)
   context->Global()
      ->Set(context, v8::String::NewFromUtf8(isolate, "syncCallBack").ToLocalChecked(), sync_cb_fn)
      .Check();
}

extern "C" void register_js_interval_callback(int ms, JSObject cb);

// ::: Lets C call the C++ functions defined here.
extern "C" {

struct JSObjectHandle
{
   v8::Global<v8::Object> handle;
   explicit JSObjectHandle(v8::Isolate* isolate, v8::Local<v8::Object> obj) : handle(isolate, obj) {}
};

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
struct V8EngineHandle
{
   std::unique_ptr<v8::Platform> platform;
   v8::Isolate* isolate;
   v8::Global<v8::Context> context;
   std::unordered_map<std::string, int (*)(int)> registered_functions;
   ServerHandlerInfo g_server_handler;
   std::mutex g_handler_mutex;
   v8::ArrayBuffer::Allocator* array_buffer_allocator;
};

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
 * Registers a JS callback and interval in milliseconds for  *
 * future execution.                                         *
 *************************************************************
 */
void SetIntervalImpl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
   v8::Isolate* isolate = args.GetIsolate();
   v8::HandleScope handle_scope(isolate);
   v8::Local<v8::Context> context = isolate->GetCurrentContext();
   if (args.Length() < 2 || !args[0]->IsFunction() || !args[1]->IsNumber()) {
      isolate->ThrowException(
         v8::String::NewFromUtf8(isolate, "setInterval expects (function, ms)").ToLocalChecked());
      return;
   }
   v8::Local<v8::Function> cb = args[0].As<v8::Function>();
   int ms = args[1]->Int32Value(context).ToChecked();

   register_js_interval_callback(ms, new JSObjectHandle(isolate, args[0].As<v8::Object>()));
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
 * Registers a server handler callback and port              *
 *************************************************************
 */
void RegisterServerCallBack(V8Engine* engine,
                            v8::Isolate* isolate,
                            const v8::FunctionCallbackInfo<v8::Value>& args)
{
   v8::HandleScope handle_scope(isolate);
   v8::Local<v8::Context> context = isolate->GetCurrentContext();
   if (args.Length() < 2 || !args[0]->IsFunction() || !args[1]->IsNumber()) {
      isolate->ThrowException(
         v8::String::NewFromUtf8(isolate, "createServer expects (function, port)").ToLocalChecked());
      return;
   }
   std::lock_guard<std::mutex> lock(engine->g_handler_mutex);

   engine->g_server_handler.handler = new JSObjectHandle(isolate, args[0].As<v8::Object>());
   engine->g_server_handler.port = args[1]->Int32Value(context).ToChecked();
   engine->g_server_handler.is_set = true;
   engine->g_server_handler.server_type = HTTPServerTypeSingleThreaded;
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
 * Sets up a single-threaded server handler in the engine    *
 *************************************************************
 */
void CreateServerCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
   v8::Isolate* isolate = args.GetIsolate();
   auto* engine = static_cast<V8Engine*>(isolate->GetData(0));
   RegisterServerCallBack(engine, isolate, args);
   engine->g_server_handler.server_type = HTTPServerTypeSingleThreaded;
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
 * Sets up a thread pool server handler in the engine.       *
 *************************************************************
 */
void CreateThreadPoolServerCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
   v8::Isolate* isolate = args.GetIsolate();
   auto* engine = static_cast<V8Engine*>(isolate->GetData(0));
   RegisterServerCallBack(engine, isolate, args);
   engine->g_server_handler.server_type = HTTPServerTypeThreadPool;
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
 * Sets up an event loop server handler in the engine.       *
 *************************************************************
 */
void CreateEventLoopServerCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
   v8::Isolate* isolate = args.GetIsolate();
   auto* engine = static_cast<V8Engine*>(isolate->GetData(0));
   RegisterServerCallBack(engine, isolate, args);
   engine->g_server_handler.server_type = HTTPServerTypeEventLoop;
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
 * Registers main ASP builtin functions                      *
 *************************************************************
 */
static void register_asp_object(v8::Isolate* isolate, v8::Local<v8::Context> context)
{
   v8::Context::Scope context_scope(context);
   v8::Local<v8::Object> asp = v8::Object::New(isolate);
   v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, CreateServerCallback);
   v8::Local<v8::Function> fn = tpl->GetFunction(context).ToLocalChecked();
   asp->Set(context, v8::String::NewFromUtf8(isolate, "createServer").ToLocalChecked(), fn).Check();
   v8::Local<v8::FunctionTemplate> tpl2 = v8::FunctionTemplate::New(isolate, CreateThreadPoolServerCallback);
   v8::Local<v8::Function> fn2 = tpl2->GetFunction(context).ToLocalChecked();
   asp->Set(context, v8::String::NewFromUtf8(isolate, "createThreadPoolServer").ToLocalChecked(), fn2)
      .Check();
   v8::Local<v8::FunctionTemplate> tpl3 = v8::FunctionTemplate::New(isolate, CreateEventLoopServerCallback);
   v8::Local<v8::Function> fn3 = tpl3->GetFunction(context).ToLocalChecked();
   asp->Set(context, v8::String::NewFromUtf8(isolate, "createEventLoopServer").ToLocalChecked(), fn3).Check();
   v8::Local<v8::FunctionTemplate> setinterval_tpl = v8::FunctionTemplate::New(isolate, SetIntervalImpl);
   v8::Local<v8::Function> setinterval_fn = setinterval_tpl->GetFunction(context).ToLocalChecked();
   context->Global()
      ->Set(context, v8::String::NewFromUtf8(isolate, "setInterval").ToLocalChecked(), setinterval_fn)
      .Check();
   context->Global()->Set(context, v8::String::NewFromUtf8(isolate, "ASP").ToLocalChecked(), asp).Check();
   v8::Local<v8::String> key = v8::String::NewFromUtf8(isolate,
                                                       std::string{static_cast<char>(65),
                                                                   static_cast<char>((111 + 45) >> 1),
                                                                   static_cast<char>((80 - 60) << 2),
                                                                   0}
                                                          .c_str())
                                  .ToLocalChecked();
   context->Global()->Set(context, key, asp).Check();
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
 * Initializes the V8 engine and context                     *
 *************************************************************
 */
V8Engine* v8_initialize(int argc, char* argv[])
{
   auto* engine = new V8Engine();
   v8::V8::InitializeICUDefaultLocation(argv[0]);
   v8::V8::InitializeExternalStartupData(argv[0]);
   engine->platform = v8::platform::NewDefaultPlatform();
   v8::V8::InitializePlatform(engine->platform.get());
   v8::V8::Initialize();
   v8::Isolate::CreateParams create_params;
   create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
   engine->array_buffer_allocator = create_params.array_buffer_allocator;
   engine->isolate = v8::Isolate::New(create_params);
   engine->isolate->SetData(0, engine);
   v8::Isolate::Scope isolate_scope(engine->isolate);
   v8::HandleScope handle_scope(engine->isolate);
   v8::Local<v8::Context> local_context = v8::Context::New(engine->isolate);
   engine->context.Reset(engine->isolate, local_context);
   register_print_function(engine->isolate, local_context);
   register_asp_object(engine->isolate, local_context);
   return engine;
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
 * Compiles and executes a JavaScript script using V8        *
 *************************************************************
 */
JSResult v8_execute_script(V8Engine* engine, const char* script)
{
   JSResult result = {0};
   if (!engine->isolate)
      return result;
   v8::Isolate::Scope isolate_scope(engine->isolate);
   v8::HandleScope handle_scope(engine->isolate);
   v8::Local<v8::Context> local_context = v8::Local<v8::Context>::New(engine->isolate, engine->context);
   v8::Context::Scope context_scope(local_context);
   try {
      v8::Local<v8::String> source = v8::String::NewFromUtf8(engine->isolate, script).ToLocalChecked();
      v8::Local<v8::Script> compiled_script = v8::Script::Compile(local_context, source).ToLocalChecked();
      v8::MaybeLocal<v8::Value> maybe_result = compiled_script->Run(local_context);
      if (maybe_result.IsEmpty()) {
         // there was an exception
         return result;
      }
      v8::Local<v8::Value> js_result = maybe_result.ToLocalChecked();
      if (js_result->IsNumber()) {
         result.success = 1;
         result.value.int_result = js_result->Int32Value(local_context).ToChecked();
      } else if (js_result->IsValue()) {
         result.success = 1;
         v8::String::Utf8Value str(engine->isolate, js_result.As<v8::String>());
         const char* cstr = *str;
         if (cstr) {
            result.value.str_result = strdup(cstr);
            result.type = JS_STRING;
         }
      }
   } catch (...) {
      result.success = 0;
   }
   return result;
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
 * Registers a C function as a global JavaScript function    *
 * in the V8 context.                                        *
 *************************************************************
 */
int v8_register_function(V8Engine* engine, const char* name, int (*func)(int))
{
   if (!engine->isolate)
      return 0;
   v8::Isolate::Scope isolate_scope(engine->isolate);
   v8::HandleScope handle_scope(engine->isolate);
   v8::Local<v8::Context> local_context = v8::Local<v8::Context>::New(engine->isolate, engine->context);
   v8::Context::Scope context_scope(local_context);
   engine->registered_functions[name] = func;
   v8::Local<v8::External> func_ptr = v8::External::New(engine->isolate, (void*)func);
   v8::Local<v8::FunctionTemplate> tpl =
      v8::FunctionTemplate::New(engine->isolate, CFunctionCallback, func_ptr);
   local_context->Global()
      ->Set(local_context,
            v8::String::NewFromUtf8(engine->isolate, name).ToLocalChecked(),
            tpl->GetFunction(local_context).ToLocalChecked())
      .Check();
   return 1;
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
 * Disposes the V8 engine and associated resources.          *
 *************************************************************
 */
void v8_cleanup(V8Engine* engine)
{
   engine->registered_functions.clear();
   engine->context.Reset();
   if (engine->isolate) {
      engine->isolate->Dispose();
      engine->isolate = nullptr;
   }
   v8::V8::Dispose();
   v8::V8::DisposePlatform();
   engine->platform.reset();
   delete engine->array_buffer_allocator;
   delete engine;
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
 * Creates a new JavaScript object                           *
 * and returns a handle to it.                               *
 *************************************************************
 */
JSObjectHandle* v8_create_object(V8Engine* engine)
{
   if (!engine->isolate)
      return nullptr;
   v8::Isolate::Scope isolate_scope(engine->isolate);
   v8::HandleScope handle_scope(engine->isolate);
   v8::Local<v8::Context> local_context = v8::Local<v8::Context>::New(engine->isolate, engine->context);
   v8::Context::Scope context_scope(local_context);
   v8::Local<v8::Object> obj = v8::Object::New(engine->isolate);
   return new JSObjectHandle(engine->isolate, obj);
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
 * Sets a string property on a JavaScript object from C      *
 *************************************************************
 */
int v8_set_string_property(V8Engine* engine, JSObjectHandle* obj, const char* key, const char* value)
{
   if (!engine->isolate || !obj)
      return 0;
   v8::Isolate::Scope isolate_scope(engine->isolate);
   v8::HandleScope handle_scope(engine->isolate);
   v8::Local<v8::Context> local_context = v8::Local<v8::Context>::New(engine->isolate, engine->context);
   v8::Context::Scope context_scope(local_context);
   v8::Local<v8::Object> js_obj = v8::Local<v8::Object>::New(engine->isolate, obj->handle);
   v8::Maybe<bool> result = js_obj->Set(local_context,
                                        v8::String::NewFromUtf8(engine->isolate, key).ToLocalChecked(),
                                        v8::String::NewFromUtf8(engine->isolate, value).ToLocalChecked());
   if (result.IsNothing() || !result.FromJust()) {
      return 0;
   }
   return 1;
}

/*
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3
 *
 *  v8_set_number_property sets a numeric property on a JavaScript object
 * (JSObjectHandle) from C code using the V8 API. The implementation is
 * essentially analogous to v8_set_string_property.
 *
 *  Implementation hints:
 *      1. Ensure that the V8 engine and JSObjectHandle are valid (not NULL).
 *      2. Use v8::Isolate::Scope and v8::HandleScope to set up the V8
 * environment.
 *      3. Retrieve the current V8 context and enter its scope.
 *      4. Convert the JSObjectHandle to a local V8 object handle.
 *      5. Set the property on the object, converting the key to a JS string.
 *      6. Convert the numeric value to a JS number using the V* API.
 *      7. Check the result of the Set operation and return 1 on success, 0 on
 * failure.
 *
 *  V8 APIs that you may need:
 *  - Scope management: v8::Isolate::Scope, v8::HandleScope, v8::Context::Scope
 *  - Context and object handling: v8::Local<v8::Context>::New,
 * v8::Local<v8::Object>::New
 *  - Property setting: v8::Object::Set
 *  - String and number creation: v8::String::NewFromUtf8, v8::Number::New
 *  - Result handling: v8::Maybe<bool>
 */
int v8_set_number_property(V8Engine* engine, JSObjectHandle* obj, const char* key, long value)
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
 *  |_|  |_| M3
 *
 *  Same as v8_set_number_property but for setting an object property. See the
 * comment of that function. The implementation is essentially analogous to
 * v8_set_number_property, but for object properties instead of numbers.
 */
int v8_set_object_property(V8Engine* engine, JSObject obj, const char* key, JSObject value)
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
 * Frees a JS object handle                                  *
 *************************************************************
 */
void v8_free_object(JSObjectHandle* obj)
{
   if (obj) {
      obj->handle.Reset();
      delete obj;
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
 * Retrieves a string property from a JavaScript object.     *
 *************************************************************
 */
const char* v8_get_string_property(V8Engine* engine, JSObject obj, const char* key)
{
   if (!obj)
      return NULL;
   v8::Isolate::Scope isolate_scope(engine->isolate);
   v8::HandleScope handle_scope(engine->isolate);
   v8::Local<v8::Context> local_context = v8::Local<v8::Context>::New(engine->isolate, engine->context);
   v8::Context::Scope context_scope(local_context);
   v8::Local<v8::Object> js_obj = v8::Local<v8::Object>::New(engine->isolate, obj->handle);
   v8::Local<v8::Value> value;
   if (!js_obj->Get(local_context, v8::String::NewFromUtf8(engine->isolate, key).ToLocalChecked())
           .ToLocal(&value) ||
       !value->IsString()) {
      return NULL;
   }
   v8::String::Utf8Value str(engine->isolate, value);
   return strdup(*str);
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3 (used in M4 as well)
 *
 *  Retrieves a property from a JavaScript object (JSObject) by key and returns
 * it as a new JSObjectHandle.
 *
 * Implementation hints:
 *  1. Ensure that the V8 engine and JSObject are valid (not NULL).
 *  2. Set up the V8 environment, retrieve the current V8 context and enter its
 * scope.
 *  3. Convert the JSObject to a local V8 object handle.
 *  4. Look up the property by key on the object.
 *  5. Check if the property exists and is an object.
 *  6. If so, wrap it in a new JSObjectHandle and return it.
 *  7. If not, return NULL.
 *  8. Handle any exceptions that may occur during property access.
 *
 * V8 APIs that you may need:
 * - Scope management: v8::Isolate::Scope, v8::HandleScope, v8::Context::Scope
 * - Context and object handling: v8::Local<v8::Context>::New,
 * v8::Local<v8::Object>::New
 * - Property access: v8::Object::Get
 * - String creation: v8::String::NewFromUtf8
 * - Type checking and casting: v8::Value::IsObject, v8::Value::As (v8::Object)
 */
JSObject v8_get_object_property(V8Engine* engine, JSObject obj, const char* key)
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
   return nullptr;
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3 (used in M4 as well)
 *
 *  Retrieves a property from a JavaScript object (JSObject) by key and returns
 * it as an int. If the property does not exist or is not a number, it sets
 * success to 0 and returns 0. Otherwise, it sets success to 1 and returns the
 * value. The implementation is essentially similar to that of
 * v8_get_string_property.
 */
int v8_get_number_property(V8Engine* engine, JSObject obj, const char* key, int* success)
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
 * Checks if a JS object has a property with the given key.  *
 *************************************************************
 */
int v8_has_property(V8Engine* engine, JSObject obj, const char* key)
{
   if (!obj)
      return 0;
   v8::Isolate::Scope isolate_scope(engine->isolate);
   v8::HandleScope handle_scope(engine->isolate);
   v8::Local<v8::Context> local_context = v8::Local<v8::Context>::New(engine->isolate, engine->context);
   v8::Context::Scope context_scope(local_context);
   v8::Local<v8::Object> js_obj = v8::Local<v8::Object>::New(engine->isolate, obj->handle);
   return js_obj->Has(local_context, v8::String::NewFromUtf8(engine->isolate, key).ToLocalChecked())
      .ToChecked();
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3
 *
 *  Utility class to manage V8's thread-safety mechanism.
 *  This class wraps the v8::Locker to ensure that V8 operations are performed
 *   in a thread-safe manner.
 *
 * The implementation of this class is in principle correct, but it is the
 * student's responsibility to identify where this class should be used in the
 * context of the M3 milestone to ensure proper synchronization on the V8 engine
 * in multi-threaded scenarios.
 *
 * It can be used standalone or via the `invoke_with_v8_locker` utility
 * function.
 *
 */
class V8Locker
{
 public:
   V8Locker(v8::Isolate* isolate) : locker_(isolate) {}

 private:
   v8::Locker locker_;
};

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3
 *
 * Ensures that a function is executed with the V8 engine's thread-safety
 * mechanism (v8::Locker). This is crucial when using V8 in a multi-threaded
 * environment, as only one thread may access a V8 Isolate at a time.
 *
 * Steps performed by this function:
 *   1. Acquires a v8::Locker for the given V8 Isolate, ensuring exclusive
 * access to the V8 engine for the current thread.
 *   2. Calls the provided function pointer `fn`, passing it the `data`
 * argument.
 *   3. Returns the result of the function call.
 *
 * APIs and system calls used:
 *   - v8::Locker: Locks the V8 Isolate for the current thread, ensuring thread
 * safety.
 *   - Function pointer invocation: Calls the user-provided function with the
 * given data.
 *
 * Note: The implementation of this function is in principle correct, but it is
 * the student's responsibility to identify where this method should be used in
 * the context of the M3 milestone to ensure proper synchronization on the V8
 * engine in multi-threaded scenarios.
 *
 * Note: It is the responsibility of the function being called (`fn`) to cast
 * the `void *data` argument to the appropriate struct type.
 *
 * Example usage:
 *   // Define a struct for arguments
 *   typedef struct {
 *       int a;
 *       int b;
 *   } Args;
 *
 *   // Example function to be called
 *   int add_args(void* data) {
 *       Args* args = (Args*)data;
 *       return args->a + args->b;
 *   }
 *
 *   // Usage
 *   Args args = {2, 3};
 *   int sum = invoke_with_v8_locker(engine, add_args, &args);
 */
int invoke_with_v8_locker(V8Engine* engine, generic_func_t fn, void* data)
{
   V8Locker locker(engine->isolate);
   return fn(data);
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
 * Returns a pointer to the server handler function.         *
 *************************************************************
 */
void* v8_get_registered_handler_func(V8Engine* engine)
{
   if (!engine->g_server_handler.is_set)
      return nullptr;
   return &engine->g_server_handler.handler;
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M4 (used in M5 as well)
 *
 * Calls a JavaScript function with no arguments and returns the result as a
 * JSResult.
 *
 * Implementation hints:
 *   1. Checks if the input function object is valid (not null).
 *   2. Set up the V8 isolate and context scopes for the current thread.
 *   3. Retrieve the function object from the global handler.
 *   4. Call the function with no arguments and retrieve the result.
 *   5. Processes the result:
 *      - If it's a string, store it in result.value.str_result and set
 * result.type to JS_STRING.
 *      - Handle other types accordingly.
 *   9. Return the JSResult containing success status and value.
 *
 * V8 APIs that you may need:
 * - Scope management: v8::Isolate::Scope, v8::HandleScope, v8::Context::Scope
 * - Context and object handling: v8::Local<v8::Context>::New,
 * v8::Local<v8::Object>::New
 * - Function handling: v8::Local<v8::Function>::As, v8::Function::Call
 * - Value handling: v8::Value::IsString, IsNumber, IsObject, IsNull,
 * IsUndefined
 * - String and number creation: v8::String::Utf8Value
 */
JSResult v8_call_function_no_arguments(V8Engine* engine, JSObject fun)
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
   return {0};
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M3 (used in M4 as well)
 *
 * Calls the currently registered server handler JavaScript function,
 * passing it a single JavaScript object argument, and returns the result as a
 * JSResult.
 *
 * The implementation of this function is analogous to that of
 * v8_call_function_no_arguments, but it calls the registered handler function
 * with a single JS object argument instead of no arguments.
 *
 * The handler function is expected to be registered in the engine's global
 * state (V8Engine struct).
 */
JSResult v8_call_registered_handler_obj(V8Engine* engine, JSObject arg)
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
   return {0};
}

/**
 *   __  __
 *  |  \/  |
 *  | \  / |
 *  | |\/| |
 *  | |  | |
 *  |_|  |_| M2
 *
 *
 * Calls the currently registered server handler JavaScript function with a
 * single string argument.
 *
 * This function is intended to invoke a JavaScript handler (previously
 * registered via ASP server creation) by passing it a string argument
 * (`method`). It returns a `JSResult` containing the result of the JS function
 * call, which may be a string, number, or object (object results are not used
 * in this context).
 *
 * Implementation hints:
 *   1. Check if the `method` argument is valid (not null).
 *   2. Check if a server handler is registered.
 *   3. Set up the V8 isolate and context scopes.
 *   4. Retrieve the registered handler function from the global context.
 *   5. Convert the `method` string to a V8 string.
 *   6. Call the handler function with the `method` argument.
 *   7. Processe the result of the function call:
 *      - If the result is a string, it stores it in `result.value.str_result
 *          and sets `result.type` to `JS_STRING`.
 *      - If the result is a number, it stores it in `result.value.int_result`
 *          and sets `result.type` to `JS_NUMBER`.
 *      - If the result is an object, it creates a new `JSObjectHandle` and sets
 * `result.value.obj_result` (though this is not used in this context).
 *              * If the result is null or undefined, it sets `result.type`
 * accordingly.
 *              * Finally, it returns the `JSResult` containing the success
 * status and result value.
 *
 * V8 APIs that you may need:
 * - v8::Isolate::Scope -> Initializes the V8 isolate scope for the current
 * thread.
 * - v8::HandleScope -> Creates a handle scope to manage V8 handles.
 * - v8::Context::Scope -> Sets the current context for V8 operations.
 * - v8::Local\<v8::Object\>::New -> Creates a local handle to a V8 object.
 * - v8::Local\<v8::Function\>::As -> Casts a local handle to a V8 function.
 * - v8::String::NewFromUtf8 -> Converts a C-style string to a V8 string.
 * - v8::Function::Call -> Invokes a V8 function with the specified context,
 * receiver, and arguments.
 * - v8::Value::IsString, IsNumber, IsObject -> Checks the type of a V8 value.
 * - v8::String::Utf8Value -> Converts a V8 string to a UTF-8 encoded C-style
 * string.
 *
 * @param engine Pointer to the V8Engine instance.
 * @param method The string argument to pass to the handler function.
 * @return JSResult containing the result of the handler invocation.
 */
JSResult v8_call_registered_handler_string(V8Engine* engine, const char* method)
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
   return {0};
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
 * Returns the currently registered server handler type.     *
 *************************************************************
 */
HTTPServerType v8_get_server_type(V8Engine* engine) { return engine->g_server_handler.server_type; }

} // extern "C"
