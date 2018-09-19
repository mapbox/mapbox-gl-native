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

using StyledText = std::pair<std::u16string, std::vector<uint8_t>>;
    
class BiDi : private util::noncopyable {
public:
    BiDi();
    ~BiDi();

    std::vector<std::u16string> processText(const std::u16string&, std::set<std::size_t>);
    std::vector<StyledText> processStyledText(const StyledText&, std::set<std::size_t>);

private:
    void mergeParagraphLineBreaks(std::set<std::size_t>&);
    std::vector<std::u16string> applyLineBreaking(std::set<std::size_t>);
    std::u16string getLine(std::size_t start, std::size_t end);
    std::u16string writeReverse(const std::u16string&, std::size_t, std::size_t);

    std::unique_ptr<BiDiImpl> impl;
};

} // end namespace mbgl
