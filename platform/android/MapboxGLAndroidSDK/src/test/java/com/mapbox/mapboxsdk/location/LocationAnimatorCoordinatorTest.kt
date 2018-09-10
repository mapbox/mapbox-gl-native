package com.mapbox.mapboxsdk.location

import android.location.Location
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_TRACKING_TILT_ANIM_DURATION
import com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_TRACKING_ZOOM_ANIM_DURATION
import com.mapbox.mapboxsdk.location.MapboxAnimator.*
import junit.framework.Assert.assertEquals
import junit.framework.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.Mockito
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class LocationAnimatorCoordinatorTest {

  private lateinit var locationAnimatorCoordinator: LocationAnimatorCoordinator
  private val cameraPosition: CameraPosition = CameraPosition.DEFAULT

  @Before
  fun setUp() {
    locationAnimatorCoordinator = LocationAnimatorCoordinator()
  }

  @Test
  fun feedNewLocation_animatorsAreCreated() {
    locationAnimatorCoordinator.feedNewLocation(Location(""), cameraPosition, false)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_GPS_BEARING] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_LATLNG] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING] != null)
  }

  @Test
  fun feedNewLocation_animatorValue() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    val cameraLatLngTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG]?.target as LatLng
    assertEquals(cameraLatLngTarget.latitude, cameraLatLngTarget.latitude)

    val layerLatLngTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_LATLNG]?.target as LatLng
    assertEquals(layerLatLngTarget.latitude, layerLatLngTarget.latitude)

    val cameraBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, cameraBearingTarget)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, layerBearingTarget)
  }

  @Test
  fun feedNewLocation_isNorth_animatorsAreCreated() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, false)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_GPS_BEARING] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_LATLNG] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING] != null)
  }

  @Test
  fun feedNewLocation_isNorth_animatorValue() {
    val location = Location("")
    location.latitude = 51.0
    location.longitude = 17.0
    location.bearing = 35f
    locationAnimatorCoordinator.feedNewLocation(location, cameraPosition, true)

    val cameraLatLngTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG]?.target as LatLng
    assertEquals(cameraLatLngTarget.latitude, cameraLatLngTarget.latitude)

    val layerLatLngTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_LATLNG]?.target as LatLng
    assertEquals(layerLatLngTarget.latitude, layerLatLngTarget.latitude)

    val cameraBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_GPS_BEARING]?.target as Float
    assertEquals(0f, cameraBearingTarget)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_GPS_BEARING]?.target as Float
    assertEquals(location.bearing, layerBearingTarget)
  }

  @Test
  fun feedNewCompassBearing_animatorsAreCreated() {
    locationAnimatorCoordinator.feedNewCompassBearing(77f, cameraPosition)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_COMPASS_BEARING] != null)
    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_COMPASS_BEARING] != null)
  }

  @Test
  fun feedNewCompassBearing_animatorValue() {
    val bearing = 77f
    locationAnimatorCoordinator.feedNewCompassBearing(bearing, cameraPosition)

    val cameraBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_COMPASS_BEARING]?.target as Float
    assertEquals(bearing, cameraBearingTarget)

    val layerBearingTarget = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_COMPASS_BEARING]?.target as Float
    assertEquals(bearing, layerBearingTarget)
  }

  @Test
  fun feedNewAccuracyRadius_animatorsCreated() {
    locationAnimatorCoordinator.feedNewAccuracyRadius(150f, false)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY] != null)
  }

  @Test
  fun feedNewAccuracyRadius_animatorValue() {
    val accuracy = 150f
    locationAnimatorCoordinator.feedNewAccuracyRadius(accuracy, false)

    val layerAccuracy = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY]?.target as Float
    assertEquals(layerAccuracy, accuracy)

    val animationDuration = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY]?.duration as Long
    assertEquals(LocationComponentConstants.ACCURACY_RADIUS_ANIMATION_DURATION, animationDuration)
  }

  @Test
  fun feedNewAccuracyRadius_noAnimation_animatorsCreated() {
    locationAnimatorCoordinator.feedNewAccuracyRadius(150f, true)

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY] != null)
  }

  @Test
  fun feedNewAccuracyRadius_noAnimation_animatorValue() {
    val accuracy = 150f
    locationAnimatorCoordinator.feedNewAccuracyRadius(accuracy, true)

    val layerAccuracy = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY]?.target as Float
    assertEquals(layerAccuracy, accuracy)

    val animationDuration = locationAnimatorCoordinator.animatorArray[ANIMATOR_LAYER_ACCURACY]?.duration as Long
    assertEquals(0L, animationDuration)
  }

  @Test
  fun feedNewZoomLevel_animatorsCreated() {
    locationAnimatorCoordinator.feedNewZoomLevel(
      15.0,
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_ZOOM] != null)
  }

  @Test
  fun feedNewZoomLevel_animatorValue() {
    val zoom = 15.0f
    locationAnimatorCoordinator.feedNewZoomLevel(
      zoom.toDouble(),
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )

    val animationDuration = locationAnimatorCoordinator.animatorArray[ANIMATOR_ZOOM]?.duration as Long
    assertEquals(DEFAULT_TRACKING_ZOOM_ANIM_DURATION, animationDuration)

    val target = locationAnimatorCoordinator.animatorArray[ANIMATOR_ZOOM]?.target as Float
    assertEquals(zoom, target)
  }

  @Test
  fun feedNewTiltLevel_animatorsCreated() {
    locationAnimatorCoordinator.feedNewTilt(
      30.0,
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_TILT] != null)
  }

  @Test
  fun feedNewTiltLevel_animatorValue() {
    val tilt = 30.0f
    locationAnimatorCoordinator.feedNewTilt(
      tilt.toDouble(),
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    val animationDuration = locationAnimatorCoordinator.animatorArray[ANIMATOR_TILT]?.duration as Long
    assertEquals(DEFAULT_TRACKING_TILT_ANIM_DURATION, animationDuration)

    val target = locationAnimatorCoordinator.animatorArray[ANIMATOR_TILT]?.target as Float
    assertEquals(tilt, target)
  }

  @Test
  fun cancelAllAnimators() {
    locationAnimatorCoordinator.feedNewLocation(Location(""), cameraPosition, true)
    locationAnimatorCoordinator.cancelAllAnimations()

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_CAMERA_LATLNG] == null)
  }

  @Test
  fun cancelZoomAnimators() {
    locationAnimatorCoordinator.feedNewZoomLevel(
      15.0,
      cameraPosition,
      DEFAULT_TRACKING_ZOOM_ANIM_DURATION,
      null
    )
    locationAnimatorCoordinator.cancelZoomAnimation()

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_ZOOM] == null)
  }

  @Test
  fun cancelTiltAnimation() {
    locationAnimatorCoordinator.feedNewTilt(
      30.0,
      cameraPosition,
      DEFAULT_TRACKING_TILT_ANIM_DURATION,
      null
    )

    locationAnimatorCoordinator.cancelTiltAnimation()

    assertTrue(locationAnimatorCoordinator.animatorArray[ANIMATOR_TILT] == null)
  }

  @Test
  fun resetAllCameraAnimations_empty() {
    locationAnimatorCoordinator.resetAllCameraAnimations(cameraPosition, false)
    assertTrue(locationAnimatorCoordinator.animatorArray.size() == 0)
  }

  @Test
  fun addLayerListener() {
    val layerListener = Mockito.mock(OnLayerAnimationsValuesChangeListener::class.java)
    locationAnimatorCoordinator.addLayerListener(layerListener)

    assertTrue(locationAnimatorCoordinator.layerListeners.contains(layerListener))
  }

  @Test
  fun removeLayerListener() {
    val layerListener = Mockito.mock(OnLayerAnimationsValuesChangeListener::class.java)
    locationAnimatorCoordinator.addLayerListener(layerListener)
    locationAnimatorCoordinator.removeLayerListener(layerListener)

    assertTrue(locationAnimatorCoordinator.layerListeners.isEmpty())
  }

  @Test
  fun addCameraListener() {
    val cameraListener = Mockito.mock(OnCameraAnimationsValuesChangeListener::class.java)
    locationAnimatorCoordinator.addCameraListener(cameraListener)

    assertTrue(locationAnimatorCoordinator.cameraListeners.contains(cameraListener))
  }

  @Test
  fun removeCameraListener() {
    val cameraListener = Mockito.mock(OnCameraAnimationsValuesChangeListener::class.java)
    locationAnimatorCoordinator.addCameraListener(cameraListener)
    locationAnimatorCoordinator.removeCameraListener(cameraListener)

    assertTrue(locationAnimatorCoordinator.cameraListeners.isEmpty())
  }
}