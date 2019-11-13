
#include <mbgl/test/util.hpp>

#include <mbgl/text/tagged_string.hpp>

using namespace mbgl;

TEST(TaggedString, Trim) {
    TaggedString basic(u" \t\ntrim that and not this  \n\t", SectionOptions(1.0f, {}));
    basic.trim();
    EXPECT_EQ(basic.rawText(), u"trim that and not this");
    
    TaggedString twoSections;
    twoSections.addTextSection(u" \t\ntrim that", 1.5f, {});
    twoSections.addTextSection(u" and not this  \n\t", 0.5f, {});

    twoSections.trim();
    EXPECT_EQ(twoSections.rawText(), u"trim that and not this");
    
    TaggedString empty(u"\n\t\v \r  \t\n", SectionOptions(1.0f, {}));
    empty.trim();
    EXPECT_EQ(empty.rawText(), u"");
    
    TaggedString noTrim(u"no trim!", SectionOptions(1.0f, {}));
    noTrim.trim();
    EXPECT_EQ(noTrim.rawText(), u"no trim!");
}
