#pragma once

#include <string>

#define ANSI_COLOR_RED        "\x1b[31m"
#define ANSI_COLOR_GREEN      "\x1b[32m"
#define ANSI_COLOR_YELLOW     "\x1b[33m"
#define ANSI_COLOR_LIGHT_GRAY "\x1b[90m"
#define ANSI_COLOR_RESET      "\x1b[0m"

class TestStats;

void printStats(const TestStats&);
void writeHTMLResults(const TestStats&, const std::string&, bool, uint32_t);
