#include <nan.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace v8;

class MknodWorker : public Nan::AsyncWorker {
 public:
  MknodWorker(Nan::Callback *callback, char *path, int mode, int dev)
    : Nan::AsyncWorker(callback), path(path), mode(mode), dev(dev) {}
  ~MknodWorker() {}

  void Execute () {
    this->error = mknod(path, mode, dev);
    free(path);
  }

  void HandleOKCallback () {
    Nan::HandleScope scope;
    if (this->error) {
      Local<Value> tmp[] = {
        Nan::Error("mknod failed")
      };
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
  Nan::HandleScope scope;

  if (!info[0]->IsString()) return Nan::ThrowError("path must be a string");
  Nan::Utf8String path(info[0]);

  if (!info[1]->IsNumber()) return Nan::ThrowError("mode must be a number");
  int mode = info[1]->Uint32Value();

  if (!info[2]->IsNumber()) return Nan::ThrowError("dev must be a number");
  int dev = info[2]->Uint32Value();

  if (!info[3]->IsFunction()) return Nan::ThrowError("callback must be a function");
  Local<Function> callback = info[3].As<Function>();

  char *path_alloc = (char *) malloc(1024);
  stpcpy(path_alloc, *path);

  Nan::AsyncQueueWorker(new MknodWorker(new Nan::Callback(callback), path_alloc, mode, dev));
}

NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New<String>("mknod").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(Mknod)).ToLocalChecked());
}

NODE_MODULE(mknod, InitAll)

