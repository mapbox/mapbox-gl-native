package com.mapbox.mapboxsdk.utils;

import android.support.test.runner.AndroidJUnit4;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

@RunWith(AndroidJUnit4.class)
public class FontUtilsTest {

  @Test
  public void testExtractedFontShouldMatchDefault() {
    String[] fonts = new String[] {"foo", "bar"};
    String actual = FontUtils.extractValidFont(fonts);
    assertEquals("Selected font should match", MapboxConstants.DEFAULT_FONT, actual);
  }

  @Test
  public void testExtractedFontShouldMatchMonospace() {
    String expected = "monospace";
    String[] fonts = new String[] {"foo", expected};
    String actual = FontUtils.extractValidFont(fonts);
    assertEquals("Selected font should match", expected, actual);
  }

  @Test
  public void testExtractedFontArrayShouldBeNull() {
    String[] fonts = null;
    String actual = FontUtils.extractValidFont(fonts);
    assertNull(actual);
  }

  @Test
  public void testExtractedFontShouldBeNull() {
    String actual = FontUtils.extractValidFont(null);
    assertNull(actual);
  }
}
