#include <memory>

#include <mbgl/text/bidi.hpp>

#include <QString>

namespace mbgl {

class BiDiImpl {
public:
    QString string;
};

std::u16string applyArabicShaping(const std::u16string& input) {
    QString utf16string = QString::fromStdU16String(input);
    return utf16string.toStdU16String();
}

void BiDi::mergeParagraphLineBreaks(std::set<std::size_t>& lineBreakPoints) {
    lineBreakPoints.insert(static_cast<std::size_t>(bidi.impl->string.length()));
}

std::vector<std::u16string>
BiDi::applyLineBreaking(std::set<std::size_t> lineBreakPoints) {
    mergeParagraphLineBreaks(lineBreakPoints);

    std::vector<std::u16string> transformedLines;
    std::size_t start = 0;
    for (std::size_t lineBreakPoint : lineBreakPoints) {
        transformedLines.push_back(bidi.getLine(start, lineBreakPoint));
        start = lineBreakPoint;
    }

    return transformedLines;
}

BiDi::BiDi() : impl(std::make_unique<BiDiImpl>())
{
}

BiDi::~BiDi() = default;

std::vector<std::u16string> BiDi::processText(const std::u16string& input, std::set<std::size_t> lineBreakPoints) {
    impl->string = QString::fromStdU16String(input);
    return applyLineBreaking(lineBreakPoints);
}

std::u16string BiDi::getLine(std::size_t start, std::size_t end) {
    return impl->string.mid(static_cast<int32_t>(start), static_cast<int32_t>(end - start)).toStdU16String();
}

} // end namespace mbgl
