#include "node_conversion.hpp"
#include "node_expression.hpp"
#include "node_feature.hpp"

#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/util/geojson.hpp>
#include <nan.h>

using namespace mbgl::style;
using namespace mbgl::style::expression;

namespace node_mbgl {

Nan::Persistent<v8::Function> NodeExpression::constructor;

void NodeExpression::Init(v8::Local<v8::Object> target) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Expression").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1); // what is this doing?

    Nan::SetPrototypeMethod(tpl, "evaluate", Evaluate);
    Nan::SetPrototypeMethod(tpl, "getType", GetType);
    Nan::SetPrototypeMethod(tpl, "isFeatureConstant", IsFeatureConstant);
    Nan::SetPrototypeMethod(tpl, "isZoomConstant", IsZoomConstant);

    Nan::SetPrototypeMethod(tpl, "serialize", Serialize);

    Nan::SetMethod(tpl, "parse", Parse);

    constructor.Reset(tpl->GetFunction()); // what is this doing?
    Nan::Set(target, Nan::New("Expression").ToLocalChecked(), tpl->GetFunction());
}

type::Type parseType(v8::Local<v8::Object> type) {
    static std::unordered_map<std::string, type::Type> types = {
        {"string", type::String},
        {"number", type::Number},
        {"noolean", type::Boolean},
        {"object", type::Object},
        {"color", type::Color},
        {"value", type::Value}
    };

    v8::Local<v8::Value> v8kind = Nan::Get(type, Nan::New("kind").ToLocalChecked()).ToLocalChecked();
    std::string kind(*v8::String::Utf8Value(v8kind));

    if (kind == "array") {
        type::Type itemType = parseType(Nan::Get(type, Nan::New("itemType").ToLocalChecked()).ToLocalChecked()->ToObject());
        mbgl::optional<std::size_t> N;

        v8::Local<v8::String> Nkey = Nan::New("N").ToLocalChecked();
        if (Nan::Has(type, Nkey).FromMaybe(false)) {
            N = Nan::Get(type, Nkey).ToLocalChecked()->ToInt32()->Value();
        }
        return type::Array(itemType, N);
    }

    return types[kind];
}

void NodeExpression::Parse(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Function> cons = Nan::New(constructor);

    if (info.Length() < 1 || info[0]->IsUndefined()) {
        return Nan::ThrowTypeError("Requires a JSON style expression argument.");
    }

    mbgl::optional<type::Type> expected;
    if (info.Length() > 1 && info[1]->IsObject()) {
        expected = parseType(info[1]->ToObject());
    }

    auto expr = info[0];

    try {
        ParsingContext ctx(expected);
        ParseResult parsed = ctx.parse(mbgl::style::conversion::Convertible(expr));
        if (parsed) {
            assert(ctx.getErrors().size() == 0);
            auto nodeExpr = new NodeExpression(std::move(*parsed));
            const int argc = 0;
            v8::Local<v8::Value> argv[0] = {};
            auto wrapped = Nan::NewInstance(cons, argc, argv).ToLocalChecked();
            nodeExpr->Wrap(wrapped);
            info.GetReturnValue().Set(wrapped);
            return;
        }

        v8::Local<v8::Array> result = Nan::New<v8::Array>();
        for (std::size_t i = 0; i < ctx.getErrors().size(); i++) {
            const auto& error = ctx.getErrors()[i];
            v8::Local<v8::Object> err = Nan::New<v8::Object>();
            Nan::Set(err,
                    Nan::New("key").ToLocalChecked(),
                    Nan::New(error.key.c_str()).ToLocalChecked());
            Nan::Set(err,
                    Nan::New("error").ToLocalChecked(),
                    Nan::New(error.message.c_str()).ToLocalChecked());
            Nan::Set(result, Nan::New((uint32_t)i), err);
        }
        info.GetReturnValue().Set(result);
    } catch(std::exception &ex) {
        return Nan::ThrowError(ex.what());
    }
}

void NodeExpression::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create new Expression objects");
    }

    info.GetReturnValue().Set(info.This());
}

struct ToValue {
    v8::Local<v8::Value> operator()(mbgl::NullValue) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::Null());
    }

    v8::Local<v8::Value> operator()(bool t) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New(t));
    }

    v8::Local<v8::Value> operator()(double t) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New(t));
    }

    v8::Local<v8::Value> operator()(const std::string& t) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New(t).ToLocalChecked());
    }

    v8::Local<v8::Value> operator()(const std::vector<Value>& array) {
        Nan::EscapableHandleScope scope;
        v8::Local<v8::Array> result = Nan::New<v8::Array>();
        for (unsigned int i = 0; i < array.size(); i++) {
            result->Set(i, toJS(array[i]));
        }
        return scope.Escape(result);
    }

    v8::Local<v8::Value> operator()(const mbgl::Color& color) {
        return operator()(std::vector<Value> {
            static_cast<double>(color.r),
            static_cast<double>(color.g),
            static_cast<double>(color.b),
            static_cast<double>(color.a)
        });
    }

    v8::Local<v8::Value> operator()(const std::unordered_map<std::string, Value>& map) {
        Nan::EscapableHandleScope scope;
        v8::Local<v8::Object> result = Nan::New<v8::Object>();
        for (const auto& entry : map) {
            Nan::Set(result, Nan::New(entry.first).ToLocalChecked(), toJS(entry.second));
        }

        return scope.Escape(result);
    }
};

v8::Local<v8::Value> toJS(const Value& value) {
    return Value::visit(value, ToValue());
}

void NodeExpression::Evaluate(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());
    const std::unique_ptr<Expression>& expression = nodeExpr->expression;

    if (info.Length() < 2 || !info[0]->IsObject()) {
        return Nan::ThrowTypeError("Requires globals and feature arguments.");
    }

    mbgl::optional<float> zoom;
    v8::Local<v8::Value> v8zoom = Nan::Get(info[0]->ToObject(), Nan::New("zoom").ToLocalChecked()).ToLocalChecked();
    if (v8zoom->IsNumber()) zoom = v8zoom->NumberValue();

    mbgl::optional<double> heatmapDensity;
    v8::Local<v8::Value> v8heatmapDensity = Nan::Get(info[0]->ToObject(), Nan::New("heatmapDensity").ToLocalChecked()).ToLocalChecked();
    if (v8heatmapDensity->IsNumber()) heatmapDensity = v8heatmapDensity->NumberValue();

    Nan::JSON NanJSON;
    conversion::Error conversionError;
    mbgl::optional<mbgl::GeoJSON> geoJSON = conversion::convert<mbgl::GeoJSON>(info[1], conversionError);
    if (!geoJSON) {
        Nan::ThrowTypeError(conversionError.message.c_str());
        return;
    }

    try {
        mapbox::geojson::feature feature = geoJSON->get<mapbox::geojson::feature>();
        auto result = expression->evaluate(zoom, feature, heatmapDensity);
        if (result) {
            info.GetReturnValue().Set(toJS(*result));
        } else {
            v8::Local<v8::Object> res = Nan::New<v8::Object>();
            Nan::Set(res,
                    Nan::New("error").ToLocalChecked(),
                    Nan::New(result.error().message.c_str()).ToLocalChecked());
            info.GetReturnValue().Set(res);
        }
    } catch(std::exception &ex) {
        return Nan::ThrowTypeError(ex.what());
    }
}

void NodeExpression::GetType(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());
    const std::unique_ptr<Expression>& expression = nodeExpr->expression;

    const type::Type type = expression->getType();
    const std::string name = type.match([&] (const auto& t) { return t.getName(); });
    info.GetReturnValue().Set(Nan::New(name.c_str()).ToLocalChecked());
}

void NodeExpression::IsFeatureConstant(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());
    const std::unique_ptr<Expression>& expression = nodeExpr->expression;
    info.GetReturnValue().Set(Nan::New(isFeatureConstant(*expression)));
}

void NodeExpression::IsZoomConstant(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());
    const std::unique_ptr<Expression>& expression = nodeExpr->expression;
    info.GetReturnValue().Set(Nan::New(isZoomConstant(*expression)));
}

void NodeExpression::Serialize(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    NodeExpression* nodeExpr = ObjectWrap::Unwrap<NodeExpression>(info.Holder());
    const std::unique_ptr<Expression>& expression = nodeExpr->expression;

    const mbgl::Value serialized = expression->serialize();
    info.GetReturnValue().Set(toJS(serialized));
}

} // namespace node_mbgl
