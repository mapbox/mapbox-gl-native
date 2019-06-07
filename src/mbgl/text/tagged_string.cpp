#include <mbgl/text/tagged_string.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/util/i18n.hpp>

namespace mbgl {
    
void TaggedString::addSection(const std::u16string& sectionText, double scale, FontStack fontStack, optional<Color> textColor) {
    styledText.first += sectionText;
    sections.emplace_back(scale, fontStack, std::move(textColor));
    styledText.second.resize(styledText.first.size(), sections.size() - 1);
    supportsVerticalWritingMode = nullopt;
}

void TaggedString::trim() {
    std::size_t beginningWhitespace = styledText.first.find_first_not_of(u" \t\n\v\f\r");
    if (beginningWhitespace == std::u16string::npos) {
        // Entirely whitespace
        styledText.first.clear();
        styledText.second.clear();
    } else {
        std::size_t trailingWhitespace = styledText.first.find_last_not_of(u" \t\n\v\f\r") + 1;

        styledText.first = styledText.first.substr(beginningWhitespace, trailingWhitespace - beginningWhitespace);
        styledText.second = std::vector<uint8_t>(styledText.second.begin() + beginningWhitespace, styledText.second.begin() + trailingWhitespace);
    }
}

double TaggedString::getMaxScale() const {
    double maxScale = 0.0;
    for (std::size_t i = 0; i < styledText.first.length(); i++) {
        maxScale = util::max(maxScale, getSection(i).scale);
    }
    return maxScale;
}
    
void TaggedString::verticalizePunctuation() {
    // Relies on verticalization changing characters in place so that style indices don't need updating
    styledText.first = util::i18n::verticalizePunctuation(styledText.first);
}

bool TaggedString::allowsVerticalWritingMode() {
    if (!supportsVerticalWritingMode) {
        supportsVerticalWritingMode = util::i18n::allowsVerticalWritingMode(rawText());
    }
    return *supportsVerticalWritingMode;
}

} // namespace mbgl
