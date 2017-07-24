package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.widget.ZoomButtonsController;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

/**
 * The MapZoomButtonController is a ui interface element shown on top of the map.
 * <p>
 * Allows single touch only devices to zoom in and out.
 * </p>
 */
final class MapZoomButtonController {

  private UiSettings uiSettings;
  private ZoomButtonsController zoomButtonsController;

  MapZoomButtonController(@NonNull ZoomButtonsController zoomButtonsController) {
    this.zoomButtonsController = zoomButtonsController;
    this.zoomButtonsController.setZoomSpeed(MapboxConstants.ANIMATION_DURATION);
  }

  void bind(UiSettings uiSettings, ZoomButtonsController.OnZoomListener onZoomListener) {
    this.uiSettings = uiSettings;
    zoomButtonsController.setOnZoomListener(onZoomListener);
  }

  void setVisible(boolean visible) {
    if (uiSettings != null && !uiSettings.isZoomControlsEnabled()) {
      return;
    }
    zoomButtonsController.setVisible(visible);
  }
}
