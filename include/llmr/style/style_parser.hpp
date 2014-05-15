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
    void parseClasses(JSVal value, std::map<std::string, ClassDescription>& classes, std::map<std::string, BucketDescription>& buckets, std::vector<LayerDescription>& layers);

private:
    LayerDescription parseLayer(JSVal value);
    BucketDescription parseBucket(JSVal value);
    std::pair<std::string, ClassDescription> parseClassDescription(JSVal value, std::map<std::string, BucketDescription>& buckets, std::vector<LayerDescription>& layers);
    void parseClass(const std::string& name, JSVal value, ClassDescription& class_desc, std::map<std::string, BucketDescription>& buckets, std::vector<LayerDescription>& layers);
    FillClass parseFillClass(JSVal value);
    LineClass parseLineClass(JSVal value);
    IconClass parseIconClass(JSVal value);
    TextClass parseTextClass(JSVal value);
    BackgroundClass parseBackgroundClass(JSVal value);
    RasterClass parseRasterClass(JSVal value);

    bool parseBoolean(JSVal value);
    std::string parseString(JSVal value);
    const JSVal& replaceConstant(const JSVal& value);
    std::vector<FunctionProperty> parseArray(JSVal value, uint16_t expected_count);
    Color parseColor(JSVal value);
    Value parseValue(JSVal value);
    FunctionProperty::fn parseFunctionType(JSVal type);
    FunctionProperty parseFunction(JSVal value);

private:
    std::map<std::string, const rapidjson::Value *> constants;
};

}

#endif
