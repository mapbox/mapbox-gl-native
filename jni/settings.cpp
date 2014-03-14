#include "settings.hpp"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"
#include "rapidjson/document.h"
#include <stdio.h>

using namespace llmr;

Settings_Android::Settings_Android()
{
}

void Settings_Android::load()
{
}

void Settings_Android::persist()
{
}

void Settings_Android::sync()
{
}

void Settings_Android::clear()
{
    longitude = 0;
    latitude = 0;
    scale = 0;
    angle = 0;
    debug = false;
}

