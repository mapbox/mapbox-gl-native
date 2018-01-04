package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

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

  private final Map<Icon, Integer> iconMap = new HashMap<>();

  private NativeMapView nativeMapView;
  private int highestIconWidth;
  private int highestIconHeight;

  IconManager(NativeMapView nativeMapView) {
    this.nativeMapView = nativeMapView;
    // load transparent icon for MarkerView to trace actual markers, see #6352
    loadIcon(IconFactory.recreate(IconFactory.ICON_MARKERVIEW_ID, IconFactory.ICON_MARKERVIEW_BITMAP));
  }

  Icon loadIconForMarker(Marker marker) {
    Icon icon = marker.getIcon();
    if (icon == null) {
      // TODO replace with anchor implementation, we are faking an anchor by adding extra pixels and diving height by 2
      // TODO we can move this code afterwards to getIcon as with MarkerView.getIcon
      icon = loadDefaultIconForMarker(marker);
    } else {
      updateHighestIconSize(icon);
    }
    addIcon(icon);
    return icon;
  }

  void loadIconForMarkerView(MarkerView marker) {
    Icon icon = marker.getIcon();
    Bitmap bitmap = icon.getBitmap();
    updateHighestIconSize(bitmap);
    addIcon(icon, false);
  }

  int getTopOffsetPixelsForIcon(Icon icon) {
    return (int) (nativeMapView.getTopOffsetPixelsForAnnotationSymbol(icon.getId()) * nativeMapView.getPixelRatio());
  }

  int getHighestIconWidth() {
    return highestIconWidth;
  }

  int getHighestIconHeight() {
    return highestIconHeight;
  }

  private Icon loadDefaultIconForMarker(Marker marker) {
    Icon icon = IconFactory.getInstance(Mapbox.getApplicationContext()).defaultMarker();
    Bitmap bitmap = icon.getBitmap();
    updateHighestIconSize(bitmap.getWidth(), bitmap.getHeight() / 2);
    marker.setIcon(icon);
    return icon;
  }

  private void addIcon(Icon icon) {
    addIcon(icon, true);
  }

  private void addIcon(Icon icon, boolean addIconToMap) {
    if (!iconMap.keySet().contains(icon)) {
      iconMap.put(icon, 1);
      if (addIconToMap) {
        loadIcon(icon);
      }
    } else {
      iconMap.put(icon, iconMap.get(icon) + 1);
    }
  }

  private void updateHighestIconSize(Icon icon) {
    updateHighestIconSize(icon.getBitmap());
  }

  private void updateHighestIconSize(Bitmap bitmap) {
    updateHighestIconSize(bitmap.getWidth(), bitmap.getHeight());
  }

  private void updateHighestIconSize(int width, int height) {
    if (width > highestIconWidth) {
      highestIconWidth = width;
    }

    if (height > highestIconHeight) {
      highestIconHeight = height;
    }
  }

  private void loadIcon(Icon icon) {
    Bitmap bitmap = icon.getBitmap();
    nativeMapView.addAnnotationIcon(icon.getId(),
      bitmap.getWidth(),
      bitmap.getHeight(),
      icon.getScale(),
      icon.toBytes());
  }

  void reloadIcons() {
    for (Icon icon : iconMap.keySet()) {
      loadIcon(icon);
    }
  }

  void ensureIconLoaded(Marker marker, MapboxMap mapboxMap) {
    Icon icon = marker.getIcon();
    if (icon == null) {
      icon = loadDefaultIconForMarker(marker);
    }
    addIcon(icon);
    setTopOffsetPixels(marker, mapboxMap, icon);
  }

  private void setTopOffsetPixels(Marker marker, MapboxMap mapboxMap, Icon icon) {
    // this seems to be a costly operation according to the profiler so I'm trying to save some calls
    Marker previousMarker = marker.getId() != -1 ? (Marker) mapboxMap.getAnnotation(marker.getId()) : null;
    if (previousMarker == null || previousMarker.getIcon() == null || previousMarker.getIcon() != marker.getIcon()) {
      marker.setTopOffsetPixels(getTopOffsetPixelsForIcon(icon));
    }
  }

  void iconCleanup(Icon icon) {
    Integer refCounter = iconMap.get(icon);
    if (refCounter != null) {
      refCounter--;
      if (refCounter == 0) {
        remove(icon);
      } else {
        updateIconRefCounter(icon, refCounter);
      }
    }
  }

  private void remove(Icon icon) {
    nativeMapView.removeAnnotationIcon(icon.getId());
    iconMap.remove(icon);
  }

  private void updateIconRefCounter(Icon icon, int refCounter) {
    iconMap.put(icon, refCounter);
  }

}
