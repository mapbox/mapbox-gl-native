#pragma once

#include <set>
#include <string>
#include <vector>
#include <memory>

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

class BiDi;
class BiDiImpl;

std::u16string applyArabicShaping(const std::u16string&);

// StyledText pairs each code point in a string with an integer indicating
// the styling options to use for rendering that code point
// The data structure is intended to accomodate the reordering/interleaving
// of formatting that can happen when BiDi rearranges inputs
using StyledText = std::pair<std::u16string, std::vector<uint8_t>>;
    
class BiDi : private util::noncopyable {
public:
    BiDi();
    ~BiDi();

    // Given text in logical ordering and a set of line break points,
    // return a set of lines in visual order with bidi and line breaking applied
    std::vector<std::u16string> processText(const std::u16string&, std::set<std::size_t>);
    // Same as processText but preserves per-code-point formatting information
    std::vector<StyledText> processStyledText(const StyledText&, std::set<std::size_t>);

private:
    void mergeParagraphLineBreaks(std::set<std::size_t>&);
    std::vector<std::u16string> applyLineBreaking(std::set<std::size_t>);
    std::u16string getLine(std::size_t start, std::size_t end);
    std::u16string writeReverse(const std::u16string&, std::size_t, std::size_t);

    std::unique_ptr<BiDiImpl> impl;
};

} // end namespace mbgl
