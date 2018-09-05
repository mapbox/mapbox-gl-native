package com.mapbox.mapboxsdk.location;

import com.mapbox.mapboxsdk.location.modes.CameraMode;

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
