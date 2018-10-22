#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>

#include <vector>
#include <string>

namespace mbgl {
namespace style {
namespace expression {

struct FormattedSection {
    FormattedSection(std::string text_, optional<double> fontScale_, optional<FontStack> fontStack_)
        : text(std::move(text_))
        , fontScale(std::move(fontScale_))
        , fontStack(std::move(fontStack_))
    {}
    std::string text;
    optional<double> fontScale;
    optional<FontStack> fontStack;
};

class Formatted {
public:
    Formatted() = default;
    
    Formatted(const char* plainU8String) {
        sections.emplace_back(std::string(plainU8String), nullopt, nullopt);
    }
    
    Formatted(std::vector<FormattedSection> sections_)
        : sections(std::move(sections_))
    {}
    
    bool operator==(const Formatted& ) const;
    
    std::string toString() const;
    
    bool empty() const {
        return sections.empty() || sections.at(0).text.empty();
    }
     
    std::vector<FormattedSection> sections;
};
            
} // namespace expression
    
namespace conversion {
    
template <>
struct Converter<mbgl::style::expression::Formatted> {
public:
    optional<mbgl::style::expression::Formatted> operator()(const Convertible& value, Error& error) const;
};
    
} // namespace conversion
    
} // namespace style
} // namespace mbgl
