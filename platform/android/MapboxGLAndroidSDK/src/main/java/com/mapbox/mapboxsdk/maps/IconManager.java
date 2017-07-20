package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.exceptions.IconBitmapChangedException;

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

  private int highestIconWidth;
  private int highestIconHeight;

  IconManager(NativeMapView nativeMapView) {
    this.nativeMapView = nativeMapView;
    this.icons = new ArrayList<>();
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
    if (!icons.contains(icon)) {
      icons.add(icon);
      if (addIconToMap) {
        loadIcon(icon);
      }
    } else {
      validateIconChanged(icon);
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
    for (Icon icon : icons) {
      loadIcon(icon);
    }
  }

  private void validateIconChanged(Icon icon) {
    Icon oldIcon = icons.get(icons.indexOf(icon));
    if (!oldIcon.getBitmap().sameAs(icon.getBitmap())) {
      throw new IconBitmapChangedException();
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
}
