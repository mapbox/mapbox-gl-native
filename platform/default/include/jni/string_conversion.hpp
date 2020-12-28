#pragma once

// This file replaces the default implementation in jni.hpp.

#include <mbgl/util/utf.hpp>

namespace jni {

inline std::u16string convertUTF8ToUTF16(const std::string& str) {
    return mbgl::util::convertUTF8ToUTF16(str);
}

inline std::string convertUTF16ToUTF8(const std::u16string& str) {
    return mbgl::util::convertUTF16ToUTF8(str);
}

} // namespace jni
