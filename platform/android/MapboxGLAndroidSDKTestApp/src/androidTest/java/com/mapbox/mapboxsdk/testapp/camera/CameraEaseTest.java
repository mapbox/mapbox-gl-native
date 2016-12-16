package com.mapbox.mapboxsdk.testapp.camera;

import android.graphics.PointF;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.assertEquals;

public class CameraEaseTest {

  @Rule
  public final ActivityTestRule<EspressoTestActivity> rule = new ActivityTestRule<>(EspressoTestActivity.class);

  private OnMapReadyIdlingResource idlingResource;

  @Before
  public void registerIdlingResource() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @Test
  public void testEaseToCameraPositionTarget() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    /*TODO remove zoom #6474*/
    float zoom = 1.0f;
    LatLng moveTarget = new LatLng(1, 1);

    CameraPosition initialPosition = new CameraPosition.Builder().target(
      new LatLng()).zoom(zoom).bearing(0).tilt(0).build();
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Default camera position should match default", cameraPosition, initialPosition);

    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap, CameraUpdateFactory.newLatLng(moveTarget)));
    cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
      moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA);
    assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
      moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA);
  }

  @Test
  public void testEaseToCameraPositionTargetZoom() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    final float moveZoom = 15.5f;
    final LatLng moveTarget = new LatLng(1.0000000001, 1.0000000003);

    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap,
      CameraUpdateFactory.newLatLngZoom(moveTarget, moveZoom)));
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
      moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA);
    assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
      moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA);
    assertEquals("Moved zoom should match", cameraPosition.zoom, moveZoom, TestConstants.ZOOM_DELTA);
  }

  @Test
  public void testEaseToCameraPosition() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    final LatLng moveTarget = new LatLng(1.0000000001, 1.0000000003);
    final float moveZoom = 15.5f;
    final float moveTilt = 45.5f;
    final float moveBearing = 12.5f;

    onView(withId(R.id.mapView)).perform(
      new EaseCameraAction(mapboxMap, CameraUpdateFactory.newCameraPosition(
        new CameraPosition.Builder()
          .target(moveTarget)
          .zoom(moveZoom)
          .tilt(moveTilt)
          .bearing(moveBearing)
          .build()))
    );

    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
      moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA);
    assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
      moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA);
    assertEquals("Moved zoom should match", cameraPosition.zoom, moveZoom, TestConstants.ZOOM_DELTA);
    assertEquals("Moved zoom should match", cameraPosition.tilt, moveTilt, TestConstants.TILT_DELTA);
    assertEquals("Moved bearing should match", cameraPosition.bearing, moveBearing, TestConstants.BEARING_DELTA);
  }

  @Test
  public void testEaseToBounds() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    final LatLng centerBounds = new LatLng(1, 1);
    LatLng cornerOne = new LatLng();
    LatLng cornerTwo = new LatLng(2, 2);

    final LatLngBounds.Builder builder = new LatLngBounds.Builder();
    builder.include(cornerOne);
    builder.include(cornerTwo);

    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap,
      CameraUpdateFactory.newLatLngBounds(builder.build(), 0)));

    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera position latitude should match center bounds",
      cameraPosition.target.getLatitude(),
      centerBounds.getLatitude(),
      TestConstants.LAT_LNG_DELTA);

    assertEquals("Moved camera position longitude should match center bounds",
      cameraPosition.target.getLongitude(),
      centerBounds.getLongitude(),
      TestConstants.LAT_LNG_DELTA);
  }

  @Test
  public void testEaseToMoveBy() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    final PointF centerPoint = mapboxMap.getProjection().toScreenLocation(mapboxMap.getCameraPosition().target);
    final LatLng moveTarget = new LatLng(2, 2);
    final PointF moveTargetPoint = mapboxMap.getProjection().toScreenLocation(moveTarget);

    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap, CameraUpdateFactory.scrollBy(
      moveTargetPoint.x - centerPoint.x, moveTargetPoint.y - centerPoint.y)));

    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera position latitude should match", cameraPosition.target.getLatitude(),
      moveTarget.getLatitude(), TestConstants.LAT_LNG_DELTA_LARGE);
    assertEquals("Moved camera position longitude should match", cameraPosition.target.getLongitude(),
      moveTarget.getLongitude(), TestConstants.LAT_LNG_DELTA_LARGE);
  }

  @Test
  public void testEaseToZoomIn() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    /*TODO fix zoom #6474*/
    float zoom = 1.0f;

    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap, CameraUpdateFactory.zoomIn()));
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera zoom should match moved camera zoom", cameraPosition.zoom, zoom + 1,
      TestConstants.ZOOM_DELTA);
  }

  @Test
  public void testEaseToZoomOut() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    /*TODO fix zoom #6474*/
    float zoom = 10.0f;
    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap,
      CameraUpdateFactory.newLatLngZoom(new LatLng(), zoom)));
    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap, CameraUpdateFactory.zoomOut()));
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera zoom should match moved camera zoom", 9, cameraPosition.zoom, TestConstants.ZOOM_DELTA);
  }

  @Test
  public void testEaseToZoomBy() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    /*TODO fix zoom #6474*/
    float zoom = 1.0f;
    final float zoomBy = 2.45f;

    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap, CameraUpdateFactory.zoomBy(zoomBy)));
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera zoom should match moved camera zoom", cameraPosition.zoom, zoom + zoomBy,
      TestConstants.ZOOM_DELTA);
  }

  @Test
  public void testEaseToZoomTo() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    /*TODO fix zoom #6474*/
    final float zoomTo = 2.45f;

    onView(withId(R.id.mapView)).perform(new EaseCameraAction(mapboxMap, CameraUpdateFactory.zoomTo(zoomTo)));
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Moved camera zoom should match moved camera zoom", cameraPosition.zoom, zoomTo,
      TestConstants.ZOOM_DELTA);
  }

  @After
  public void unregisterIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }

  private class EaseCameraAction implements ViewAction {

    private MapboxMap mapboxMap;
    private CameraUpdate cameraUpdate;

    EaseCameraAction(MapboxMap map, CameraUpdate update) {
      mapboxMap = map;
      cameraUpdate = update;
    }

    @Override
    public Matcher<View> getConstraints() {
      return isDisplayed();
    }

    @Override
    public String getDescription() {
      return getClass().getSimpleName();
    }

    @Override
    public void perform(UiController uiController, View view) {
      mapboxMap.easeCamera(cameraUpdate);
      uiController.loopMainThreadForAtLeast(TestConstants.ANIMATION_TEST_TIME);
    }
  }
}

