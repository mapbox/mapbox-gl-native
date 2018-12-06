package com.mapbox.mapboxsdk.location;

import com.mapbox.mapboxsdk.location.modes.CameraMode;

/**
 * Callback for {@link CameraMode } transition state.
 */
public interface OnLocationCameraTransitionListener {
  /**
   * Invoked when the camera mode transition animation has been finished.
   *
   * @param cameraMode camera mode change that initiated the transition
   */
  void onLocationCameraTransitionFinished(@CameraMode.Mode int cameraMode);

  /**
   * Invoked when the camera mode transition animation has been canceled.
   * <p>
   * The camera mode is set regardless of the cancellation of the transition animation.
   *
   * @param cameraMode camera mode change that initiated the transition
   */
  void onLocationCameraTransitionCanceled(@CameraMode.Mode int cameraMode);
}
