#include "settings_json.hpp"
#include <rapidjson/prettywriter.h>
#include <rapidjson/filestream.h>
#include <rapidjson/document.h>
#include <stdio.h>

using namespace llmr;

Settings_JSON::Settings_JSON() { load(); }

void Settings_JSON::load() {
    FILE *settingsFile = fopen("/tmp/llmr-native.json", "r");
    if (settingsFile != NULL) {
        rapidjson::FileStream is(settingsFile);
        rapidjson::Document document;
        document.ParseStream<0>(is);
        if (document.IsArray()) {
            longitude = document[rapidjson::SizeType(0)].GetDouble();
            latitude = document[1].GetDouble();
            zoom = document[2].GetDouble();
            angle = document[3].GetDouble();
            debug = document[4].GetBool();
        }
    }
}

void Settings_JSON::save() {

    rapidjson::FileStream s(fopen("/tmp/llmr-native.json", "w"));
    rapidjson::PrettyWriter<rapidjson::FileStream> writer(s);
    writer.StartArray();
    writer.Double(longitude);
    writer.Double(latitude);
    writer.Double(zoom);
    writer.Double(angle);
    writer.Bool(debug);
    writer.EndArray();
}

void Settings_JSON::clear() {
    longitude = 0;
    latitude = 0;
    zoom = 0;
    angle = 0;
    debug = false;
}
