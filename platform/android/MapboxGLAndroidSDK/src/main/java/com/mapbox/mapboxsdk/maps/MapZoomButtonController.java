package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.view.View;
import android.widget.ZoomButtonsController;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

/**
 * The MapZoomButtonController is a ui interface element shown on top of the map.
 * <p>
 * Allows single touch only devices to zoom in and out.
 * </p>
 */
final class MapZoomButtonController extends ZoomButtonsController {

  private UiSettings uiSettings;

  MapZoomButtonController(@NonNull View ownerView, @NonNull UiSettings uiSettings, @NonNull OnZoomListener listener) {
    super(ownerView);
    this.uiSettings = uiSettings;
    setZoomSpeed(MapboxConstants.ANIMATION_DURATION);
    setOnZoomListener(listener);
  }

  @Override
  public void setVisible(boolean visible) {
    if (uiSettings.isZoomControlsEnabled()) {
      super.setVisible(visible);
    }
  }
}
