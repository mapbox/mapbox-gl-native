package com.mapbox.mapboxsdk.testapp.activity.camera

import android.content.Context
import android.os.Bundle
import android.support.design.widget.BottomSheetBehavior
import android.support.v7.app.AppCompatActivity
import android.view.View
import com.mapbox.geojson.FeatureCollection
import com.mapbox.geojson.FeatureCollection.fromJson
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.geometry.LatLngBounds
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.style.layers.Property.ICON_ANCHOR_CENTER
import com.mapbox.mapboxsdk.style.layers.PropertyFactory.*
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.utils.GeoParseUtil.loadStringFromAssets
import com.mapbox.mapboxsdk.utils.BitmapUtils
import java.net.URISyntaxException
import kotlinx.android.synthetic.main.activity_latlngbounds.*

/**
 * Test activity showcasing using the LatLngBounds camera API.
 */
class LatLngBoundsActivity : AppCompatActivity() {

  private lateinit var mapboxMap: MapboxMap
  private lateinit var bottomSheetBehavior: BottomSheetBehavior<*>
  private lateinit var bounds: LatLngBounds

  private val peekHeight by lazy {
    375.toPx(this) // 375dp
  }

  private val additionalPadding by lazy {
    32.toPx(this) // 32dp
  }

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_latlngbounds)
    initMapView(savedInstanceState)
  }

  private fun initMapView(savedInstanceState: Bundle?) {
    mapView.onCreate(savedInstanceState)
    mapView.getMapAsync { map ->
      mapboxMap = map

      val featureCollection: FeatureCollection = fromJson(loadStringFromAssets(this, "points-sf.geojson"))
      bounds = createBounds(featureCollection)

      map.getCameraForLatLngBounds(bounds, createPadding(peekHeight))?.let {
        map.cameraPosition = it
      }

      try {
        loadStyle(featureCollection)
      } catch (e: URISyntaxException) {
        e.printStackTrace()
      }
    }
  }

  private fun loadStyle(featureCollection: FeatureCollection) {
    mapboxMap.setStyle(Style.Builder()
      .fromUri(Style.MAPBOX_STREETS)
      .withLayer(SymbolLayer("symbol", "symbol")
        .withProperties(
          iconAllowOverlap(true),
          iconIgnorePlacement(true),
          iconImage("icon"),
          iconAnchor(ICON_ANCHOR_CENTER)
        )
      )
      .withSource(GeoJsonSource("symbol", featureCollection))
      .withImage("icon", BitmapUtils.getDrawableFromRes(this@LatLngBoundsActivity, R.drawable.ic_android)!!)
    ) {
      initBottomSheet()
      fab.setOnClickListener { bottomSheetBehavior.state = BottomSheetBehavior.STATE_EXPANDED }
    }
  }

  private fun initBottomSheet() {
    bottomSheetBehavior = BottomSheetBehavior.from(bottomSheet)
    bottomSheetBehavior.setBottomSheetCallback(object : BottomSheetBehavior.BottomSheetCallback() {
      override fun onSlide(bottomSheet: View, slideOffset: Float) {
        val offset = convertSlideOffset(slideOffset)
        val bottomPadding = (peekHeight * offset).toInt()

        mapboxMap.getCameraForLatLngBounds(bounds, createPadding(bottomPadding))?.let {
          mapboxMap.cameraPosition = it
        }
      }

      override fun onStateChanged(bottomSheet: View, newState: Int) {
        // no-op
      }
    })
  }

  // slideOffset ranges from NaN to -1.0, range from 1.0 to 0 instead
  fun convertSlideOffset(slideOffset: Float): Float {
    return if (slideOffset.equals(Float.NaN)) {
      1.0f
    } else {
      1 + slideOffset
    }
  }

  fun createPadding(bottomPadding: Int): IntArray {
    return intArrayOf(additionalPadding, additionalPadding, additionalPadding, bottomPadding)
  }

  private fun createBounds(featureCollection: FeatureCollection): LatLngBounds {
    val boundsBuilder = LatLngBounds.Builder()
    featureCollection.features()?.let {
      for (feature in it) {
        val point = feature.geometry() as Point
        boundsBuilder.include(LatLng(point.latitude(), point.longitude()))
      }
    }
    return boundsBuilder.build()
  }

  override fun onStart() {
    super.onStart()
    mapView.onStart()
  }

  override fun onResume() {
    super.onResume()
    mapView.onResume()
  }

  override fun onPause() {
    super.onPause()
    mapView.onPause()
  }

  override fun onStop() {
    super.onStop()
    mapView.onStop()
  }

  override fun onLowMemory() {
    super.onLowMemory()
    mapView.onLowMemory()
  }

  override fun onDestroy() {
    super.onDestroy()
    mapView.onDestroy()
  }

  override fun onSaveInstanceState(outState: Bundle) {
    super.onSaveInstanceState(outState)
    mapView.onSaveInstanceState(outState)
  }
}

fun Int.toPx(context: Context): Int = (this * context.resources.displayMetrics.density).toInt()