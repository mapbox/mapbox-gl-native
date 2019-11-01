package com.mapbox.mapboxsdk.testapp.annotations;

import android.app.Activity;
import android.support.test.annotation.UiThreadTest;
import android.support.v4.content.res.ResourcesCompat;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.IconManagerResolver;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.EspressoTest;
import com.mapbox.mapboxsdk.testapp.utils.IconUtils;
import org.junit.Before;
import org.junit.Test;

import java.util.Map;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertNull;
import static junit.framework.Assert.assertTrue;

/**
 * Tests integration between Icons and Markers
 */
public class IconTest extends EspressoTest {

  private Map<Icon, Integer> iconMap;

  @Before
  public void beforeTest() {
    super.beforeTest();
    iconMap = new IconManagerResolver(mapboxMap).getIconMap();
  }

  @Test
  @UiThreadTest
  public void testAddSameIconMarker() {
    validateTestSetup();
    Icon defaultMarker = IconFactory.getInstance(rule.getActivity()).defaultMarker();
    mapboxMap.addMarker(new MarkerOptions().position(new LatLng()));
    mapboxMap.addMarker(new MarkerOptions().position(new LatLng(1, 1)));
    assertEquals(1, iconMap.size());
    assertEquals(2, iconMap.get(defaultMarker), 0);
  }

  @Test
  @UiThreadTest
  public void testAddDifferentIconMarker() {
    validateTestSetup();
    Icon icon = IconFactory.getInstance(rule.getActivity()).fromResource(R.drawable.mapbox_logo_icon);
    mapboxMap.addMarker(new MarkerOptions().icon(icon).position(new LatLng()));
    mapboxMap.addMarker(new MarkerOptions().position(new LatLng(1, 1)));
    assertEquals(iconMap.size(), 2);
    assertTrue(iconMap.containsKey(icon));
    assertTrue(iconMap.get(icon) == 1);
  }

  @Test
  @UiThreadTest
  public void testAddRemoveIconMarker() {
    validateTestSetup();
    Icon icon = IconFactory.getInstance(rule.getActivity()).fromResource(R.drawable.mapbox_logo_icon);
    Marker marker = mapboxMap.addMarker(new MarkerOptions().icon(icon).position(new LatLng()));
    mapboxMap.addMarker(new MarkerOptions().position(new LatLng(1, 1)));
    assertEquals(iconMap.size(), 2);
    assertTrue(iconMap.containsKey(icon));
    assertTrue(iconMap.get(icon) == 1);

    mapboxMap.removeMarker(marker);
    assertEquals(iconMap.size(), 1);
    assertFalse(iconMap.containsKey(icon));
  }

  @Test
  @UiThreadTest
  public void testAddRemoveDefaultMarker() {
    validateTestSetup();
    Marker marker = mapboxMap.addMarker(new MarkerOptions().position(new LatLng(1, 1)));
    assertEquals(iconMap.size(), 1);

    mapboxMap.removeMarker(marker);
    assertEquals(iconMap.size(), 0);

    mapboxMap.addMarker(new MarkerOptions().position(new LatLng()));
    assertEquals(iconMap.size(), 1);
  }

  @Test
  @UiThreadTest
  public void testAddRemoveMany() {
    validateTestSetup();
    Activity activity = rule.getActivity();
    IconFactory iconFactory = IconFactory.getInstance(activity);

    // add 2 default icon markers
    Marker defaultMarkerOne = mapboxMap.addMarker(new MarkerOptions().position(new LatLng(1, 1)));
    Marker defaultMarkerTwo = mapboxMap.addMarker(new MarkerOptions().position(new LatLng(2, 1)));

    // add 4 unique icon markers
    mapboxMap.addMarker(new MarkerOptions()
      .icon(iconFactory.fromResource(R.drawable.mapbox_logo_icon))
      .position(new LatLng(3, 1))
    );
    mapboxMap.addMarker(new MarkerOptions()
      .icon(iconFactory.fromResource(R.drawable.mapbox_compass_icon))
      .position(new LatLng(4, 1))
    );
    mapboxMap.addMarker(new MarkerOptions()
      .icon(IconUtils.drawableToIcon(activity, R.drawable.ic_stars,
        ResourcesCompat.getColor(activity.getResources(),
          R.color.blueAccent, activity.getTheme())))
      .position(new LatLng(5, 1))
    );
    mapboxMap.addMarker(new MarkerOptions()
      .icon(iconFactory.fromResource(R.drawable.ic_android))
      .position(new LatLng(6, 1))
    );

    assertEquals("Amount of icons should match 5", 5, iconMap.size());
    assertEquals("Refcounter of default marker should match 2", 2, iconMap.get(iconFactory.defaultMarker()), 0);

    mapboxMap.removeMarker(defaultMarkerOne);

    assertEquals("Amount of icons should match 5", 5, iconMap.size());
    assertEquals("Refcounter of default marker should match 1", 1, iconMap.get(iconFactory.defaultMarker()), 0);

    mapboxMap.removeMarker(defaultMarkerTwo);

    assertEquals("Amount of icons should match 4", 4, iconMap.size());
    assertNull("DefaultMarker shouldn't exist anymore", iconMap.get(iconFactory.defaultMarker()));

    mapboxMap.clear();
    assertEquals("Amount of icons should match 0", 0, iconMap.size());
  }
}
