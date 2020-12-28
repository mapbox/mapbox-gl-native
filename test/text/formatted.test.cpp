
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/optional.hpp>

using namespace mbgl;
using namespace mbgl::style::expression;

TEST(Formatted, Equality) {
    Formatted text{"Formatted"};
    auto emptyImage = FormattedSection{style::expression::Image("Formatted")};
    Formatted image{std::vector<FormattedSection>{emptyImage}};
    EXPECT_FALSE(text == image);
    EXPECT_EQ(text, text);
    EXPECT_EQ(image, image);
}

TEST(Formatted, Empty) {
    Formatted emptyFormatted{""};
    EXPECT_TRUE(emptyFormatted.empty());

    auto emptyText = FormattedSection{"", nullopt, nullopt, nullopt};
    auto emptyImage = FormattedSection{style::expression::Image()};
    Formatted multipleEmptySections{std::vector<FormattedSection>{emptyText, emptyText, emptyText}};
    EXPECT_TRUE(multipleEmptySections.empty());

    Formatted multipleEmptySectionsWithImage{std::vector<FormattedSection>{emptyText, emptyImage, emptyText}};
    EXPECT_TRUE(multipleEmptySectionsWithImage.empty());

    auto text = FormattedSection{"Formatted", nullopt, nullopt, nullopt};
    auto image = FormattedSection{style::expression::Image("Image")};

    Formatted multipleSections{std::vector<FormattedSection>{emptyText, text, emptyText}};
    EXPECT_FALSE(multipleSections.empty());

    Formatted multipleSectionsWithImage{std::vector<FormattedSection>{emptyText, image, text}};
    EXPECT_FALSE(multipleSectionsWithImage.empty());
}

TEST(Formatted, ToString) {
    Formatted emptyFormatted{""};
    EXPECT_EQ(emptyFormatted.toString(), "");

    auto text = FormattedSection{"Formatted", nullopt, nullopt, nullopt};
    Formatted multipleSections{std::vector<FormattedSection>{text, text}};
    EXPECT_EQ(multipleSections.toString(), "FormattedFormatted");

    auto image = FormattedSection{style::expression::Image("Image")};
    Formatted multipleEmptySectionsWithImage{std::vector<FormattedSection>{text, image}};
    EXPECT_EQ(multipleEmptySectionsWithImage.toString(), "Formatted");
}
