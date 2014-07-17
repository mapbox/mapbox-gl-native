#include <mbgl/style/value.hpp>

#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
#include <boost/lexical_cast.hpp>
#pragma GCC diagnostic pop

mbgl::Value mbgl::parseValue(pbf data) {
    while (data.next())
    {
        switch (data.tag)
        {
        case 1: // string_value
            return data.string();
        case 2: // float_value
            return static_cast<double>(data.float32());
        case 3: // double_value
            return data.float64();
        case 4: // int_value
            return data.varint<int64_t>();
        case 5: // uint_value
            return data.varint<uint64_t>();
        case 6: // sint_value
            return data.svarint<int64_t>();
        case 7: // bool_value
            return data.boolean();
        default:
            data.skip();
            break;
        }
    }
    return false;
}

std::string mbgl::toString(const mbgl::Value& value) {
    if (value.is<std::string>()) return value.get<std::string>();
    else if (value.is<bool>()) return value.get<bool>() ? "true" : "false";
    else if (value.is<int64_t>()) return std::to_string(value.get<int64_t>());
    else if (value.is<uint64_t>()) return std::to_string(value.get<uint64_t>());
    else if (value.is<double>()) return boost::lexical_cast<std::string>(value.get<double>());
    else return "null";
}
