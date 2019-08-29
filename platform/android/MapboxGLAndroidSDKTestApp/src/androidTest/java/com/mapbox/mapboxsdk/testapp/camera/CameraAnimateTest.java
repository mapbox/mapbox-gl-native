package com.mapbox.mapboxsdk.testapp.camera;

import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.maps.MapboxMap;

public class CameraAnimateTest extends CameraTest {
  @Override
  void executeCameraMovement(CameraUpdate cameraUpdate, MapboxMap.CancelableCallback callback) {
    mapboxMap.animateCamera(cameraUpdate, callback);
  }
}