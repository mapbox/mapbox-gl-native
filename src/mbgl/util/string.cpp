#include <mbgl/util/string.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <cassert>

namespace mbgl {
namespace util {

std::string toString(int32_t t) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.Int(t);
    return s.GetString();
}

std::string toString(uint32_t t) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.Uint(t);
    return s.GetString();
}

std::string toString(int64_t t) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.Int64(t);
    return s.GetString();
}

std::string toString(uint64_t t) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.Uint64(t);
    return s.GetString();
}

std::string toString(double t, bool decimal) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.Double(t);
    std::string data = s.GetString();
    if (!decimal && data.length() >= 3 && data[data.length() - 1] == '0' && data[data.length() - 2] == '.') {
        // Remove trailing ".0" for integers
        data.resize(data.length() - 2);
    }
    return data;
}

std::string toString(std::exception_ptr error) {
    assert(error);

    if (!error) {
        return "(null)";
    }

    try {
        std::rethrow_exception(error);
    } catch (const std::exception& ex) {
        return ex.what();
    } catch (...) {
        return "Unknown exception type";
    }
}

namespace {

template <typename T>
std::string toPaddedHex(T x) {
    std::string result;
    result.resize(sizeof(T) * 2);
    for (int index = sizeof(T) * 2 - 1; index >= 0; index--) {
        const int digit = x & 0x0F;
        result[index] = '0' + digit + (digit > 9 ? 39 : 0);
        x >>= 4;
    }
    return result;
}

} // namespace

std::string toHex(uint32_t value) {
    return toPaddedHex(value);
}

std::string toHex(uint64_t value) {
    return toPaddedHex(value);
}

} // namespace util
} // namespace mbgl
