#pragma once

#include <string>

#include <mbgl/util/noncopyable.hpp>

struct UBiDiTransform;

namespace mbgl {
    
enum class WritingDirection : bool { LeftToRight, RightToLeft };    
    
class BiDi : private util::noncopyable {
public:
    BiDi();
    ~BiDi();

    std::u16string bidiTransform(const std::u16string&);
    WritingDirection baseWritingDirection(const std::u16string&);

private:
    UBiDiTransform* transform;
};

} // end namespace mbgl
