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

  MapZoomButtonController(@NonNull View ownerView, @NonNull UiSettings uiSettings, @NonNull Transform transform) {
    super(ownerView);
    this.uiSettings = uiSettings;
    setZoomSpeed(MapboxConstants.ANIMATION_DURATION);
    setOnZoomListener(new OnZoomListener(uiSettings, transform));
  }

  @Override
  public void setVisible(boolean visible) {
    if (uiSettings.isZoomControlsEnabled()) {
      super.setVisible(visible);
    }
  }

  // Zoom controls allow single touch only devices to zoom in and out
  private static class OnZoomListener implements ZoomButtonsController.OnZoomListener {

    private final UiSettings uiSettings;
    private final Transform transform;

    OnZoomListener(UiSettings uiSettings, Transform transform) {
      this.uiSettings = uiSettings;
      this.transform = transform;
    }

    // Not used
    @Override
    public void onVisibilityChanged(boolean visible) {
      // Ignore
    }

    // Called when user pushes a zoom button
    @Override
    public void onZoom(boolean zoomIn) {
      if (!uiSettings.isZoomGesturesEnabled()) {
        return;
      }
      transform.zoom(zoomIn);
    }
  }
}
