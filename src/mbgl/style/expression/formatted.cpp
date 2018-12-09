#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/boolean_operator.hpp>

namespace mbgl {
namespace style {
    
namespace expression {

bool Formatted::operator==(const Formatted& other) const {
    if (other.sections.size() != sections.size()) {
        return false;
    }
    for (std::size_t i = 0; i < sections.size(); i++) {
        const auto& thisSection = sections.at(i);
        const auto& otherSection = other.sections.at(i);
        if (thisSection.text != otherSection.text ||
            thisSection.fontScale != otherSection.fontScale ||
            thisSection.fontStack != otherSection.fontStack) {
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
            if (sectionLength > 1) {
                Convertible sectionParams = arrayMember(section, 1);
                if (!isObject(sectionParams)) {
                    error.message = "Parameters have to be enclosed in an object.";
                    return nullopt;
                }

                optional<Convertible> fontScaleMember = objectMember(sectionParams, "font-scale");
                if (fontScaleMember) {
                    fontScale = toDouble(*fontScaleMember);
                }

                optional<Convertible> textFontMember = objectMember(sectionParams, "text-font");
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
            }

            sections.push_back(FormattedSection(*sectionText, fontScale, textFont));
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

