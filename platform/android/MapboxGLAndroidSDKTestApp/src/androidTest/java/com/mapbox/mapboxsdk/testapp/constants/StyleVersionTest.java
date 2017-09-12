package com.mapbox.mapboxsdk.testapp.constants;

import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;

import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 * Verifies if Style.java defined constants match the strings.xml equivalent
 */
public class StyleVersionTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  public void mapboxStreets() {
    assertEquals(
      "Mapbox Streets should match",
      Style.MAPBOX_STREETS,
      getString(R.string.mapbox_style_mapbox_streets)
    );
  }

  @Test
  public void satelliteStreets() {
    assertEquals(
      "Satellite Streets should match",
      Style.SATELLITE_STREETS,
      getString(R.string.mapbox_style_satellite_streets)
    );
  }

  @Test
  public void outdoors() {
    assertEquals(
      "Outdoors should match",
      Style.OUTDOORS,
      getString(R.string.mapbox_style_outdoors)
    );
  }

  @Test
  public void satellite() {
    assertEquals(
      "Satellite should match",
      Style.SATELLITE,
      getString(R.string.mapbox_style_satellite)
    );
  }

  @Test
  public void light() {
    assertEquals(
      "Light should match",
      Style.LIGHT,
      getString(R.string.mapbox_style_light)
    );
  }

  @Test
  public void dark() {
    assertEquals(
      "Dark should match",
      Style.DARK,
      getString(R.string.mapbox_style_dark)
    );
  }

  @Test
  public void trafficDay() {
    assertEquals(
      "Traffic Day should match",
      Style.TRAFFIC_DAY,
      getString(R.string.mapbox_style_traffic_day)
    );
  }

  @Test
  public void trafficNight() {
    assertEquals(
      "Traffic Night should match",
      Style.TRAFFIC_NIGHT,
      getString(R.string.mapbox_style_traffic_night)
    );
  }

  @Test
  public void navigationGuidanceDay() {
    assertEquals(
      "Navigation Guidance Day should match",
      Style.NAVIGATION_GUIDANCE_DAY,
      getString(R.string.mapbox_style_navigation_guidance_day)
    );
  }

  @Test
  public void navigationGuidanceNight() {
    assertEquals(
      "Navigation Guidance Night should match",
      Style.NAVIGATION_GUIDANCE_NIGHT,
      getString(R.string.mapbox_style_navigation_guidance_night)
    );
  }

  @Test
  public void navigationPreviewDay() {
    assertEquals(
      "Navigation Preview Day should match",
      Style.NAVIGATION_PREVIEW_DAY,
      getString(R.string.mapbox_style_navigation_preview_day)
    );
  }

  @Test
  public void navigationPreviewNight() {
    assertEquals(
      "Navigation Preview Night should match",
      Style.NAVIGATION_PREVIEW_NIGHT,
      getString(R.string.mapbox_style_navigation_preview_night)
    );
  }
}
