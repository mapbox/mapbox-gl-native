package com.mapbox.mapboxsdk.testapp.style

import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.matcher.ViewMatchers.isRoot
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource.THREAD_POOL_LIMIT
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource.THREAD_PREFIX
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke
import com.mapbox.mapboxsdk.testapp.action.OrientationAction.orientationLandscape
import com.mapbox.mapboxsdk.testapp.action.OrientationAction.orientationPortrait
import com.mapbox.mapboxsdk.testapp.action.WaitAction
import com.mapbox.mapboxsdk.testapp.activity.BaseTest
import com.mapbox.mapboxsdk.testapp.activity.style.GridSourceActivity
import com.mapbox.mapboxsdk.testapp.activity.style.GridSourceActivity.ID_GRID_LAYER
import com.mapbox.mapboxsdk.testapp.activity.style.GridSourceActivity.ID_GRID_SOURCE
import com.mapbox.mapboxsdk.testapp.utils.TestingAsyncUtils
import org.junit.Assert
import org.junit.Test

class CustomGeometrySourceTest : BaseTest() {

  override fun getActivityClass(): Class<*> = GridSourceActivity::class.java

  @Test
  fun sourceNotLeakingThreadsTest() {
    validateTestSetup()
    WaitAction.invoke(4000)
    onView(isRoot()).perform(orientationLandscape())
    WaitAction.invoke(2000)
    onView(isRoot()).perform(orientationPortrait())
    WaitAction.invoke(2000)
    Assert.assertFalse("Threads should be shutdown when the source is destroyed.",
      Thread.getAllStackTraces().keys.filter {
        it.name.startsWith(THREAD_PREFIX)
      }.count() > THREAD_POOL_LIMIT)
  }

  @Test
  fun threadsShutdownWhenSourceRemovedTest() {
    validateTestSetup()
    invoke(mapboxMap) { uiController, mapboxMap ->
      mapboxMap.style!!.removeLayer(ID_GRID_LAYER)
      TestingAsyncUtils.waitForLayer(uiController, mapView)
      mapboxMap.style!!.removeSource(ID_GRID_SOURCE)
      TestingAsyncUtils.waitForLayer(uiController, mapView)
      Assert.assertTrue("There should be no threads running when the source is removed.",
        Thread.getAllStackTraces().keys.filter {
          it.name.startsWith(THREAD_PREFIX)
        }.count() == 0)
    }
  }

  @Test
  fun threadsRestartedWhenSourceReAddedTest() {
    validateTestSetup()
    invoke(mapboxMap) { uiController, mapboxMap ->
      mapboxMap.style!!.removeLayer((rule.activity as GridSourceActivity).layer)
      TestingAsyncUtils.waitForLayer(uiController, mapView)
      mapboxMap.style!!.removeSource(ID_GRID_SOURCE)
      TestingAsyncUtils.waitForLayer(uiController, mapView)
      mapboxMap.style!!.addSource((rule.activity as GridSourceActivity).source)
      mapboxMap.style!!.addLayer((rule.activity as GridSourceActivity).layer)
      TestingAsyncUtils.waitForLayer(uiController, mapView)
      Assert.assertTrue("Threads should be restarted when the source is re-added to the map.",
        Thread.getAllStackTraces().keys.filter {
          it.name.startsWith(THREAD_PREFIX)
        }.count() == THREAD_POOL_LIMIT)
    }
  }
}