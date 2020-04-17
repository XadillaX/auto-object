#include <nan.h>
#include <string>

namespace __auto_object__ {

using v8::Local;
using v8::MaybeLocal;
using v8::Persistent;
using v8::Array;
using v8::Function;
using v8::FunctionTemplate;
using v8::Name;
using v8::NamedPropertyHandlerConfiguration;
using v8::Object;
using v8::ObjectTemplate;
using v8::PropertyCallbackInfo;
using v8::PropertyHandlerFlags;
using v8::String;
using v8::Value;

#define INTERNAL_PROPERTIES_COUNT (14)
std::string _internal_properties[INTERNAL_PROPERTIES_COUNT] = {
    "constructor",
    "hasOwnProperty",
    "isPrototypeOf",
    "propertyIsEnumerable",
    "toLocaleString",
    "toString",
    "valueOf",
    "__defineGetter__",
    "__defineSetter__",
    "__lookupGetter__",
    "__lookupSetter__",
    "__proto__",
    "inspect",
    "$$constructor"
};

Nan::Persistent<Function> emit_warning;

void PropertyGetter(
        Local<Name> property,
        const PropertyCallbackInfo<v8::Value>& info)
{
    if(!property->IsString())
    {
        return;
    }

    // ignore reserve property `$$access`
    Nan::Utf8String key(property);
    if(!*key || strcmp(*key, "$$access") == 0)
    {
        return;
    }

    // ignore internal properties
    for(auto i = 0; i < INTERNAL_PROPERTIES_COUNT; i++)
    {
        if(*key == _internal_properties[i])
        {
            return;
        }
    }

    Local<Value> val = Nan::Get(
            info.This(),
            Nan::New<String>("$$access").ToLocalChecked()).ToLocalChecked();

    if(val.IsEmpty() || !val->IsFunction())
    {
        Local<Function> emit = Nan::New(emit_warning);
        Local<Value> argv[] = {
            Nan::New("No $$access implemented in the object.").ToLocalChecked()
        };
        Nan::Call(emit, info.Holder(), 1, argv);
        return;
    }

    Local<Function> func = val.As<Function>();
    Local<Value> argv[] = { property };

    Nan::TryCatch trycatch;
    MaybeLocal<Value> ret = Nan::Call(func, info.Holder(), 1, argv);
    if(trycatch.HasCaught()) {
        trycatch.ReThrow();
        return;
    }

    info.GetReturnValue().Set(ret.ToLocalChecked());
    return;
}


NAN_METHOD(__Constructor)
{
    if(!info.IsConstructCall())
    {
        Nan::ThrowError("Please use `new` to create object.");
        return;
    }

    Nan::TryCatch trycatch;
    Local<Value> val = Nan::Get(
            info.This(),
            Nan::New<String>("$$constructor").ToLocalChecked()).ToLocalChecked();

    if(trycatch.HasCaught())
    {
        trycatch.ReThrow();
        return;
    }

    if(val.IsEmpty() || val->IsUndefined() || val->IsNull())
    {
        info.GetReturnValue().Set(info.This());
        return;
    }
    else
    if(!val->IsFunction())
    {
        Nan::ThrowError("Broken object missing `$$constructor`.");
        trycatch.ReThrow();
        return;
    }

    Local<Function> func = val.As<Function>();
    Local<Value> argv[info.Length()];
    for(auto i = 0; i < info.Length(); i++)
    {
        argv[i] = info[i];
    }

    Nan::Call(func, info.This(), info.Length(), argv);
    if(trycatch.HasCaught()) {
        trycatch.ReThrow();
        return;
    }

    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(CreateObject)
{
    auto ctx = Nan::GetCurrentContext();
    Local<ObjectTemplate> tpl = Nan::New<ObjectTemplate>();
    tpl->SetHandler(NamedPropertyHandlerConfiguration(
                PropertyGetter,
                0,
                0,
                0,
                0,
                Local<Value>(),
                PropertyHandlerFlags::kNonMasking));
    info.GetReturnValue().Set(tpl->NewInstance(ctx).ToLocalChecked());
}

NAN_METHOD(CreateClass)
{
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(__Constructor);
    Local<ObjectTemplate> instance_t = tpl->InstanceTemplate();

    // Drop support for lower version of Node.js
    //
    // Do not use `Nan::SetNamedPropertyHandler` here because we need
    // `PropertyHandlerFlags::kNonMasking`
    instance_t->SetHandler(NamedPropertyHandlerConfiguration(
            PropertyGetter,
            0,
            0,
            0,
            0,
            Local<Value>(),
            PropertyHandlerFlags::kNonMasking));

    auto ctx = Nan::GetCurrentContext();
    Nan::TryCatch trycatch;
    Local<String> class_name = Nan::New("AutoClass").ToLocalChecked();
    if(info.Length() > 0)
    {
        MaybeLocal<String> maybe = info[0]->ToString(ctx);
        if(!maybe.IsEmpty())
        {
            class_name = maybe.ToLocalChecked();
        }
        else
        if(trycatch.HasCaught())
        {
            trycatch.ReThrow();
            return;
        }
    }

    tpl->SetClassName(class_name);
    info.GetReturnValue().Set(tpl->GetFunction(ctx).ToLocalChecked());
}

NAN_METHOD(InitEnv)
{
    auto ctx = Nan::GetCurrentContext();
    Local<Object> param = info[0]->ToObject(ctx).ToLocalChecked();
    Local<Function> func = Nan::Get(
            param,
            Nan::New("emitWarning").ToLocalChecked()).ToLocalChecked().As<Function>();

    emit_warning.Reset(func);
}

NAN_MODULE_INIT(Init)
{
    Nan::HandleScope scope;
    Nan::Export(target, "createClass", CreateClass);
    Nan::Export(target, "createObject", CreateObject);
    Nan::Export(target, "init", InitEnv);

    Local<Array> array = Nan::New<Array>(INTERNAL_PROPERTIES_COUNT);
    for(auto i = 0; i < INTERNAL_PROPERTIES_COUNT; i++)
    {
        Nan::Set(array, i, Nan::New<String>(
                _internal_properties[i].c_str()).ToLocalChecked());
    }
    Nan::Set(target, Nan::New("internalProperties").ToLocalChecked(), array);
}

NODE_MODULE(auto, Init)

}
