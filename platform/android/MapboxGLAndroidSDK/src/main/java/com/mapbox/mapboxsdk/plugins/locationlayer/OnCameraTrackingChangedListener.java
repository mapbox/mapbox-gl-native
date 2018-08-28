package com.mapbox.mapboxsdk.plugins.locationlayer;

import com.mapbox.mapboxsdk.plugins.locationlayer.modes.CameraMode;

/**
 * Listener that gets invoked when camera tracking state changes.
 */
public interface OnCameraTrackingChangedListener {
  /**
   * Invoked whenever camera tracking is broken.
   * This callback gets invoked just after {@link #onCameraTrackingChanged(int)}, if needed.
   */
  void onCameraTrackingDismissed();

  /**
   * Invoked on every {@link CameraMode} change.
   *
   * @param currentMode current active {@link CameraMode}.
   */
  void onCameraTrackingChanged(@CameraMode.Mode int currentMode);
}
