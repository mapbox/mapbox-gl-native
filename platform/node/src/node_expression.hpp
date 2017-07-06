#pragma once

#include <exception>
#include <memory>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/expression.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

using namespace mbgl::style::expression;

namespace node_mbgl {

v8::Local<v8::Value> toJS(const Value&);

class NodeExpression : public Nan::ObjectWrap {
public:
    static void Init(v8::Local<v8::Object>);

private:
    NodeExpression(std::unique_ptr<Expression> expression_) :
        expression(std::move(expression_))
    {};

    static void New(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Parse(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void Evaluate(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void GetType(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void IsFeatureConstant(const Nan::FunctionCallbackInfo<v8::Value>&);
    static void IsZoomConstant(const Nan::FunctionCallbackInfo<v8::Value>&);
    static Nan::Persistent<v8::Function> constructor;

    std::unique_ptr<Expression> expression;
};

} // namespace node_mbgl
