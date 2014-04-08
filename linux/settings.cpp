#include "settings.hpp"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"
#include "rapidjson/document.h"
#include <stdio.h>

using namespace llmr;

Settings_JSON::Settings_JSON()
{
}

Configuration Settings_JSON::load()
{
    Configuration config;
    FILE *settingsFile = fopen("/tmp/llmr-native.json", "r");
    if (settingsFile != NULL) {
        rapidjson::FileStream is(settingsFile);
        rapidjson::Document document;
        document.ParseStream<0>(is);
        if (document.IsArray()) {
            config.longitude = document[rapidjson::SizeType(0)].GetDouble();
            config.latitude = document[1].GetDouble();
            config.scale = document[2].GetDouble();
            config.angle = document[3].GetDouble();
            config.debug = document[4].GetBool();
        }
    }
    return config;
}

void Settings_JSON::persist(const Configuration &config)
{
    configuration = config;
}

void Settings_JSON::sync()
{
    rapidjson::FileStream s(fopen("/tmp/llmr-native.json", "w"));
    rapidjson::PrettyWriter<rapidjson::FileStream> writer(s);
    writer.StartArray();
    writer.Double(configuration.longitude);
    writer.Double(configuration.latitude);
    writer.Double(configuration.scale);
    writer.Double(configuration.angle);
    writer.Bool(configuration.debug);
    writer.EndArray();
}

void Settings_JSON::clear()
{
    configuration.longitude = 0;
    configuration.latitude = 0;
    configuration.scale = 0;
    configuration.angle = 0;
    configuration.debug = false;
}
