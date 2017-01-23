#include <memory>

#include <mbgl/text/bidi.hpp>

#include <QString>

namespace mbgl {

class BiDiImpl {
public:
    QString string;
};

std::u16string applyArabicShaping(const std::u16string& input) {
    return input;
}

void BiDi::mergeParagraphLineBreaks(std::set<std::size_t>& lineBreakPoints) {
    lineBreakPoints.insert(static_cast<std::size_t>(impl->string.length()));
}

std::vector<std::u16string>
BiDi::applyLineBreaking(std::set<std::size_t> lineBreakPoints) {
    mergeParagraphLineBreaks(lineBreakPoints);

    std::vector<std::u16string> transformedLines;
    std::size_t start = 0;
    for (std::size_t lineBreakPoint : lineBreakPoints) {
        transformedLines.push_back(getLine(start, lineBreakPoint));
        start = lineBreakPoint;
    }

    return transformedLines;
}

BiDi::BiDi() : impl(std::make_unique<BiDiImpl>())
{
}

BiDi::~BiDi() = default;

std::vector<std::u16string> BiDi::processText(const std::u16string& input, std::set<std::size_t> lineBreakPoints) {
    impl->string = QString::fromUtf16(reinterpret_cast<const ushort*>(input.data()), int(input.size()));
    return applyLineBreaking(lineBreakPoints);
}

std::u16string BiDi::getLine(std::size_t start, std::size_t end) {
    auto utf16 = impl->string.mid(static_cast<int32_t>(start), static_cast<int32_t>(end - start));
    return std::u16string(reinterpret_cast<const char16_t*>(utf16.utf16()), utf16.length());
}

} // end namespace mbgl
