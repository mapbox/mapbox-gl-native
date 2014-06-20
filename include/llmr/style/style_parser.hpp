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
    void parseClasses(JSVal value, std::map<std::string, ClassDescription> &classes,
                      const std::map<std::string, BucketDescription> &buckets,
                      const std::vector<LayerDescription> &layers);

private:
    LayerDescription parseLayer(JSVal value);
    BucketDescription parseBucket(JSVal value);

    std::pair<std::string, ClassDescription>
    parseClassDescription(JSVal value,
                          const std::map<std::string, BucketDescription> &buckets,
                          const std::map<std::string, std::string> &layerBuckets);

    void parseClass(const std::string &name, JSVal value, ClassDescription &class_desc,
                    const std::map<std::string, BucketDescription> &buckets,
                    const std::map<std::string, std::string> &layerBuckets);

    void parseGenericClass(ClassProperties &klass, JSVal value);
    ClassProperties parseFillClass(JSVal value);
    ClassProperties parseLineClass(JSVal value);
    ClassProperties parseIconClass(JSVal value);
    ClassProperties parseTextClass(JSVal value);
    ClassProperties parseBackgroundClass(JSVal value);
    ClassProperties parseRasterClass(JSVal value);
    ClassProperties parseCompositeClass(JSVal value);

    PropertyFilterExpression parseFilterOrExpression(JSVal value);

    JSVal replaceConstant(JSVal value);
    Value parseValue(JSVal value);

    template <typename T>
    T parse(JSVal value);

    template <typename T>
    inline void parse(const char *property_name, ClassPropertyKey key, ClassProperties &klass, JSVal value) {
        if (value.HasMember(property_name)) {
            klass.emplace(key, parse<T>(value[property_name]));
        }
    }

    void parseTransition(const char *property_name, ClassPropertyKey key, ClassProperties &klass, JSVal value);
    void parseFunctionArray(const char *property_name, const std::vector<ClassPropertyKey> &keys, ClassProperties &klass, JSVal value);


private:
    std::map<std::string, const rapidjson::Value *> constants;
};

}

#endif
