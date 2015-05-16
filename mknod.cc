#include <nan.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace v8;

class MknodWorker : public NanAsyncWorker {
 public:
  MknodWorker(NanCallback *callback, char *path, int mode, int dev)
    : NanAsyncWorker(callback), path(path), mode(mode), dev(dev) {}
  ~MknodWorker() {}

  void Execute () {
    this->error = mknod(path, mode, dev);
    free(path);
  }

  void HandleOKCallback () {
    NanScope();
    if (this->error) {
      Local<Value> tmp[] = {NanError("mknod failed")};
      callback->Call(1, tmp);
    } else {
      callback->Call(0, NULL);
    }
  }

 private:
  char *path;
  int mode;
  int dev;
  int error;
};

NAN_METHOD(Mknod) {
  NanScope();

  if (!args[0]->IsString()) return NanThrowError("path must be a string");
  NanUtf8String path(args[0]);

  if (!args[1]->IsNumber()) return NanThrowError("mode must be a number");
  int mode = args[1]->Uint32Value();

  if (!args[2]->IsNumber()) return NanThrowError("dev must be a number");
  int dev = args[2]->Uint32Value();

  if (!args[3]->IsFunction()) return NanThrowError("callback must be a function");
  Local<Function> callback = args[3].As<Function>();

  char *path_alloc = (char *) malloc(1024);
  stpcpy(path_alloc, *path);

  NanAsyncQueueWorker(new MknodWorker(new NanCallback(callback), path_alloc, mode, dev));

}

void Init(Handle<Object> exports) {
  exports->Set(NanNew("mknod"), NanNew<FunctionTemplate>(Mknod)->GetFunction());
}

NODE_MODULE(mknod, Init)