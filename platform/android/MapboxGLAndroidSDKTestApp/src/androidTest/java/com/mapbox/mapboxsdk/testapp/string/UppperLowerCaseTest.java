package com.mapbox.mapboxsdk.testapp.string;

import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import org.junit.Test;

import static junit.framework.Assert.assertEquals;

/**
 * Test verifying if String#toUpperCase and String#toLowerCase produces desired results
 * <p>
 * See core test in https://github.com/mapbox/mapbox-gl-native/blob/master/test/util/text_conversions.test.cpp
 * </p>
 */
public class UppperLowerCaseTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  public void testToUpperCase() {
    assertEquals("STREET", "strEEt".toUpperCase());  // EN
    assertEquals("ROAD", "rOAd".toUpperCase());      // EN

    assertEquals("STRASSE", "straße".toUpperCase()); // DE
    assertEquals("MASSE", "maße".toUpperCase());     // DE
    assertEquals("WEISSKOPFSEEADLER", "weißkopfseeadler".toUpperCase()); // DE

    assertEquals("BÊNÇÃO", "bênção".toUpperCase()); // PT
    assertEquals("AZƏRBAYCAN", "Azərbaycan".toUpperCase()); // AZ
    assertEquals("ὈΔΥΣΣΕΎΣ", "Ὀδυσσεύς".toUpperCase()); // GR
  }

  @Test
  public void testToLowerCase() {
    assertEquals("street", "strEEt".toLowerCase());  // EN
    assertEquals("road", "rOAd".toLowerCase());      // EN

    assertEquals("straße", "Straße".toLowerCase());   // DE
    assertEquals("strasse", "STRASSE".toLowerCase()); // DE
    assertEquals("masse", "MASSE".toLowerCase());     // DE
    assertEquals("weisskopfseeadler", "weiSSkopfseeadler".toLowerCase()); // DE

    assertEquals("bênção", "BÊNÇÃO".toLowerCase()); // PT
    assertEquals("azərbaycan", "AZƏRBAYCAN".toLowerCase()); //
  }

}
