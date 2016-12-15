package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;
import android.util.DisplayMetrics;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.exceptions.IconBitmapChangedException;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Responsible for managing icons added to the Map.
 * <p>
 * Maintains a {@link List} of {@link Icon} and  is responsible for initialising default markers and
 * setting up {@link MarkerView} annotation ghosting.
 * </p>
 * <p>
 * Keep track of icons added and the resulting average icon size. This is used internally by our
 * gestures detection to calculate the size of a touch target.
 * </p>
 */
class IconManager {

  private NativeMapView nativeMapView;
  private List<Icon> icons;

  private int averageIconHeight;
  private int averageIconWidth;

  IconManager(NativeMapView nativeMapView) {
    this.nativeMapView = nativeMapView;
    this.icons = new ArrayList<>();
    // load transparent icon for MarkerView to trace actual markers, see #6352
    loadIcon(IconFactory.recreate(IconFactory.ICON_MARKERVIEW_ID, IconFactory.ICON_MARKERVIEW_BITMAP));
  }

  Icon loadIconForMarker(Marker marker) {
    Icon icon = marker.getIcon();

    // calculating average before adding
    int iconSize = icons.size() + 1;

    // TODO replace former if case with anchor implementation,
    // current workaround for having extra pixels is diving height by 2
    if (icon == null) {
      icon = IconFactory.getInstance(nativeMapView.getContext()).defaultMarker();
      Bitmap bitmap = icon.getBitmap();
      averageIconHeight = averageIconHeight + (bitmap.getHeight() / 2 - averageIconHeight) / iconSize;
      averageIconWidth = averageIconWidth + (bitmap.getWidth() - averageIconWidth) / iconSize;
      marker.setIcon(icon);
    } else {
      Bitmap bitmap = icon.getBitmap();
      averageIconHeight = averageIconHeight + (bitmap.getHeight() - averageIconHeight) / iconSize;
      averageIconWidth = averageIconWidth + (bitmap.getWidth() - averageIconWidth) / iconSize;
    }

    if (!icons.contains(icon)) {
      icons.add(icon);
      loadIcon(icon);
    } else {
      Icon oldIcon = icons.get(icons.indexOf(icon));
      if (!oldIcon.getBitmap().sameAs(icon.getBitmap())) {
        throw new IconBitmapChangedException();
      }
    }
    return icon;
  }

  Icon loadIconForMarkerView(MarkerView marker) {
    Icon icon = marker.getIcon();
    int iconSize = icons.size() + 1;
    if (icon == null) {
      icon = IconFactory.getInstance(nativeMapView.getContext()).defaultMarkerView();
      marker.setIcon(icon);
    }
    Bitmap bitmap = icon.getBitmap();
    averageIconHeight = averageIconHeight + (bitmap.getHeight() - averageIconHeight) / iconSize;
    averageIconWidth = averageIconWidth + (bitmap.getWidth() - averageIconWidth) / iconSize;
    if (!icons.contains(icon)) {
      icons.add(icon);
    } else {
      Icon oldIcon = icons.get(icons.indexOf(icon));
      if (!oldIcon.getBitmap().sameAs(icon.getBitmap())) {
        throw new IconBitmapChangedException();
      }
    }
    return icon;
  }

  int getTopOffsetPixelsForIcon(Icon icon) {
    return (int) (nativeMapView.getTopOffsetPixelsForAnnotationSymbol(icon.getId()) * nativeMapView.getPixelRatio());
  }

  void loadIcon(Icon icon) {
    Bitmap bitmap = icon.getBitmap();
    String id = icon.getId();
    if (bitmap.getConfig() != Bitmap.Config.ARGB_8888) {
      bitmap = bitmap.copy(Bitmap.Config.ARGB_8888, false);
    }
    ByteBuffer buffer = ByteBuffer.allocate(bitmap.getRowBytes() * bitmap.getHeight());
    bitmap.copyPixelsToBuffer(buffer);

    float density = bitmap.getDensity();
    if (density == Bitmap.DENSITY_NONE) {
      density = DisplayMetrics.DENSITY_DEFAULT;
    }
    float scale = density / DisplayMetrics.DENSITY_DEFAULT;
    nativeMapView.addAnnotationIcon(
      id,
      bitmap.getWidth(),
      bitmap.getHeight(),
      scale, buffer.array());
  }

  void reloadIcons() {
    int count = icons.size();
    for (int i = 0; i < count; i++) {
      Icon icon = icons.get(i);
      loadIcon(icon);
    }
  }

  void ensureIconLoaded(Marker marker, MapboxMap mapboxMap) {
    Icon icon = marker.getIcon();
    if (icon == null) {
      icon = IconFactory.getInstance(nativeMapView.getContext()).defaultMarker();
      marker.setIcon(icon);
    }
    if (!icons.contains(icon)) {
      icons.add(icon);
      loadIcon(icon);
    } else {
      Icon oldIcon = icons.get(icons.indexOf(icon));
      if (!oldIcon.getBitmap().sameAs(icon.getBitmap())) {
        throw new IconBitmapChangedException();
      }
    }

    // this seems to be a costly operation according to the profiler so I'm trying to save some calls
    Marker previousMarker = marker.getId() != -1 ? (Marker) mapboxMap.getAnnotation(marker.getId()) : null;
    if (previousMarker == null || previousMarker.getIcon() == null || previousMarker.getIcon() != marker.getIcon()) {
      marker.setTopOffsetPixels(getTopOffsetPixelsForIcon(icon));
    }
  }

  int getAverageIconHeight() {
    return averageIconHeight;
  }

  int getAverageIconWidth() {
    return averageIconWidth;
  }
}
