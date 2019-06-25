package com.mapbox.mapboxsdk.testapp.maps

import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.AppCenter
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity
import junit.framework.Assert.assertEquals
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import java.util.concurrent.TimeoutException

@RunWith(AndroidJUnit4::class)
class ImageMissingTest : AppCenter(){

  @Rule
  @JvmField
  var rule = ActivityTestRule(EspressoTestActivity::class.java)

  private lateinit var mapView: MapView
  private lateinit var latch: CountDownLatch

  @Before
  fun setup() {
    latch = CountDownLatch(1)
  }

  @Test
  fun testMissingImage() {
    rule.runOnUiThread {
      initMap(styleJson).addOnStyleImageMissingListener {
        assertEquals("missing-icon", it)
        latch.countDown()
      }
    }

    if (!latch.await(10, TimeUnit.SECONDS)) {
      throw TimeoutException()
    }
  }

  @Test
  fun testMissingImage_invalidSprite() {
    rule.runOnUiThread {
      initMap(styleJsonInvalidSprite).addOnStyleImageMissingListener {
        assertEquals("missing-icon", it)
        latch.countDown()
      }
    }

    if (!latch.await(10, TimeUnit.SECONDS)) {
      throw TimeoutException()
    }
  }

  private fun initMap(style: String): MapView {
    mapView = rule.activity.findViewById(R.id.mapView)
    mapView.getMapAsync { it.setStyle(Style.Builder().fromJson(style)) }
    return mapView
  }

  companion object {
    private const val styleJson = """
    {
      "version": 8,
      "name": "Mapbox Streets",
      "sprite": "mapbox://sprites/mapbox/streets-v8",
      "glyphs": "mapbox://fonts/mapbox/{fontstack}/{range}.pbf",
      "sources": {
        "point": {
          "type": "geojson",
          "data": {
            "type": "Feature",
            "properties": {},
            "geometry": {
              "type": "Point",
              "coordinates": [0, 0]
            }
          }
        }
      },
      "layers": [{
        "id": "bg",
        "type": "background",
        "paint": {
          "background-color": "#f00"
        }
      }, {
        "id": "point",
        "type": "circle",
        "source": "point",
        "paint": {
          "circle-radius": 100
        }
      }, {
        "id": "icon",
        "type": "symbol",
        "source": "point",
        "layout": {
          "icon-image": "missing-icon"
        }
      }]
    }
          """

    private const val styleJsonInvalidSprite = """
    {
      "version": 8,
      "name": "Mapbox Streets",
      "sprite": "asset://sprites/mapbox/invalid",
      "glyphs": "mapbox://fonts/mapbox/{fontstack}/{range}.pbf",
      "sources": {
        "point": {
          "type": "geojson",
          "data": {
            "type": "Feature",
            "properties": {},
            "geometry": {
              "type": "Point",
              "coordinates": [0, 0]
            }
          }
        }
      },
      "layers": [{
        "id": "bg",
        "type": "background",
        "paint": {
          "background-color": "#f00"
        }
      }, {
        "id": "point",
        "type": "circle",
        "source": "point",
        "paint": {
          "circle-radius": 100
        }
      }, {
        "id": "icon",
        "type": "symbol",
        "source": "point",
        "layout": {
          "icon-image": "missing-icon"
        }
      }]
    }
          """
  }
}
