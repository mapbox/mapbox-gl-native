package com.mapbox.mapboxsdk.location

import android.content.Context
import com.mapbox.android.core.location.LocationEngine
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.location.modes.CameraMode
import com.mapbox.mapboxsdk.location.modes.RenderMode
import com.mapbox.mapboxsdk.maps.MapboxMap
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.Mock
import org.mockito.Mockito.*
import org.mockito.MockitoAnnotations
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class LocationComponentTest {
  private lateinit var locationComponent: LocationComponent

  @Mock
  private lateinit var locationComponentOptions: LocationComponentOptions

  @Mock
  private lateinit var mapboxMap: MapboxMap

  @Mock
  private lateinit var context: Context

  @Mock
  private lateinit var locationEngine: LocationEngine

  @Mock
  private lateinit var compassEngine: CompassEngine

  @Mock
  private lateinit var locationLayerController: LocationLayerController

  @Mock
  private lateinit var locationCameraController: LocationCameraController

  @Mock
  private lateinit var locationAnimatorCoordinator: LocationAnimatorCoordinator

  @Mock
  private lateinit var staleStateManager: StaleStateManager

  @Before
  fun before() {
    MockitoAnnotations.initMocks(this)
    locationComponent = LocationComponent(mapboxMap, locationEngine, locationLayerController,
      locationCameraController, locationAnimatorCoordinator, staleStateManager, compassEngine)
  }

  @Test
  fun compass_listenWhenConsumedByNoneCamera() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationCameraController.isConsumingCompass).thenReturn(true)
    locationComponent.cameraMode = CameraMode.NONE_COMPASS
    verify(compassEngine).addCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_listenWhenConsumedByTrackingCamera() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationCameraController.isConsumingCompass).thenReturn(true)
    locationComponent.cameraMode = CameraMode.TRACKING_COMPASS
    verify(compassEngine).addCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_listenWhenConsumedByLayer() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    verify(compassEngine).addCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_notListenWhenNotConsumed() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(false)
    `when`(locationCameraController.isConsumingCompass).thenReturn(false)
    locationComponent.renderMode = RenderMode.GPS
    locationComponent.renderMode = RenderMode.NORMAL
    locationComponent.cameraMode = CameraMode.TRACKING
    locationComponent.cameraMode = CameraMode.NONE
    locationComponent.cameraMode = CameraMode.NONE_GPS
    locationComponent.cameraMode = CameraMode.TRACKING_GPS
    locationComponent.cameraMode = CameraMode.TRACKING_GPS_NORTH
    verify(compassEngine, never()).addCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_removeListenerOnChange() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    `when`(locationLayerController.isConsumingCompass).thenReturn(false)
    locationComponent.renderMode = RenderMode.NORMAL
    verify(compassEngine).removeCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_removeListenerOnStop() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    locationComponent.onStop()
    verify(compassEngine).removeCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_reAddListenerOnStart() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    locationComponent.onStop()
    locationComponent.onStart()
    verify(compassEngine, times(2)).addCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_removeListenerOnStyleStartLoad() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    locationComponent.onStartLoadingMap()
    verify(compassEngine).removeCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_reAddListenerOnStyleLoadFinished() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    locationComponent.onStartLoadingMap()
    locationComponent.onFinishLoadingStyle()
    verify(compassEngine, times(2)).addCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_reAddListenerOnlyWhenEnabled() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    locationComponent.isLocationComponentEnabled = false
    locationComponent.onStartLoadingMap()
    locationComponent.onFinishLoadingStyle()
    verify(compassEngine).addCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_notAdListenerWhenDisabled() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.onStart()
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    verify(compassEngine, never()).addCompassListener(any(CompassListener::class.java))
  }

  @Test
  fun compass_notAdListenerWhenStopped() {
    locationComponent.activateLocationComponent(context, locationEngine, locationComponentOptions)
    locationComponent.isLocationComponentEnabled = true
    `when`(mapboxMap.cameraPosition).thenReturn(CameraPosition.DEFAULT)

    `when`(locationLayerController.isConsumingCompass).thenReturn(true)
    locationComponent.renderMode = RenderMode.COMPASS
    verify(compassEngine, never()).addCompassListener(any(CompassListener::class.java))
  }
}