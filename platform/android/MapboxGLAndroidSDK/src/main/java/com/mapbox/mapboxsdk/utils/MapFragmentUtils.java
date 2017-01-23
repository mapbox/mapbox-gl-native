package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;

import com.mapbox.mapboxsdk.R;
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

  public static Bundle createFragmentArgs(MapboxMapOptions options) {
    Bundle bundle = new Bundle();
    bundle.putParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS, options);
    return bundle;
  }

  public static MapboxMapOptions resolveArgs(Context context, Bundle args) {
    MapboxMapOptions options;
    if (args != null && args.containsKey(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS)) {
      options = args.getParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS);
    } else {
      // load default options
      options = MapboxMapOptions.createFromAttributes(context, null);
    }
    options = loadDefaultMyLocationViewDrawables(context, options);
    return options;
  }

  private static MapboxMapOptions loadDefaultMyLocationViewDrawables(Context context, MapboxMapOptions options) {
    Drawable foregroundDrawable = options.getMyLocationForegroundDrawable();
    Drawable foregroundBearingDrawable = options.getMyLocationForegroundBearingDrawable();
    if (foregroundDrawable == null || foregroundBearingDrawable == null) {
      if (foregroundDrawable == null) {
        foregroundDrawable = ContextCompat.getDrawable(context, R.drawable.mapbox_mylocation_icon_default);
      }
      if (foregroundBearingDrawable == null) {
        foregroundBearingDrawable = ContextCompat.getDrawable(context, R.drawable.mapbox_mylocation_icon_bearing);
      }
      options.myLocationForegroundDrawables(foregroundDrawable, foregroundBearingDrawable);
    }
    if (options.getMyLocationBackgroundDrawable() == null) {
      options.myLocationBackgroundDrawable(ContextCompat.getDrawable(context, R.drawable.mapbox_mylocation_bg_shape));
    }
    return options;
  }
}
