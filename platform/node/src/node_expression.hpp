#pragma once

#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <exception>
#include <memory>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

namespace node_mbgl {

v8::Local<v8::Value> toJS(const mbgl::style::expression::Value&);

class NodeExpression : public Nan::ObjectWrap {
public:
    static void Init(v8::Local<v8::Object>);

private:
    NodeExpression(std::unique_ptr<mbgl::style::expression::Expression> expression_)
        : expression(std::move(expression_)){};

    static void New(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Parse(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Evaluate(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void GetType(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void IsFeatureConstant(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void IsZoomConstant(const Nan::FunctionCallbackInfo<v8::Value>&);

    static void Serialize(const Nan::FunctionCallbackInfo<v8::Value>&);

    static Nan::Persistent<v8::Function> constructor;

    std::unique_ptr<mbgl::style::expression::Expression> expression;
};

} // namespace node_mbgl
