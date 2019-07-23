package com.mapbox.mapboxsdk.snapshotter

import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import java.util.concurrent.TimeoutException
import junit.framework.Assert.assertNotNull
import org.junit.Assert
import org.junit.Ignore
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Integration test that validates if a snapshotter creation
 */
@RunWith(AndroidJUnit4::class)
@Ignore("Ignored until https://github.com/mapbox/mapbox-gl-native/issues/11669 is resolved.")
class MapSnapshotterTest {

  @Rule
  @JvmField
  var rule = ActivityTestRule(FeatureOverviewActivity::class.java)

  private val countDownLatch = CountDownLatch(1)

  @Test
  fun mapSnapshotter() {
    rule.activity.runOnUiThread {
      val options = MapSnapshotter.Options(512, 512)
        .withPixelRatio(1.0f)
        .withStyle(Style.SATELLITE_STREETS)
        .withCameraPosition(
          CameraPosition.Builder()
            .zoom(12.0)
            .target(LatLng(51.145495, 5.742234))
            .build()
        )
      val mapSnapshotter = MapSnapshotter(rule.activity, options)
      mapSnapshotter.start({
        assertNotNull(it)
        assertNotNull(it.bitmap)
        countDownLatch.countDown()
      }, {
        Assert.fail(it)
      })
    }
    if (!countDownLatch.await(30, TimeUnit.SECONDS)) {
      throw TimeoutException()
    }
  }
}