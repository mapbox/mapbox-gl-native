package com.mapbox.mapboxsdk.style

import android.os.Handler
import android.os.Looper
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.log.Logger
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import java.util.*
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import java.util.concurrent.TimeoutException
import kotlin.concurrent.schedule

/**
 * Test to validate weak reference setup
 */
@RunWith(AndroidJUnit4::class)
class StyleDetachTest {

  @Rule
  @JvmField
  var rule = ActivityTestRule(EspressoTestActivity::class.java)

  private val countDownLatch = CountDownLatch(1)
  private val handler = Handler(Looper.getMainLooper())

  @Test
  fun styleDetachTest() {
    rule.activity.apply {
      runOnUiThread {
        mapView.getMapAsync {
          setupStyle(it)
        }
      }
    }

    if (!countDownLatch.await(15, TimeUnit.SECONDS)) {
      throw TimeoutException()
    }
  }

  fun setupStyle(map: MapboxMap) {
    val source = GeoJsonSource("id")
    Timer().schedule(200, 500) {
      Logger.e(TAG, "Update geojson")
      handler.post { source.setGeoJson("") }
    }

    map.setStyle(Style.Builder().fromUri(Style.MAPBOX_STREETS).withSource(source)) {
      handler.postDelayed({
        Logger.e(TAG, "Set new style")
        map.setStyle(Style.SATELLITE)
        Handler().postDelayed({
          Logger.e(TAG, "End test")
          countDownLatch.countDown()
        }, 2000)
      }, 1000)
    }
  }

  companion object {
    const val TAG = "StyleDetachTest"
  }
}