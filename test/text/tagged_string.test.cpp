
#include <mbgl/test/util.hpp>

#include <mbgl/text/tagged_string.hpp>

using namespace mbgl;
using namespace std::literals;

TEST(TaggedString, Trim) {
    TaggedString basic(u" \t\ntrim that and not this  \n\t", SectionOptions(1.0f, 0));
    basic.trim();
    EXPECT_EQ(basic.rawText(), u"trim that and not this");
    
    TaggedString twoSections;
    twoSections.addSection(u" \t\ntrim that", 1.5f, 1);
    twoSections.addSection(u" and not this  \n\t", 0.5f, 2);
    
    twoSections.trim();
    EXPECT_EQ(twoSections.rawText(), u"trim that and not this");
    
    TaggedString empty(u"\n\t\v \r  \t\n", SectionOptions(1.0f, 0));
    empty.trim();
    EXPECT_EQ(empty.rawText(), u"");
    
    TaggedString noTrim(u"no trim!", SectionOptions(1.0f, 0));
    noTrim.trim();
    EXPECT_EQ(noTrim.rawText(), u"no trim!");
}

TEST(TaggedString, MultipleSections) {
    TaggedString oneSection(u"One section", SectionOptions(1.0f, {}));
    EXPECT_FALSE(oneSection.hasMultipleUniqueSections());

    TaggedString twoSections;
    twoSections.addSection(u"first section", 1.5f, {});
    twoSections.addSection(u"second section", 0.5f, {});
    EXPECT_FALSE(twoSections.hasMultipleUniqueSections());

    TaggedString multipleSections(u"", SectionOptions(1.0f, {}));
    multipleSections.addSection(u"title", 1.5f, {"DIN Offc Pro Bold"s, "Arial Unicode MS Bold"s}, {"header"s});
    multipleSections.addSection(u"content", 1.5f, {"DIN Offc Pro Italic"s, "Arial Unicode MS Regular"s}, {"footer"s});

    EXPECT_TRUE(multipleSections.hasMultipleUniqueSections());
}
