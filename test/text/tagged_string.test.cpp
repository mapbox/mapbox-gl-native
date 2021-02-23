
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

TEST(TaggedString, ImageSections) {
    TaggedString string;
    string.addImageSection("image_name");
    EXPECT_EQ(string.rawText(), u"\uE000");
    EXPECT_TRUE(string.getSection(0).imageID);
    EXPECT_EQ(*string.getSection(0).imageID, "image_name");

    TaggedString maxSections;
    for (std::size_t i = 0; i < 6401; ++i) {
        maxSections.addImageSection(util::toString(i));
    }

    EXPECT_EQ(maxSections.getSections().size(), 6400u);
    EXPECT_EQ(maxSections.getCharCodeAt(0), u'\uE000');
    EXPECT_EQ(maxSections.getCharCodeAt(6399), u'\uF8FF');
}