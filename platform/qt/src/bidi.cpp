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

void BiDi::mergeParagraphLineBreaks(std::set<int32_t>& lineBreakPoints) {
    lineBreakPoints.insert(bidi.impl->string.length());
}

std::vector<std::u16string>
BiDi::applyLineBreaking(std::set<int32_t> lineBreakPoints) {
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

std::vector<std::u16string> BiDi::processText(const std::u16string& input, std::set<int32_t> lineBreakPoints) {
    impl->string = QString::fromStdU16String(input);
    return applyLineBreaking(lineBreakPoints);
}

std::u16string BiDi::getLine(int32_t start, int32_t end) {
    return impl->string.mid(start, end - start).toStdU16String();
}

} // end namespace mbgl
