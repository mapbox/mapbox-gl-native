#include <mbgl/test/util.hpp>

#include <mbgl/text/language_tag.hpp>

using namespace mbgl;

TEST(LanguageTag, Parsing) {
    LanguageTag bareLanguage = LanguageTag::fromBCP47("de");
    EXPECT_EQ(*(bareLanguage.language), "de");
    EXPECT_FALSE(bareLanguage.script);
    EXPECT_FALSE(bareLanguage.region);
    
    LanguageTag languageAndScript = LanguageTag::fromBCP47("de-hans");
    EXPECT_EQ(*(languageAndScript.language), "de");
    EXPECT_EQ(*(languageAndScript.script), "hans");
    EXPECT_FALSE(languageAndScript.region);
    
    LanguageTag languageAndRegion = LanguageTag::fromBCP47("de-US");
    EXPECT_EQ(*(languageAndRegion.language), "de");
    EXPECT_FALSE(languageAndRegion.script);
    EXPECT_EQ(*(languageAndRegion.region), "US");
    
    LanguageTag kitchenSink = LanguageTag::fromBCP47("de-ext-two-Hans-US-variant-x-private");
    EXPECT_EQ(*(kitchenSink.language), "de");
    EXPECT_EQ(*(kitchenSink.script), "Hans");
    EXPECT_EQ(*(kitchenSink.region), "US");
}

TEST(LanguageTag, Generation) {
    LanguageTag deHansUS({"de"}, {"hans"}, {"US"});
    EXPECT_EQ(deHansUS.toBCP47(), "de-hans-US");
    
    LanguageTag deUS({"de"}, {}, {"US"});
    EXPECT_EQ(deUS.toBCP47(), "de-US");
    
    LanguageTag noLanguage({}, {"hans"}, {"US"});
    EXPECT_EQ(noLanguage.toBCP47(), "");
    
    LanguageTag deHans({"de"}, {"hans"}, {});
    EXPECT_EQ(deHans.toBCP47(), "de-hans");
    
    LanguageTag de({"de"}, {}, {});
    EXPECT_EQ(de.toBCP47(), "de");
}

