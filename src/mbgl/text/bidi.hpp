#pragma once

#include <string>

#include <mbgl/util/noncopyable.hpp>
#include <unicode/ubiditransform.h>

namespace mbgl {

class BiDi : private util::noncopyable {
public:
    BiDi();
    ~BiDi();

    std::u16string bidiTransform(const std::u16string&);
    bool baseDirectionRightToLeft(const std::u16string&);

private:
    UBiDiTransform* transform;
};

} // end namespace mbgl
