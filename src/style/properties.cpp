#include <llmr/style/properties.hpp>


#include <cassert>

using namespace llmr;


float functions::null(float z, const std::vector<float>&) {
    return 0;
}

bool functions::null(float z, const std::vector<bool>&) {
    return false;
}

float functions::constant(float z, const std::vector<float>& values) {
    assert(values.size() == 1);
    return values.front();
}

bool functions::constant(float z, const std::vector<bool>& values) {
    assert(values.size() == 1);
    return values.front();
}