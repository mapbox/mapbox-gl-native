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

class ProcessedBiDiText {
public:
    ProcessedBiDiText(BiDi&);

    std::vector<std::u16string> applyLineBreaking(std::set<int32_t>);

private:
    void mergeParagraphLineBreaks(std::set<int32_t>&);

    BiDi& bidi;
};

class BiDi : private util::noncopyable {
public:
    BiDi();
    ~BiDi();

    // Calling processText resets internal state, invalidating any existing ProcessedBiDiText
    // objects
    ProcessedBiDiText processText(const std::u16string&);

    friend class ProcessedBiDiText;

private:
    std::u16string getLine(int32_t start, int32_t end);

    std::unique_ptr<BiDiImpl> impl;
};

} // end namespace mbgl
