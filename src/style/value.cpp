#include <llmr/style/value.hpp>

llmr::Value llmr::parseValue(pbf data) {
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
