#pragma once

#include <mbgl/text/bidi.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/util/font_stack.hpp>

namespace mbgl {

struct SectionOptions {
    SectionOptions(double scale_, FontStack fontStack_, optional<Color> textColor_ = nullopt)
        : scale(scale_),
          fontStackHash(FontStackHasher()(fontStack_)),
          fontStack(std::move(fontStack_)),
          textColor(std::move(textColor_))
    {}

    explicit SectionOptions(std::string imageID_) : scale(1.0), imageID(std::move(imageID_)) {}

    double scale;
    FontStackHash fontStackHash;
    FontStack fontStack;
    optional<Color> textColor;
    optional<std::string> imageID;
};

/**
 * A TaggedString is the shaping-code counterpart of the Formatted type
 * Whereas Formatted matches the logical structure of a 'format' expression,
 * a TaggedString represents the same data at a per-character level so that
 * character-rearranging operations (e.g. BiDi) preserve formatting.
 * Text is represented as:
 * - A string of characters
 * - A matching array of indices, pointing to:
 * - An array of SectionsOptions, representing the evaluated formatting
 *    options of the original sections.
 *
 * Once the guts of a TaggedString have been re-arranged by BiDi, you can
 * iterate over the contents in order, using getCharCodeAt and getSection
 * to get the formatting options for each character in turn.
 */
struct TaggedString {
    TaggedString() = default;

    TaggedString(std::u16string text_, SectionOptions options)
        : styledText(std::move(text_),
                     std::vector<uint8_t>(text_.size(), 0)) {
        sections.push_back(std::move(options));
    }
    
    TaggedString(StyledText styledText_, std::vector<SectionOptions> sections_)
        : styledText(std::move(styledText_))
        , sections(std::move(sections_)) {
    }

    std::size_t length() const {
        return styledText.first.length();
    }
    
    std::size_t sectionCount() const {
        return sections.size();
    }
    
    bool empty() const {
        return styledText.first.empty();
    }
    
    const SectionOptions& getSection(std::size_t index) const {
        return sections.at(styledText.second.at(index));
    }
    
    char16_t getCharCodeAt(std::size_t index) const {
        return styledText.first[index];
    }
    
    const std::u16string& rawText() const {
        return styledText.first;
    }
    
    const StyledText& getStyledText() const {
        return styledText;
    }

    void addTextSection(const std::u16string& text,
                        double scale,
                        FontStack fontStack,
                        optional<Color> textColor_ = nullopt);

    void addImageSection(const std::string& imageID);

    const SectionOptions& sectionAt(std::size_t index) const {
        return sections.at(index);
    }
    
    const std::vector<SectionOptions>& getSections() const {
        return sections;
    }
    
    uint8_t getSectionIndex(std::size_t characterIndex) const {
        return styledText.second.at(characterIndex);
    }
    
    double getMaxScale() const;
    void trim();
    
    void verticalizePunctuation();
    bool allowsVerticalWritingMode();

private:
    optional<char16_t> getNextImageSectionCharCode();

private:
    StyledText styledText;
    std::vector<SectionOptions> sections;
    optional<bool> supportsVerticalWritingMode;
    // Max number of images within a text is 6400 U+E000–U+F8FF
    // that covers Basic Multilingual Plane Unicode Private Use Area (PUA).
    char16_t imageSectionID = 0u;
};

} // namespace mbgl
