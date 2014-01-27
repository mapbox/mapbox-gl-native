#include <llmr/style/value.hpp>

llmr::Value llmr::parseValue(pbf data) {
    while (data.next()) {
        if (data.tag == 1) { // string_value
            return data.string();
        } else if (data.tag == 2) { // float_value
            return data.float32();
        } else if (data.tag == 3) { // double_value
            return data.float64();
        } else if (data.tag == 4) { // int_value
            return data.varint<int64_t>();
        } else if (data.tag == 5) { // uint_value
            return data.varint<uint64_t>();
        } else if (data.tag == 6) { // sint_value
            return data.svarint<int64_t>();
        } else if (data.tag == 7) { // bool_value
            return data.boolean();
        } else {
            data.skip();
        }
    }

    return false;
}
