package com.mapbox.mapboxsdk.testapp.activity.location;

import android.location.Location;

import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;

import java.util.Random;

import timber.log.Timber;

/**
 * Useful utilities used throughout the testapp.
 */
public class Utils {

  private static final String[] STYLES = new String[] {
    Style.MAPBOX_STREETS,
    Style.OUTDOORS,
    Style.LIGHT,
    Style.DARK,
    Style.SATELLITE_STREETS
  };

  private static int index;

  /**
   * Utility to cycle through map styles. Useful to test if runtime styling source and layers transfer over to new
   * style.
   *
   * @return a string ID representing the map style
   */
  public static String getNextStyle() {
    index++;
    if (index == STYLES.length) {
      index = 0;
    }
    return STYLES[index];
  }

  /**
   * Utility for getting a random coordinate inside a provided bounds and creates a {@link Location} from it.
   *
   * @param bounds bounds of the generated location
   * @return a {@link Location} object using the random coordinate
   */
  public static Location getRandomLocation(LatLngBounds bounds) {
    Random random = new Random();

    double randomLat = bounds.getLatSouth() + (bounds.getLatNorth() - bounds.getLatSouth()) * random.nextDouble();
    double randomLon = bounds.getLonWest() + (bounds.getLonEast() - bounds.getLonWest()) * random.nextDouble();

    Location location = new Location("random-loc");
    location.setLongitude(randomLon);
    location.setLatitude(randomLat);
    Timber.d("getRandomLatLng: %s", location.toString());
    return location;
  }
}