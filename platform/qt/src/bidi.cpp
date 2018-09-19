#include <memory>

#include <mbgl/text/bidi.hpp>

namespace mbgl {

// This stub implementation is stateless and doesn't implement the private
// methods used by the ICU BiDi
class BiDiImpl {
    // Used by the ICU implementation to hold onto internal BiDi state
};

std::u16string applyArabicShaping(const std::u16string& input) {
    return input;
}

BiDi::BiDi() : impl(std::make_unique<BiDiImpl>())
{
}

BiDi::~BiDi() = default;

std::vector<std::u16string> BiDi::processText(const std::u16string& input, std::set<std::size_t> lineBreakPoints) {
    lineBreakPoints.insert(input.length());

    std::vector<std::u16string> transformedLines;
    std::size_t start = 0;
    for (std::size_t lineBreakPoint : lineBreakPoints) {
        transformedLines.push_back(input.substr(start, lineBreakPoint - start));
        start = lineBreakPoint;
    }

    return transformedLines;
}

std::vector<StyledText> BiDi::processStyledText(const StyledText& input, std::set<std::size_t> lineBreakPoints) {
    lineBreakPoints.insert(input.first.length());

    std::vector<StyledText> transformedLines;
    std::size_t start = 0;
    for (std::size_t lineBreakPoint : lineBreakPoints) {
        transformedLines.emplace_back(
            input.first.substr(start, lineBreakPoint - start),
            std::vector<uint8_t>(input.second.begin() + start, input.second.begin() + lineBreakPoint));
        start = lineBreakPoint;
    }

    return transformedLines;
}

} // end namespace mbgl
