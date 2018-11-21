package com.mapbox.mapboxsdk.location;

import android.graphics.PointF;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.modes.CameraMode;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.maps.UiSettings;

import org.junit.Test;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class LocationCameraControllerTest {

  @Test
  public void setCameraMode_mapTransitionsAreCancelled() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));

    camera.setCameraMode(CameraMode.TRACKING_GPS);

    verify(mapboxMap).cancelTransitions();
  }

  @Test
  public void setCameraMode_gestureThresholdIsAdjusted() {
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    LocationCameraController camera = buildCamera(moveGestureDetector);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    float moveThreshold = 5f;
    when(options.trackingInitialMoveThreshold()).thenReturn(moveThreshold);
    when(options.trackingGesturesManagement()).thenReturn(true);
    camera.initializeOptions(options);

    camera.setCameraMode(CameraMode.TRACKING_GPS);

    verify(moveGestureDetector).setMoveThreshold(moveThreshold);
  }

  @Test
  public void setCameraMode_gestureThresholdNotAdjustedWhenDisabled() {
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    LocationCameraController camera = buildCamera(moveGestureDetector);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    float moveThreshold = 5f;
    when(options.trackingInitialMoveThreshold()).thenReturn(moveThreshold);
    when(options.trackingGesturesManagement()).thenReturn(false);
    camera.initializeOptions(options);

    camera.setCameraMode(CameraMode.TRACKING_GPS);

    verify(moveGestureDetector, times(0)).setMoveThreshold(moveThreshold);
    verify(moveGestureDetector, times(0)).setMoveThreshold(0f);
  }

  @Test
  public void setCameraMode_gestureThresholdIsResetWhenNotTracking() {
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    LocationCameraController camera = buildCamera(moveGestureDetector);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    when(options.trackingGesturesManagement()).thenReturn(true);
    camera.initializeOptions(options);

    camera.setCameraMode(CameraMode.NONE);

    verify(moveGestureDetector, times(2)).setMoveThreshold(0f); // one for initialization
  }

  @Test
  public void setCameraMode_notTrackingAdjustsFocalPoint() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));

    camera.setCameraMode(CameraMode.TRACKING_GPS);
    camera.setCameraMode(CameraMode.NONE);

    verify(mapboxMap.getUiSettings()).setFocalPoint(null);
  }

  @Test
  public void setCameraMode_trackingChangeListenerCameraDismissedIsCalled() {
    OnCameraTrackingChangedListener internalTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    LocationCameraController camera = buildCamera(internalTrackingChangedListener);
    camera.initializeOptions(mock(LocationComponentOptions.class));

    camera.setCameraMode(CameraMode.TRACKING_GPS);
    camera.setCameraMode(CameraMode.NONE);

    verify(internalTrackingChangedListener).onCameraTrackingDismissed();
  }

  @Test
  public void setCameraMode_internalCameraTrackingChangeListenerIsCalled() {
    OnCameraTrackingChangedListener internalTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    LocationCameraController camera = buildCamera(internalTrackingChangedListener);
    camera.initializeOptions(mock(LocationComponentOptions.class));
    int cameraMode = CameraMode.NONE;

    camera.setCameraMode(cameraMode);

    verify(internalTrackingChangedListener).onCameraTrackingChanged(cameraMode);
  }

  @Test
  public void onNewLatLngValue_cameraModeTrackingUpdatesLatLng() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    when(mapboxMap.getProjection()).thenReturn(mock(Projection.class));
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
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
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
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
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
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
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
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
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
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
    LocationCameraController camera = buildCamera(mapboxMap);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    when(options.trackingGesturesManagement()).thenReturn(true);
    camera.initializeOptions(options);
    camera.setCameraMode(CameraMode.TRACKING);
    LatLng latLng = mock(LatLng.class);

    camera.onNewLatLngValue(latLng);

    verify(uiSettings).setFocalPoint(pointF);
  }

  @Test
  public void onNewGpsBearingValue_cameraModeTrackingGpsUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_GPS);
    float gpsBearing = 5f;

    camera.onNewGpsBearingValue(gpsBearing);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewGpsBearingValue_cameraModeNoneGpsUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
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
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
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
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_GPS_NORTH);
    float gpsBearing = 5f;

    camera.onNewGpsBearingValue(gpsBearing);

    verify(mapboxMap, times(0)).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewGpsBearingValue_cameraModeNoneIgnored() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
    camera.setCameraMode(CameraMode.NONE);
    float gpsBearing = 5f;

    camera.onNewGpsBearingValue(gpsBearing);

    verify(mapboxMap, times(0)).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewCompassBearingValue_cameraModeTrackingCompassUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
    camera.setCameraMode(CameraMode.TRACKING_COMPASS);
    float compassBearing = 5f;

    camera.onNewCompassBearingValue(compassBearing);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewCompassBearingValue_cameraModeNoneCompassUpdatesBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
    camera.setCameraMode(CameraMode.NONE_COMPASS);
    float compassBearing = 5f;

    camera.onNewCompassBearingValue(compassBearing);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewCompassBearingValue_cameraModeNoneIgnored() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
    camera.setCameraMode(CameraMode.NONE);
    float compassBearing = 5f;

    camera.onNewCompassBearingValue(compassBearing);

    verify(mapboxMap, times(0)).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void onNewZoomValue_cameraIsUpdated() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));
    camera.setCameraMode(CameraMode.TRACKING);
    float zoom = 5f;

    camera.onNewZoomValue(zoom);

    verify(mapboxMap).moveCamera(any(CameraUpdate.class));
  }

  @Test
  public void gesturesManagement_enabled() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    AndroidGesturesManager initialGesturesManager = mock(AndroidGesturesManager.class);
    AndroidGesturesManager internalGesturesManager = mock(AndroidGesturesManager.class);
    LocationCameraController camera = buildCamera(mapboxMap, initialGesturesManager, internalGesturesManager);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    when(options.trackingGesturesManagement()).thenReturn(true);
    camera.initializeOptions(options);

    verify(mapboxMap).setGesturesManager(internalGesturesManager, true, true);
  }

  @Test
  public void gesturesManagement_disabled() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    AndroidGesturesManager initialGesturesManager = mock(AndroidGesturesManager.class);
    AndroidGesturesManager internalGesturesManager = mock(AndroidGesturesManager.class);
    LocationCameraController camera = buildCamera(mapboxMap, initialGesturesManager, internalGesturesManager);
    LocationComponentOptions options = mock(LocationComponentOptions.class);
    when(options.trackingGesturesManagement()).thenReturn(false);
    camera.initializeOptions(options);

    verify(mapboxMap).setGesturesManager(initialGesturesManager, true, true);
  }

  @Test
  public void onMove_cancellingTransitionWhileNone() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));

    camera.setCameraMode(CameraMode.NONE);
    camera.onMoveListener.onMove(moveGestureDetector);
    verify(mapboxMap, times(1)).cancelTransitions();
    verify(moveGestureDetector, times(0)).interrupt();

    // testing subsequent calls
    camera.onMoveListener.onMove(moveGestureDetector);
    verify(mapboxMap, times(1)).cancelTransitions();
    verify(moveGestureDetector, times(0)).interrupt();
  }

  @Test
  public void onMove_cancellingTransitionWhileGps() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));

    camera.setCameraMode(CameraMode.TRACKING);
    camera.onMoveListener.onMove(moveGestureDetector);
    verify(mapboxMap, times(2)).cancelTransitions();
    verify(moveGestureDetector, times(1)).interrupt();

    // testing subsequent calls
    camera.onMoveListener.onMove(moveGestureDetector);
    verify(mapboxMap, times(2)).cancelTransitions();
    verify(moveGestureDetector, times(1)).interrupt();
  }

  @Test
  public void onMove_cancellingTransitionWhileBearing() {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    LocationCameraController camera = buildCamera(mapboxMap);
    camera.initializeOptions(mock(LocationComponentOptions.class));

    camera.setCameraMode(CameraMode.NONE_COMPASS);
    camera.onMoveListener.onMove(moveGestureDetector);
    verify(mapboxMap, times(2)).cancelTransitions();
    verify(moveGestureDetector, times(1)).interrupt();

    // testing subsequent calls
    camera.onMoveListener.onMove(moveGestureDetector);
    verify(mapboxMap, times(2)).cancelTransitions();
    verify(moveGestureDetector, times(1)).interrupt();
  }

  private LocationCameraController buildCamera(OnCameraTrackingChangedListener onCameraTrackingChangedListener) {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = mock(OnCameraMoveInvalidateListener.class);
    AndroidGesturesManager initialGesturesManager = mock(AndroidGesturesManager.class);
    AndroidGesturesManager internalGesturesManager = mock(AndroidGesturesManager.class);
    return new LocationCameraController(mapboxMap, moveGestureDetector,
      onCameraTrackingChangedListener, onCameraMoveInvalidateListener, initialGesturesManager, internalGesturesManager);
  }

  private LocationCameraController buildCamera(MoveGestureDetector moveGestureDetector) {
    MapboxMap mapboxMap = mock(MapboxMap.class);
    when(mapboxMap.getUiSettings()).thenReturn(mock(UiSettings.class));
    OnCameraTrackingChangedListener onCameraTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = mock(OnCameraMoveInvalidateListener.class);
    AndroidGesturesManager initialGesturesManager = mock(AndroidGesturesManager.class);
    AndroidGesturesManager internalGesturesManager = mock(AndroidGesturesManager.class);
    return new LocationCameraController(mapboxMap, moveGestureDetector,
      onCameraTrackingChangedListener, onCameraMoveInvalidateListener, initialGesturesManager, internalGesturesManager);
  }

  private LocationCameraController buildCamera(MapboxMap mapboxMap) {
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    OnCameraTrackingChangedListener onCameraTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = mock(OnCameraMoveInvalidateListener.class);
    AndroidGesturesManager initialGesturesManager = mock(AndroidGesturesManager.class);
    AndroidGesturesManager internalGesturesManager = mock(AndroidGesturesManager.class);
    return new LocationCameraController(mapboxMap, moveGestureDetector,
      onCameraTrackingChangedListener, onCameraMoveInvalidateListener, initialGesturesManager, internalGesturesManager);
  }

  private LocationCameraController buildCamera(MapboxMap mapboxMap, AndroidGesturesManager initialGesturesManager,
                                               AndroidGesturesManager internalGesturesManager) {
    MoveGestureDetector moveGestureDetector = mock(MoveGestureDetector.class);
    OnCameraTrackingChangedListener onCameraTrackingChangedListener = mock(OnCameraTrackingChangedListener.class);
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = mock(OnCameraMoveInvalidateListener.class);
    return new LocationCameraController(mapboxMap, moveGestureDetector,
      onCameraTrackingChangedListener, onCameraMoveInvalidateListener, initialGesturesManager, internalGesturesManager);
  }
}
