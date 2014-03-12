#ifndef LLMR_STYLE_STYLE_PARSER
#define LLMR_STYLE_STYLE_PARSER

#include <rapidjson/document.h>
#include <llmr/style/style.hpp>

namespace llmr {

class StyleParser {
public:
    using JSVal = const rapidjson::Value&;

    void parseBuckets(JSVal value, std::map<std::string, BucketDescription>& buckets);
    void parseLayers(JSVal value, std::vector<LayerDescription>& layers);
    void parseConstants(JSVal value);
    void parseClasses(JSVal value, std::map<std::string, ClassDescription>& classes);

private:
    LayerDescription parseLayer(JSVal value);
    BucketDescription parseBucket(JSVal value);
    std::pair<std::string, ClassDescription> parseClassDescription(JSVal value);
    void parseClass(const std::string& name, JSVal value, ClassDescription& class_desc);
    FillClass parseFillClass(JSVal value);
    LineClass parseLineClass(JSVal value);
    PointClass parsePointClass(JSVal value);
    BackgroundClass parseBackgroundClass(JSVal value);

    bool parseBoolean(JSVal value);
    std::string parseString(JSVal value);
    Color parseColor(JSVal value);
    Value parseValue(JSVal value);
    template <typename T> typename FunctionProperty<T>::fn parseFunctionType(JSVal type);
    FunctionProperty<float> parseFloatFunction(JSVal value);
    FunctionProperty<bool> parseBoolFunction(JSVal value);

private:
    std::map<std::string, const rapidjson::Value *> constants;
};

}

#endif
