#include <llmr/style/style_parser.hpp>
#include <llmr/util/constants.hpp>
#include <csscolorparser/csscolorparser.hpp>

using namespace llmr;

using JSVal = const rapidjson::Value&;

void StyleParser::parseBuckets(JSVal value, std::map<std::string, BucketDescription>& buckets) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            buckets.emplace(
                std::forward<std::string>({ itr->name.GetString(), itr->name.GetStringLength() }),
                std::forward<BucketDescription>(parseBucket(itr->name, itr->value))
            );
        }
    } else {
        throw Style::exception("buckets must be an object");
    }
}

PropertyFilterExpression StyleParser::parseFilterOrExpression(JSVal value) {
    if (value.IsArray()) {
        // This is an expression.
        util::recursive_wrapper<PropertyExpression> expression;
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            JSVal filter_item = value[i];

            if (filter_item.IsString()) {
                expression.get().op = expressionOperatorType({ filter_item.GetString(), filter_item.GetStringLength() });
            } else {
                expression.get().operands.emplace_back(parseFilterOrExpression(filter_item));
            }
        }
        return std::move(expression);
    } else if (value.IsObject() && value.HasMember("field") && value.HasMember("value")) {
        // This is a filter.
        JSVal field = value["field"];
        JSVal val = value["value"];

        if (!field.IsString()) {
            throw Style::exception("field name must be a string");
        }

        const std::string field_name { field.GetString(), field.GetStringLength() };
        const FilterOperator op = [&]{
            if (value.HasMember("operator")) {
                JSVal op_val = value["operator"];
                return filterOperatorType({ op_val.GetString(), op_val.GetStringLength() });
            } else {
                return FilterOperator::Equal;
            }
        }();

        if (val.IsArray()) {
            // The filter has several values, so it's an OR sub-expression.
            util::recursive_wrapper<PropertyExpression> expression;
            for (rapidjson::SizeType i = 0; i < val.Size(); ++i) {
                expression.get().operands.emplace_back(util::recursive_wrapper<PropertyFilter>(PropertyFilter { field_name, op, parseValue(val[i]) }));
            }

            return std::move(expression);
        } else {
            // The filter only has a single value, so it is a real filter.
            return std::move(util::recursive_wrapper<PropertyFilter>(PropertyFilter { field_name, op, parseValue(val) }));
        }
    }

    return std::true_type();
}

BucketDescription StyleParser::parseBucket(JSVal bucketName, JSVal value) {
    BucketDescription bucket;
    bucket.type = BucketType::None;

    rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
    std::string name;

    // Start by figuring out the bucket type
    if (value.HasMember("text")) {
        bucket.type = BucketType::Text;

    } else if (value.HasMember("point")) {
        bucket.type = BucketType::Icon;

    } else if (value.HasMember("line")) {
        bucket.type = BucketType::Line;

    } else if (value.HasMember("fill")) {
        bucket.type = BucketType::Fill;

    } else {
        printf("Missing {text|point|line|fill} for bucket '%s'\n", bucketName.GetString());
    }

    // Parse the rest of the bucket
    for (; itr != value.MemberEnd(); ++itr) {
        name = {itr->name.GetString(), itr->name.GetStringLength() };
        JSVal value = itr->value;

        if (name == "filter") {
            if (value.IsObject()) {

                // Determine the feature_type, using bucket.type if nothing else is specified in the filter
                if (value.HasMember("feature_type")) {
                    JSVal featureTypeValue = value["feature_type"];
                    if (featureTypeValue.IsString()) {
                        bucket.feature_type = bucketType({ featureTypeValue.GetString(), featureTypeValue.GetStringLength() });
                    } else {
                        throw Style::exception("feature type must be a string");
                    }
                } else {
                    bucket.feature_type = bucket.type;
                }

                // Parse the rest of the filter object
                rapidjson::Value::ConstMemberIterator itr2 = value.MemberBegin();
                for (; itr2 != value.MemberEnd(); ++itr2) {
                    const std::string name2(itr2->name.GetString(), itr2->name.GetStringLength());
                    JSVal value2 = itr2->value;

                    if (name2 == "source") {
                        if (value2.IsString()) {
                            bucket.source_name = { value2.GetString(), value2.GetStringLength() };
                        } else {
                            throw Style::exception("source name must be a string");
                        }

                    } else if (name2 == "layer") {
                        if (value2.IsString()) {
                            bucket.source_layer = { value2.GetString(), value2.GetStringLength() };
                        } else {
                            throw Style::exception("layer name must be a string");
                        }

                    }
                }

                bucket.filter = std::move(parseFilterOrExpression(value));
            } else {
                throw Style::exception("filter type must be a object");
            }

        } else if (name == "line-cap") {
            if (value.IsString()) {
                bucket.geometry.cap = capType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("line-cap type must be a string");
            }
        } else if (name == "line-join") {
            if (value.IsString()) {
                bucket.geometry.join = joinType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("line-join type must be a string");
            }
        } else if (name == "text-font") {
            if (value.IsString()) {
                bucket.geometry.font = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("font stack must be a string");
            }
        } else if (name == "text-max-size" || name == "size") {
            if (value.IsNumber()) {
                bucket.geometry.size = value.GetDouble();
            } else {
                throw Style::exception("font size must be a number");
            }
        } else if (name == "text-field" || name == "icon") {
            if (value.IsString()) {
                bucket.geometry.field = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("text field must be a string");
            }
        } else if (name == "text-path") {
            if (value.IsString()) {
                bucket.geometry.path = textPathType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("text-path must be a string");
            }
        } else if (name == "line-miter-limit") {
            if (value.IsNumber()) {
                bucket.geometry.miter_limit = value.GetDouble();
            } else {
                throw Style::exception("line miter limit must be a number");
            }
        } else if (name == "line-round-limit") {
            if (value.IsNumber()) {
                bucket.geometry.round_limit = value.GetDouble();
            } else {
                throw Style::exception("line round limit must be a number");
            }
        } else if (name == "text-min-distance") {
            if (value.IsNumber()) {
                bucket.geometry.textMinDistance = value.GetDouble();
            } else {
                throw Style::exception("text min distance must be a number");
            }
        } else if (name == "text-max-angle") {
            if (value.IsNumber()) {
                bucket.geometry.maxAngleDelta = value.GetDouble();
            } else {
                throw Style::exception("text max angle delta must be a number");
            }
        }
    }

    if (value.HasMember("filter")) {
        bucket.filter = std::move(parseFilterOrExpression(value["filter"]));
    } else {
        // Allow a filter triple to be specified at the bucket root as well.
        bucket.filter = std::move(parseFilterOrExpression(value));
    }

    if (bucket.feature_type == BucketType::None) {
        printf("No feature_type detected for %s\n",bucketName.GetString());
        bucket.feature_type = bucket.type;
    }

    return bucket;
}

void StyleParser::parseLayers(JSVal value, std::vector<LayerDescription>& layers) {
    if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            layers.push_back(parseLayer(value[i]));
        }
    } else {
        throw Style::exception("layers must be an array");
    }
}

LayerDescription StyleParser::parseLayer(JSVal value) {
    LayerDescription layer;

    if (value.IsObject()) {
        bool layerIsBackground = false;
        if (value.HasMember("id")) {
            JSVal name = value["id"];
            if (name.IsString()) {
                layer.name = { name.GetString(), name.GetStringLength() };
                if(layer.name == "background") {
                    layerIsBackground = true;
                }
            } else {
                throw Style::exception("layers element id must be a string");
            }
        } else {
            throw Style::exception("layers element must have an id");
        }

        if (value.HasMember("bucket")) {
            JSVal bucket = value["bucket"];
            if (bucket.IsString()) {
                layer.bucket_name = { bucket.GetString(), bucket.GetStringLength() };
            } else {
                throw Style::exception("layers element bucket must be a string");
            }
        } else if (value.HasMember("layers")) {
            parseLayers(value["layers"], layer.child_layer);
        } else if (layerIsBackground) {
            layer.bucket_name = "background";
        } else {
            throw Style::exception("layers element must be the background, have a bucket name, or have child layers");
        }
    } else {
        throw Style::exception("layer element must be an object");
    }

    return layer;
}

void StyleParser::parseConstants(JSVal value) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            std::string memberName = { itr->name.GetString(), itr->name.GetStringLength() };
            constants.emplace(
                std::forward<std::string>(memberName),
                &itr->value
            );
        }
    } else {
        throw Style::exception("constants must be an object");
    }
}

TranslateAnchor parseTranslateAnchor(JSVal anchor) {
    if (anchor.IsString()) {
        std::string a { anchor.GetString(), anchor.GetStringLength() };
        if (a == "viewport") {
            return TranslateAnchor::Viewport;
        } else {
            return TranslateAnchor::Map;
        }
    } else {
        throw Style::exception("translate anchor must be a string");
    }
}


void collectLayerBuckets(std::map<std::string, std::string> &layerBuckets,
                         const std::vector<LayerDescription> &layers) {
    for (const auto& layer : layers) {
        layerBuckets.emplace(layer.name, layer.bucket_name);
        if (layer.child_layer.size()) {
            collectLayerBuckets(layerBuckets, layer.child_layer);
        }
    }
}

void StyleParser::parseClasses(JSVal value,
                               std::map<std::string, ClassDescription> &classes,
                               const std::map<std::string, BucketDescription> &buckets,
                               const std::vector<LayerDescription> &layers) {

    // Build a non-recursive mapping of layer => bucket.
    std::map<std::string, std::string> layerBuckets;
    collectLayerBuckets(layerBuckets, layers);

    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        std::string styleName;
        for (; itr != value.MemberEnd(); ++itr) {
            styleName = { itr->name.GetString(), itr->name.GetStringLength() };
            classes.insert(std::forward<std::pair<std::string, ClassDescription>>(parseClassDescription(styleName, itr->value, buckets, layerBuckets)));
        }
    } else {
        throw Style::exception("styles must be an object");
    }
}

std::pair<std::string, ClassDescription>
StyleParser::parseClassDescription(std::string styleName, JSVal value,
                                   const std::map<std::string, BucketDescription> &buckets,
                                   const std::map<std::string, std::string> &layerBuckets) {
    ClassDescription klass;

    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            const std::string name {
                itr->name.GetString(), itr->name.GetStringLength()
            };
            parseClass(name, itr->value, klass, buckets, layerBuckets);
        }
    }

    return { styleName, std::forward<ClassDescription>(klass) };
}

void StyleParser::parseClass(const std::string &name, JSVal value, ClassDescription &class_desc,
                             const std::map<std::string, BucketDescription> &buckets,
                             const std::map<std::string, std::string> &layerBuckets) {
    if (!value.IsObject()) {
        throw Style::exception("style class must be an object");
    }

    auto layer_bucket_it = layerBuckets.find(name);
    if (layer_bucket_it == layerBuckets.end()) {
        if (debug::styleParseWarnings) {
            fprintf(stderr, "[WARNING] there is no layer associated with '%s'\n", name.c_str());
        }
        return;
    }

    const std::string &bucket_name = layer_bucket_it->second;
    if (bucket_name == "background") {
        // background buckets are fake
        class_desc.background = parseBackgroundClass(value);
    } else if (bucket_name.length() == 0) {
        // no bucket name == composite bucket.
        class_desc.composite.insert({ name, std::forward<CompositeClass>(parseCompositeClass(value)) });
    } else {
        auto bucket_it = buckets.find(bucket_name);
        if (bucket_it == buckets.end()) {
            if (debug::styleParseWarnings) {
                fprintf(stderr, "[WARNING] there is no bucket named '%s'\n", bucket_name.c_str());
            }
            return;
        }

        BucketType type = bucket_it->second.type;
        if (type == BucketType::Fill) {
            class_desc.fill.insert({ name, std::forward<FillClass>(parseFillClass(value)) });
        } else if (type == BucketType::Line) {
            class_desc.line.insert({ name, std::forward<LineClass>(parseLineClass(value)) });
        } else if (type == BucketType::Icon) {
            class_desc.icon.insert({ name, std::forward<IconClass>(parseIconClass(value)) });
        } else if (type == BucketType::Text) {
            class_desc.text.insert({ name, std::forward<TextClass>(parseTextClass(value)) });
        } else if (type == BucketType::Raster) {
            class_desc.raster.insert({ name, std::forward<RasterClass>(parseRasterClass(value)) });
        } else {
            throw Style::exception("unknown class type name");
        }
    }
}

bool StyleParser::parseBoolean(JSVal value) {
    if (!value.IsBool()) {
        throw Style::exception("boolean value must be a boolean");
    }

    return value.GetBool();
}

std::string StyleParser::parseString(JSVal value) {
    if (!value.IsString()) {
        throw Style::exception("string value must be a string");
    }

    return { value.GetString(), value.GetStringLength() };
}

JSVal StyleParser::replaceConstant(JSVal value) {
    // Attempt to retrieve the value from the constants map for the given key string
    if (value.IsString()) {
        const std::string key { value.GetString(), value.GetStringLength() };
        auto it = constants.find(key);
        if (it != constants.end()) {
            // The possible constant is indeed a key in the constants map, so "replace" it by returning the associated value
            return *it->second;
        }
    }

    // The possibleConstant was not actually a constant, so return it as-is with no "replacement"
    return value;
}

std::vector<FunctionProperty> StyleParser::parseArray(JSVal value, uint16_t expected_count) {
    JSVal rvalue = replaceConstant(value);
    if (!rvalue.IsArray()) {
        throw Style::exception("array value must be an array");
    }

    if (rvalue.IsArray() && rvalue.Size() != expected_count) {
        throw Style::exception("array value has unexpected number of elements");
    }

    std::vector<FunctionProperty> values;
    values.reserve(expected_count);
    for (uint16_t i = 0; i < expected_count; i++) {
        values.push_back(parseFunction(rvalue[(rapidjson::SizeType)i]));
    }

    return values;
}

Color StyleParser::parseColor(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    if (rvalue.IsArray()) {
        // [ r, g, b, a] array
        if (rvalue.Size() != 4) {
            throw Style::exception("color array must have four elements");
        }

        JSVal r = rvalue[(rapidjson::SizeType)0], g = rvalue[1], b = rvalue[2], a = rvalue[3];
        if (!r.IsNumber() || !g.IsNumber() || !b.IsNumber() || !a.IsNumber()) {
            throw Style::exception("color values must be numbers");
        }

        // Premultiply the color.
        const double alpha = a.GetDouble();
        return {{static_cast<float>(alpha * r.GetDouble()),
                 static_cast<float>(alpha * g.GetDouble()),
                 static_cast<float>(alpha * b.GetDouble()),
                 static_cast<float>(alpha)}};

    } else if (!rvalue.IsString()) {
        throw Style::exception("color value must be a string");
    }

    // Parse and premultiply the color, using local variables so the values are easy to check in the debugger.
    const char * c_color_string = rvalue.GetString();
    CSSColorParser::Color css_color = CSSColorParser::parse({ c_color_string, rvalue.GetStringLength()});
    const float factor = css_color.a / 255;
    float red   = (float)css_color.r * factor;
    float green = (float)css_color.g * factor;
    float blue  = (float)css_color.b * factor;
    float alpha = css_color.a;
    return {{red, green, blue, alpha}};
}

FunctionProperty::fn StyleParser::parseFunctionType(JSVal type) {
    if (type.IsString()) {
        std::string t { type.GetString(), type.GetStringLength() };
        if (t == "constant") {
            return &functions::constant;
        } else if (t == "linear") {
            return &functions::linear;
        } else if (t == "stops") {
            return &functions::stops;
        } else if (t == "exponential") {
            return &functions::exponential;
        } else if (t == "min") {
            return &functions::min;
        } else if (t == "max") {
            return &functions::max;
        } else {
            throw Style::exception("unknown function type");
        }
    } else {
        throw Style::exception("function type must be a string");
    }
}

FunctionProperty StyleParser::parseFunction(JSVal value) {
    JSVal rvalue = replaceConstant(value);

    FunctionProperty property;

    if (rvalue.IsArray()) {
        if (rvalue.Size() < 1) {
            throw Style::exception("value function does not have arguments");
        }

        property.function = parseFunctionType(rvalue[(rapidjson::SizeType)0]);
        for (rapidjson::SizeType i = 1; i < rvalue.Size(); ++i) {
            JSVal stop = rvalue[i];
            if (stop.IsObject()) {
                if (!stop.HasMember("z")) {
                    throw Style::exception("stop must have zoom level specification");
                }
                JSVal z = stop["z"];
                if (!z.IsNumber()) {
                    throw Style::exception("zoom level in stops must be a number");
                }
                property.values.push_back(z.GetDouble());

                if (!stop.HasMember("val")) {
                    throw Style::exception("stop must have value specification");
                }
                JSVal val = stop["val"];
                if (!val.IsNumber()) {
                    throw Style::exception("value in stops must be a number");
                }
                property.values.push_back(val.GetDouble());
            } else if (stop.IsNumber()) {
                property.values.push_back(stop.GetDouble());
            } else if (stop.IsBool()) {
                property.values.push_back(stop.GetBool());
            } else {
                throw Style::exception("function argument must be a numeric value");
            }
        }
    } else if (rvalue.IsNumber()) {
        property.function = &functions::constant;
        property.values.push_back(rvalue.GetDouble());

    } else if (rvalue.IsObject()) {

        // TODO: implement linear, exponential, min, max, or whatever else is missing

        // Handle the v1 object syntax for the stops function
        if (rvalue.HasMember("fn") && rvalue["fn"].IsString() && rvalue["fn"].GetString() == std::string("stops")) {
            if ( rvalue.HasMember("stops") && rvalue["stops"].IsArray()) {
                for (rapidjson::SizeType i = 0; i < rvalue["stops"].Size(); ++i) {
                    JSVal stop = rvalue["stops"][i];
                    rapidjson::SizeType z = 0;
                    rapidjson::SizeType v = 1;
                    if (stop.IsArray() && stop.Size() == 2 && stop[z].IsNumber() && stop[v].IsNumber()) {
                        float zoomStop    = stop[z].GetDouble();
                        float valueAtZoom = stop[v].GetDouble();
                        property.values.push_back(zoomStop);
                        property.values.push_back(valueAtZoom);
                    } else {
                        throw Style::exception("inner arrays of the stops function should contain 2 numbers");
                    }
                }
            } else {
                throw Style::exception("stops function requires a stops array which was either not found or mal-formed");
            }
        }
    }

    return property;
}

boost::optional<PropertyTransition> StyleParser::parseTransition(JSVal value, std::string property_name) {
    uint16_t duration = 0, delay = 0;
    std::string transition_property = std::string("transition-").append(property_name);
    if (value.HasMember(transition_property.c_str())) {
        JSVal elements = value[transition_property.c_str()];
        if (elements.IsObject()) {
            if (elements.HasMember("duration") && elements["duration"].IsNumber()) {
                duration = elements["duration"].GetUint();
            }
            if (elements.HasMember("delay") && elements["delay"].IsNumber()) {
                delay = elements["delay"].GetUint();
            }
        }
    }

    if (duration || delay) {
        return boost::optional<PropertyTransition>(PropertyTransition { duration, delay });
    } else {
        return boost::optional<PropertyTransition>();
    }
}

void StyleParser::parseGenericClass(GenericClass &klass, JSVal value) {
    if (value.HasMember("enabled")) {
        klass.enabled = parseFunction(value["enabled"]);
    }

    if (value.HasMember("translate-anchor")) {
        klass.translateAnchor = parseTranslateAnchor(value["translate-anchor"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFunction(value["opacity"]);
        klass.opacity_transition = parseTransition(value, "opacity");
    }

    if (value.HasMember("prerender")) {
        klass.prerender = parseFunction(value["prerender"]);
    }

    if (value.HasMember("prerender-buffer")) {
        klass.prerenderBuffer = parseFunction(value["prerender-buffer"]);
    }

    if (value.HasMember("prerender-size")) {
        klass.prerenderSize = parseFunction(value["prerender-size"]);
    }

    if (value.HasMember("prerender-blur")) {
        klass.prerenderBlur = parseFunction(value["prerender-blur"]);
    }
}

FillClass StyleParser::parseFillClass(JSVal value) {
    FillClass klass;

    parseGenericClass(klass, value);

    if (value.HasMember("fill-color")) {
        klass.fill_color = parseColor(value["fill-color"]);
        klass.fill_color_transition = parseTransition(value, "fill-color");
    }

    if (value.HasMember("stroke-color")) {
        klass.stroke_color = parseColor(value["stroke-color"]);
        klass.stroke_color_transition = parseTransition(value, "stroke-color");
    }

    if (value.HasMember("fill-translate")) {
        std::vector<FunctionProperty> values = parseArray(value["fill-translate"], 2);
        klass.translate = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
        klass.translate_transition = parseTransition(value, "translate");
    }

    if (value.HasMember("winding")) {
        throw std::runtime_error("winding in stylesheets not yet supported");
    }

    if (value.HasMember("fill-antialias")) {
        klass.antialias = parseBoolean(value["fill-antialias"]);
    }

    if (value.HasMember("fill-image")) {
        klass.image = parseString(value["fill-image"]);
    }

    if (value.HasMember("fill-opacity")) {
        klass.opacity = parseFunction(value["fill-opacity"]);
    }

    return klass;
}

LineClass StyleParser::parseLineClass(JSVal value) {
    LineClass klass;

    parseGenericClass(klass, value);

    if (value.HasMember("enabled")) {
        klass.enabled = parseFunction(value["enabled"]);
    }

    if (value.HasMember("line-translate")) {
        std::vector<FunctionProperty> values = parseArray(value["line-translate"], 2);
        klass.translate = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
    }

    if (value.HasMember("translate-anchor")) {
        klass.translateAnchor = parseTranslateAnchor(value["translate-anchor"]);
    }

    if (value.HasMember("line-color")) {
        klass.color = parseColor(value["line-color"]);
        klass.color_transition = parseTransition(value, "line-color");
    }

    if (value.HasMember("line-width")) {
        klass.width = parseFunction(value["line-width"]);
        klass.width_transition = parseTransition(value, "line-width");
    }

    if (value.HasMember("line-opacity")) {
        klass.opacity = parseFunction(value["line-opacity"]);
    }

    if (value.HasMember("line-dasharray")) {
        std::vector<FunctionProperty> values = parseArray(value["line-dasharray"], 2);
        klass.dash_array = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
        klass.dash_array_transition = parseTransition(value, "dasharray");
    }

    return klass;
}

IconClass StyleParser::parseIconClass(JSVal value) {
    IconClass klass;

    parseGenericClass(klass, value);

    if (value.HasMember("point-color")) {
        klass.color = parseColor(value["point-color"]);
        klass.color_transition = parseTransition(value, "point-color");
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFunction(value["opacity"]);
    }

    if (value.HasMember("point-image")) {
        klass.image = parseString(value["point-image"]);
    }

    if (value.HasMember("point-size")) {
        klass.size = parseFunction(value["point-size"]);
    }

    if (value.HasMember("point-radius")) {
        klass.radius = parseFunction(value["point-radius"]);
        klass.radius_transition = parseTransition(value, "point-radius");
    }

    if (value.HasMember("point-blur")) {
        klass.blur = parseFunction(value["point-blur"]);
        klass.blur_transition = parseTransition(value, "point-blur");
    }

    return klass;
}

TextClass StyleParser::parseTextClass(JSVal value) {
    TextClass klass;

    parseGenericClass(klass, value);

    if (value.HasMember("enabled")) {
        klass.enabled = parseFunction(value["enabled"]);
    }

    if (value.HasMember("text-translate")) {
        std::vector<FunctionProperty> values = parseArray(value["text-translate"], 2);
        klass.translate = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
    }

    if (value.HasMember("translate-anchor")) {
        klass.translateAnchor = parseTranslateAnchor(value["translate-anchor"]);
    }

    if (value.HasMember("text-color")) {
        klass.color = parseColor(value["text-color"]);
        klass.color_transition = parseTransition(value, "text-color");
    }

    if (value.HasMember("text-size")) {
        klass.size = parseFunction(value["text-size"]);
    }

    if (value.HasMember("text-rotate")) {
        klass.rotate = parseFunction(value["text-rotate"]);
    }

    if (value.HasMember("text-always-visible")) {
        klass.always_visible = parseFunction(value["text-always-visible"]);
    }

    if (value.HasMember("text-halo-color")) {
        klass.halo = parseColor(value["text-halo-color"]);
        klass.halo_transition = parseTransition(value, "text-halo-color");
    }

    if (value.HasMember("text-halo-width")) {
        klass.halo_radius = parseFunction(value["text-halo-width"]);
        klass.halo_radius_transition = parseTransition(value, "text-halo-width");
    }

    if (value.HasMember("text-halo-blur")) {
        klass.halo_blur = parseFunction(value["text-halo-blur"]);
        klass.halo_blur_transition = parseTransition(value, "text-halo-blur");
    }

    return klass;
}

RasterClass StyleParser::parseRasterClass(JSVal value) {
    RasterClass klass;

    parseGenericClass(klass, value);

    return klass;
}

CompositeClass StyleParser::parseCompositeClass(JSVal value) {
    CompositeClass klass;

    parseGenericClass(klass, value);

    return klass;
}

BackgroundClass StyleParser::parseBackgroundClass(JSVal value) {
    BackgroundClass klass;

    parseGenericClass(klass, value);

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
        klass.color_transition = parseTransition(value, "color");
    }

    return klass;
}

Value StyleParser::parseValue(JSVal value) {
    switch (value.GetType()) {
        case rapidjson::kNullType:
        case rapidjson::kFalseType:
            return false;

        case rapidjson::kTrueType:
            return true;

        case rapidjson::kStringType:
            return std::string { value.GetString(), value.GetStringLength() };

        case rapidjson::kNumberType:
            if (value.IsUint64()) return value.GetUint64();
            if (value.IsInt64()) return value.GetInt64();
            return value.GetDouble();

        case rapidjson::kObjectType:
        case rapidjson::kArrayType:
            throw Style::exception("value cannot be an object or array");
            return false;
    }
    throw Style::exception("unhandled value type in style");
    return false;
}
