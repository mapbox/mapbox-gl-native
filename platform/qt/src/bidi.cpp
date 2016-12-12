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

ProcessedBiDiText::ProcessedBiDiText(BiDi& p_bidi) : bidi(p_bidi) {
}

void ProcessedBiDiText::mergeParagraphLineBreaks(std::set<int32_t>& lineBreakPoints) {
    lineBreakPoints.insert(impl->string.length());
}

std::vector<std::u16string>
ProcessedBiDiText::applyLineBreaking(std::set<int32_t> lineBreakPoints) {
    mergeParagraphLineBreaks(lineBreakPoints);

    std::vector<std::u16string> transformedLines;
    int32_t start = 0;
    for (int32_t lineBreakPoint : lineBreakPoints) {
        transformedLines.push_back(bidi.getLine(start, lineBreakPoint));
        start = lineBreakPoint;
    }

    return transformedLines;
}

BiDi::BiDi() : impl(std::make_unique<BiDiImpl>())
{
}

BiDi::~BiDi() {
}

ProcessedBiDiText BiDi::processText(const std::u16string& input) {
    impl->string = QString::fromStdU16String(input);
    return ProcessedBiDiText(*this);
}

std::u16string BiDi::getLine(int32_t start, int32_t end) {
    return impl->string.mid(start, end - start).toStdU16String();
}

} // end namespace mbgl
