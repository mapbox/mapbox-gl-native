package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.os.Bundle;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;

/**
 * MapFragment utility class.
 * <p>
 * Used to extract duplicate code between {@link com.mapbox.mapboxsdk.maps.MapFragment} and
 * {@link com.mapbox.mapboxsdk.maps.SupportMapFragment}.
 * </p>
 */
public class MapFragmentUtils {

  /**
   * Convert MapboxMapOptions to a bundle of fragment arguments.
   *
   * @param options The MapboxMapOptions to convert
   * @return a bundle of converted fragment arguments
   */
  @NonNull
  public static Bundle createFragmentArgs(MapboxMapOptions options) {
    Bundle bundle = new Bundle();
    bundle.putParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS, options);
    return bundle;
  }

  /**
   * Convert a bundle of fragment arguments to MapboxMapOptions.
   *
   * @param context The context of the activity hosting the fragment
   * @param args    The fragment arguments
   * @return converted MapboxMapOptions
   */
  @Nullable
  public static MapboxMapOptions resolveArgs(@NonNull Context context, @Nullable Bundle args) {
    MapboxMapOptions options;
    if (args != null && args.containsKey(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS)) {
      options = args.getParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS);
    } else {
      // load default options
      options = MapboxMapOptions.createFromAttributes(context);
    }
    return options;
  }
}
