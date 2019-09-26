#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/conversion/constant.hpp>

namespace mbgl {
namespace style {
namespace expression {

const char* const kFormattedSectionFontScale = "font-scale";
const char* const kFormattedSectionTextFont = "text-font";
const char* const kFormattedSectionTextColor = "text-color";

bool Formatted::operator==(const Formatted& other) const {
    if (other.sections.size() != sections.size()) {
        return false;
    }
    for (std::size_t i = 0; i < sections.size(); i++) {
        const auto& thisSection = sections.at(i);
        const auto& otherSection = other.sections.at(i);
        if (thisSection.text != otherSection.text ||
            thisSection.fontScale != otherSection.fontScale ||
            thisSection.fontStack != otherSection.fontStack ||
            thisSection.textColor != otherSection.textColor) {
            return false;
        }
    }
    return true;
}

std::string Formatted::toString() const {
    std::string result;
    for (const auto& section : sections) {
        result += section.text;
    }
    return result;
}

mbgl::Value Formatted::toObject() const {
    mapbox::base::ValueObject result;
    mapbox::base::ValueArray sectionValues;
    sectionValues.reserve(sections.size());
    for (const auto& section : sections) {
        mapbox::base::ValueObject serializedSection;
        serializedSection.emplace("text", section.text);
        if (section.fontScale) {
            serializedSection.emplace("scale", *section.fontScale);
        } else {
            serializedSection.emplace("scale", NullValue());
        }
        if (section.fontStack) {
            std::string fontStackString;
            serializedSection.emplace("fontStack", fontStackToString(*section.fontStack));
        } else {
            serializedSection.emplace("fontStack", NullValue());
        }
        if (section.textColor) {
            serializedSection.emplace("textColor", section.textColor->toObject());
        } else {
            serializedSection.emplace("textColor", NullValue());
        }
        sectionValues.emplace_back(serializedSection);
    }
    result.emplace("sections", std::move(sectionValues));
    return result;
}

} // namespace expression

namespace conversion {
    
using namespace mbgl::style::expression;

optional<Formatted> Converter<Formatted>::operator()(const Convertible& value, Error& error) const {
    using namespace mbgl::style::expression;

    if (isArray(value)) {
        std::vector<FormattedSection> sections;
        for (std::size_t i = 0; i < arrayLength(value); ++i) {
            Convertible section = arrayMember(value, i);
            std::size_t sectionLength = arrayLength(section);
            if (sectionLength < 1) {
                error.message = "Section has to contain a text and optional parameters.";
                return nullopt;
            }

            optional<std::string> sectionText = toString(arrayMember(section, 0));
            if (!sectionText) {
                error.message = "Section has to contain a text.";
                return nullopt;
            }

            optional<double> fontScale;
            optional<FontStack> textFont;
            optional<Color> textColor;
            if (sectionLength > 1) {
                Convertible sectionParams = arrayMember(section, 1);
                if (!isObject(sectionParams)) {
                    error.message = "Parameters have to be enclosed in an object.";
                    return nullopt;
                }

                optional<Convertible> fontScaleMember = objectMember(sectionParams, kFormattedSectionFontScale);
                if (fontScaleMember) {
                    fontScale = toDouble(*fontScaleMember);
                }

                optional<Convertible> textFontMember = objectMember(sectionParams, kFormattedSectionTextFont);
                if (textFontMember) {
                    if (isArray(*textFontMember)) {
                        std::vector<std::string> fontsVector;
                        for (std::size_t j = 0; j < arrayLength(*textFontMember); ++j) {
                            auto font = toString(arrayMember(*textFontMember, j));
                            if (font) {
                                fontsVector.push_back(*font);
                            } else {
                                error.message = "Font has to be a string.";
                                return nullopt;
                            }
                        }
                        textFont = fontsVector;
                    } else {
                        error.message = "Font stack has to be an array.";
                        return nullopt;
                    }
                }

                optional<Convertible> textColorMember = objectMember(sectionParams, kFormattedSectionTextColor);
                if (textColorMember) {
                    textColor = convert<Color>(*textColorMember, error);
                    if (!textColor) {
                        return nullopt;
                    }
                }
            }

            sections.emplace_back(*sectionText, fontScale, textFont, textColor);
        }
        return Formatted(sections);
    } else if (optional<std::string> result = toString(value)) {
        return Formatted(result->c_str());
    } else {
        error.message = "Formatted must be plain string or array type.";
        return nullopt;
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl

