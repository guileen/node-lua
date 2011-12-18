extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <node.h>
#include <v8.h>
#include <string>
using namespace v8;
using namespace std;

void DoFile (eio_req *);
int DoFileAfter (eio_req *);
inline int DOFILE (string&);

struct request {
    Persistent<Function> callback;
    string in;
    int out;
};

Handle<Value> DoFileAsync(const Arguments &args) {
    HandleScope scope;

    const char* usage = "usage: dofile(filename, callback)";
    if(args.Length() != 2) {
        return ThrowException(Exception::Error(String::New(usage)));
    }

    String::Utf8Value in(args[0]);
    Local<Function> callback = Local<Function>::Cast(args[1]);

    request *sr = new request;
    sr->callback = Persistent<Function>::New(callback);
    sr->in = *in;

    eio_custom(DoFile, EIO_PRI_DEFAULT, DoFileAfter, sr);
    ev_ref(EV_DEFAULT_UC);

    return scope.Close( Undefined() );
}

void DoFile(eio_req *req) {
    request *sr = static_cast<request *>(req->data);

    sr->out = DOFILE(sr->in);
}

int DoFileAfter(eio_req *req) {
    HandleScope scope;

    ev_unref(EV_DEFAULT_UC);
    request *sr = static_cast<request *>(req->data);
    Local<Value> argv[1];

    // set callback result
    argv[0] = Integer::New(static_cast<int32_t>(sr->out));

    TryCatch try_catch;
    sr->callback->Call(Context::GetCurrent()->Global(), 1, argv);
    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    /* cleanup */
    sr->callback.Dispose();
    free(sr);
    return 0;
}

Handle<Value> DoFileSync(const Arguments &args) {
    HandleScope scope;

    const char *usage = "usage: dofileSync(filename)";

    if (args.Length() < 1 || !args[0]->IsString()) {
        return ThrowException(Exception::TypeError(String::New(usage)));
    }

    String::Utf8Value utf8_in(args[0]);
    string in = *utf8_in;

    return scope.Close(Integer::New(static_cast<int32_t>(DOFILE(in))));
}

inline int DOFILE(string& filename) {

    /* initialize Lua */
    lua_State* L = lua_open();
    /* load Lua base libraries */
    luaL_openlibs(L);
    /* run the script */
    int ret = luaL_dofile(L, filename.c_str());
    /* cleanup Lua */
    lua_close(L);
    return ret; 
}

void init (Handle<Object> target) {
    NODE_SET_METHOD(target, "dofile", DoFileAsync);
    NODE_SET_METHOD(target, "dofileSync", DoFileSync);
/*
vm(rootpath);
doFile();
doFileSync();
DoString();
DoStringSync();
setGlobal(String key, Value value, bool convert)
*/
}

NODE_MODULE(node_lua, init);

