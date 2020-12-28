#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <string>

namespace mbgl {
namespace style {
namespace expression {

extern const char* const kFormattedSectionFontScale;
extern const char* const kFormattedSectionTextFont;
extern const char* const kFormattedSectionTextColor;

struct FormattedSection {
    explicit FormattedSection(std::string text_,
                              optional<double> fontScale_,
                              optional<FontStack> fontStack_,
                              optional<Color> textColor_)
        : text(std::move(text_)),
          fontScale(std::move(fontScale_)),
          fontStack(std::move(fontStack_)),
          textColor(std::move(textColor_)) {}

    explicit FormattedSection(Image image_) : image(std::move(image_)) {}

    std::string text;
    optional<Image> image;
    optional<double> fontScale;
    optional<FontStack> fontStack;
    optional<Color> textColor;
};

class Formatted {
public:
    Formatted() = default;

    Formatted(const char* plainU8String) {
        sections.emplace_back(std::string(plainU8String), nullopt, nullopt, nullopt);
    }
    
    Formatted(std::vector<FormattedSection> sections_)
        : sections(std::move(sections_))
    {}
    
    bool operator==(const Formatted& ) const;
    
    std::string toString() const;
    mbgl::Value toObject() const;

    bool empty() const;

    std::vector<FormattedSection> sections;
};
            
} // namespace expression
    
namespace conversion {

template <>
struct Converter<expression::Formatted> {
public:
    optional<expression::Formatted> operator()(const Convertible& value, Error& error) const;
};

template <>
struct ValueFactory<expression::Formatted> {
    static Value make(const expression::Formatted& formatted) { return formatted.toObject(); }
};

} // namespace conversion
    
} // namespace style
} // namespace mbgl
