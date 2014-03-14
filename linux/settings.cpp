#include "settings.hpp"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"
#include "rapidjson/document.h"
#include <stdio.h>

using namespace llmr;

Settings_JSON::Settings_JSON()
{
}

void Settings_JSON::load()
{
    FILE *settingsFile = fopen("/tmp/llmr-native.json", "r");
    if (settingsFile != NULL) {
        rapidjson::FileStream is(settingsFile);
        rapidjson::Document document;
        document.ParseStream<0>(is);
        if (document.IsArray()) {
            longitude = document[rapidjson::SizeType(0)].GetDouble();
            latitude = document[1].GetDouble();
            scale = document[2].GetDouble();
            angle = document[3].GetDouble();
            debug = document[4].GetBool();
        }
    }
}

void Settings_JSON::persist()
{
}

void Settings_JSON::sync()
{

    rapidjson::FileStream s(fopen("/tmp/llmr-native.json", "w"));
    rapidjson::PrettyWriter<rapidjson::FileStream> writer(s);
    writer.StartArray();
    writer.Double(longitude);
    writer.Double(latitude);
    writer.Double(scale);
    writer.Double(angle);
    writer.Bool(debug);
    writer.EndArray();
}

void Settings_JSON::clear()
{
    longitude = 0;
    latitude = 0;
    scale = 0;
    angle = 0;
    debug = false;
}
