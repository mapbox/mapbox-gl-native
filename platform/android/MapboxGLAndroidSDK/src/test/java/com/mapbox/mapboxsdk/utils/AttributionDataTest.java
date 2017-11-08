package com.mapbox.mapboxsdk.utils;

import com.mapbox.mapboxsdk.maps.Attribution;
import org.junit.Test;

import java.util.List;

import static junit.framework.Assert.assertEquals;

public class AttributionDataTest {

  private final static String SINGLE_ATTRIBUTION = "<a href=\"https://www.digitalglobe.com/\" target=\"_blank\">&copy; DigitalGlobe</a>";


  @Test
  public void testParseAttributionString() throws Exception {
    List<Attribution> attributionList = AttributionUtils.parseAttribution(SINGLE_ATTRIBUTION);
    assertEquals("Size of list should match", 1, attributionList.size());
    Attribution attribution = attributionList.get(0);
    assertEquals("URL should match", "https://www.digitalglobe.com/", attribution.getUrl());
    assertEquals("Title should match", "&copy; DigitalGlobe", attribution.getTitle());
  }
}
