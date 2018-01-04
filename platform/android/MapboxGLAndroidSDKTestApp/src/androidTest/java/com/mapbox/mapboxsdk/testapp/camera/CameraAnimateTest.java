package com.mapbox.mapboxsdk.testapp.camera;

import android.graphics.PointF;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;

import org.junit.Ignore;
import org.junit.Test;

import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.assertEquals;

public class CameraAnimateTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  @Ignore
  public void testAnimateToCameraPositionTarget() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      float zoom = 1.0f;
      LatLng moveTarget = new LatLng(1, 1);
      CameraPosition initialPosition = new CameraPosition.Builder().target(
        new LatLng()).zoom(zoom).bearing(0).tilt(0).build();
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Default camera position should match default", cameraPosition, initialPosition);
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLng(moveTarget));
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
        moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA);
      assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
        moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA);
    });
  }

  @Test
  @Ignore
  public void testAnimateToCameraPositionTargetZoom() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final float moveZoom = 15.5f;
      final LatLng moveTarget = new LatLng(1.0000000001, 1.0000000003);
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(moveTarget, moveZoom));
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
        moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA);
      assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
        moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA);
      assertEquals("Moved zoom should match", cameraPosition.zoom, moveZoom, TestConstants.ZOOM_DELTA);
    });
  }

  @Test
  @Ignore
  public void testAnimateToCameraPosition() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final LatLng moveTarget = new LatLng(1.0000000001, 1.0000000003);
      final float moveZoom = 15.5f;
      final float moveTilt = 45.5f;
      final float moveBearing = 12.5f;

      mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(
        new CameraPosition.Builder()
          .target(moveTarget)
          .zoom(moveZoom)
          .tilt(moveTilt)
          .bearing(moveBearing)
          .build())
      );
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
        moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA);
      assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
        moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA);
      assertEquals("Moved zoom should match", cameraPosition.zoom, moveZoom, TestConstants.ZOOM_DELTA);
      assertEquals("Moved zoom should match", cameraPosition.tilt, moveTilt, TestConstants.TILT_DELTA);
      assertEquals("Moved bearing should match", cameraPosition.bearing, moveBearing, TestConstants.BEARING_DELTA);
    });
  }

  @Test
  @Ignore
  public void testAnimateToBounds() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final LatLng centerBounds = new LatLng(1, 1);
      LatLng cornerOne = new LatLng();
      LatLng cornerTwo = new LatLng(2, 2);
      final LatLngBounds.Builder builder = new LatLngBounds.Builder();
      builder.include(cornerOne);
      builder.include(cornerTwo);
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngBounds(builder.build(), 0));
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera position latitude should match center bounds",
        cameraPosition.target.getLatitude(),
        centerBounds.getLatitude(),
        TestConstants.LAT_LNG_DELTA);
      assertEquals("Moved camera position longitude should match center bounds",
        cameraPosition.target.getLongitude(),
        centerBounds.getLongitude(),
        TestConstants.LAT_LNG_DELTA);
    });
  }

  @Test
  @Ignore
  public void testAnimateToMoveBy() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final PointF centerPoint = mapboxMap.getProjection().toScreenLocation(mapboxMap.getCameraPosition().target);
      final LatLng moveTarget = new LatLng(2, 2);
      final PointF moveTargetPoint = mapboxMap.getProjection().toScreenLocation(moveTarget);
      mapboxMap.animateCamera(CameraUpdateFactory.scrollBy(
        moveTargetPoint.x - centerPoint.x, moveTargetPoint.y - centerPoint.y));
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
        moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA_LARGE);
      assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
        moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA_LARGE);
    });
  }

  @Test
  @Ignore
  public void testAnimateToZoomIn() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      float zoom = 1.0f;
      mapboxMap.animateCamera(CameraUpdateFactory.zoomIn());
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera zoom should match moved camera zoom", cameraPosition.zoom, zoom + 1,
        TestConstants.ZOOM_DELTA);
    });
  }

  @Test
  @Ignore
  public void testAnimateToZoomOut() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      float zoom = 10.0f;
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(), zoom));
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      mapboxMap.animateCamera(CameraUpdateFactory.zoomOut());
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera zoom should match moved camera zoom", cameraPosition.zoom, zoom - 1,
        TestConstants.ZOOM_DELTA);
    });
  }

  @Test
  @Ignore
  public void testAnimateToZoomBy() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      float zoom = 1.0f;
      final float zoomBy = 2.45f;
      mapboxMap.animateCamera(CameraUpdateFactory.zoomBy(zoomBy));
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera zoom should match moved camera zoom", cameraPosition.zoom, zoom + zoomBy,
        TestConstants.ZOOM_DELTA);
    });
  }

  @Test
  @Ignore
  public void testAnimateToZoomTo() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final float zoomTo = 2.45f;
      mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(zoomTo));
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      assertEquals("Moved camera zoom should match moved camera zoom", cameraPosition.zoom, zoomTo,
        TestConstants.ZOOM_DELTA);
    });
  }
}

