package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.graphics.PointF;

import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.plugins.locationlayer.modes.CameraMode;

import org.junit.Test;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class LocationLayerCameraTest {

  @Test
  public void setCameraMode_mapTransitionsAreCancelled() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));

    camera.setCameraMode(CameraMode.TRACKING_GPS);

    verify(mapboxMap).cancelTransitions();
  }

  @Test
  public void setCameraMode_gestureThresholdIsAdjusted() {
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    LocationLayerCamera camera = buildCamera(moveGestureDetector);
    LocationLayerOptions options = mock(LocationLayerOptions.class);
    float moveThreshold = 5f;
    when(options.trackingInitialMoveThreshold()).thenReturn(moveThreshold);
    camera.initializeOptions(options);

    camera.setCameraMode(CameraMode.TRACKING_GPS);

    verify(moveGestureDetector).setMoveThreshold(moveThreshold);
  }

  @Test
  public void setCameraMode_gestureThresholdIsResetWhenNotTracking() {
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    LocationLayerCamera camera = buildCamera(moveGestureDetector);
    camera.initializeOptions(mock(LocationLayerOptions.class));

    camera.setCameraMode(CameraMode.NONE);

    verify(moveGestureDetector).setMoveThreshold(0f);
  }

  @Test
  public void setCameraMode_notTrackingAdjustsFocalPoint() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));

    camera.setCameraMode(CameraMode.TRACKING_GPS);
    camera.setCameraMode(CameraMode.NONE);

    verify(mapboxMap.getUiSettings()).setFocalPoint(null);
  }

  @Test
  public void setCameraMode_trackingChangeListenerCameraDismissedIsCalled() {
    OnCameraTrackingChangedListener internalTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    LocationLayerCamera camera = buildCamera(internalTrackingChangedListener);
    camera.initializeOptions(mock(LocationLayerOptions.class));

    camera.setCameraMode(CameraMode.TRACKING_GPS);
    camera.setCameraMode(CameraMode.NONE);

    verify(internalTrackingChangedListener).onCameraTrackingDismissed();
  }

  @Test
  public void setCameraMode_internalCameraTrackingChangeListenerIsCalled() {
    OnCameraTrackingChangedListener internalTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    LocationLayerCamera camera = buildCamera(internalTrackingChangedListener);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    int cameraMode = CameraMode.NONE;

    camera.setCameraMode(cameraMode);

    verify(internalTrackingChangedListener).onCameraTrackingChanged(cameraMode);
  }

  @Test
  public void onNewLatLngValue_cameraModeTrackingUpdatesLatLng() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    when(mapboxMap.getProjection()).thenReturn(mock(Projection.class));
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING);
    LatLng latLng = mock(LatLng.class);

    camera.onNewLatLngValue(latLng);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewLatLngValue_cameraModeTrackingGpsNorthUpdatesLatLng() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    when(mapboxMap.getProjection()).thenReturn(mock(Projection.class));
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_GPS_NORTH);
    LatLng latLng = mock(LatLng.class);

    camera.onNewLatLngValue(latLng);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewLatLngValue_cameraModeTrackingGpsUpdatesLatLng() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    when(mapboxMap.getProjection()).thenReturn(mock(Projection.class));
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_GPS);
    LatLng latLng = mock(LatLng.class);

    camera.onNewLatLngValue(latLng);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewLatLngValue_cameraModeTrackingCompassUpdatesLatLng() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    when(mapboxMap.getProjection()).thenReturn(mock(Projection.class));
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_COMPASS);
    LatLng latLng = mock(LatLng.class);

    camera.onNewLatLngValue(latLng);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewLatLngValue_cameraModeNoneIgnored() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    when(mapboxMap.getProjection()).thenReturn(mock(Projection.class));
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.NONE);
    LatLng latLng = mock(LatLng.class);

    camera.onNewLatLngValue(latLng);

    verify(mapboxMap, times(0)).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewLatLngValue_focalPointIsAdjusted() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    UiSettings uiSettings = mock(UiSettings.class);
    when(mapboxMap.getUiSettings()).thenReturn(uiSettings);
    Projection projection = mock(Projection.class);
    PointF pointF = mock(PointF.class);
    when(projection.toScreenLocation(any(LatLng.class))).thenReturn(pointF);
    when(mapboxMap.getProjection()).thenReturn(projection);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING);
    LatLng latLng = mock(LatLng.class);

    camera.onNewLatLngValue(latLng);

    verify(uiSettings).setFocalPoint(pointF);
  }

  @Test
  public void onNewGpsBearingValue_cameraModeTrackingGpsUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_GPS);
    float gpsBearing = 5f;

    camera.onNewGpsBearingValue(gpsBearing);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewGpsBearingValue_cameraModeNoneGpsUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.NONE_GPS);
    float gpsBearing = 5f;

    camera.onNewGpsBearingValue(gpsBearing);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewGpsBearingValue_cameraModeTrackingNorthUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    CameraPosition cameraPosition = new CameraPosition.Builder().bearing(7d).build();
    when(mapboxMap.getCameraPosition()).thenReturn(cameraPosition);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_GPS_NORTH);
    float gpsBearing = 5f;

    camera.onNewGpsBearingValue(gpsBearing);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewGpsBearingValue_cameraModeTrackingNorthBearingZeroIgnored() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    CameraPosition cameraPosition = new CameraPosition.Builder().bearing(0d).build();
    when(mapboxMap.getCameraPosition()).thenReturn(cameraPosition);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_GPS_NORTH);
    float gpsBearing = 5f;

    camera.onNewGpsBearingValue(gpsBearing);

    verify(mapboxMap, times(0)).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewGpsBearingValue_cameraModeNoneIgnored() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.NONE);
    float gpsBearing = 5f;

    camera.onNewGpsBearingValue(gpsBearing);

    verify(mapboxMap, times(0)).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewCompassBearingValue_cameraModeTrackingCompassUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_COMPASS);
    float compassBearing = 5f;

    camera.onNewCompassBearingValue(compassBearing);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewCompassBearingValue_cameraModeNoneCompassUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.NONE_COMPASS);
    float compassBearing = 5f;

    camera.onNewCompassBearingValue(compassBearing);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewCompassBearingValue_cameraModeNoneIgnored() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.NONE);
    float compassBearing = 5f;

    camera.onNewCompassBearingValue(compassBearing);

    verify(mapboxMap, times(0)).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewZoomValue_cameraIsUpdated() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationLayerCamera camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationLayerOptions.class));
    camera.setCameraMode(CameraMode.TRACKING);
    float zoom = 5f;

    camera.onNewZoomValue(zoom);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  private LocationLayerCamera buildCamera(OnCameraTrackingChangedListener onCameraTrackingChangedListener) {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = mock(OnCameraMoveInvalidateListener.class);
    return new LocationLayerCamera(mapboxMap, moveGestureDetector,
      onCameraTrackingChangedListener, onCameraMoveInvalidateListener);
  }

  private LocationLayerCamera buildCamera(MoveGestureDetector moveGestureDetector) {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    OnCameraTrackingChangedListener onCameraTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = mock(OnCameraMoveInvalidateListener.class);
    return new LocationLayerCamera(mapboxMap, moveGestureDetector,
      onCameraTrackingChangedListener, onCameraMoveInvalidateListener);
  }

  private LocationLayerCamera buildCamera(MapboxMap mapboxMap) {
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    OnCameraTrackingChangedListener onCameraTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = mock(OnCameraMoveInvalidateListener.class);
    return new LocationLayerCamera(mapboxMap, moveGestureDetector,
      onCameraTrackingChangedListener, onCameraMoveInvalidateListener);
  }
}
