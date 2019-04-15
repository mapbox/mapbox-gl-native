package com.mapbox.mapboxsdk.testapp.activity.turf

import android.graphics.Color
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.SeekBar
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.style.layers.BackgroundLayer
import com.mapbox.mapboxsdk.style.layers.FillLayer
import com.mapbox.mapboxsdk.style.layers.PropertyFactory.backgroundColor
import com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.turf.TurfTransformation
import kotlinx.android.synthetic.main.activity_physical_circle.*

/**
 * An Activity that showcases how to create a Circle with radius expressed in physical units using a FillLayer.
 */
class PhysicalUnitCircleActivity : AppCompatActivity(), SeekBar.OnSeekBarChangeListener {

  companion object {
    const val LAYER_ID = "circle-id"
    const val SOURCE_ID = "circle-id"
  }

  private lateinit var source: GeoJsonSource
  private var steps: Int = 10
  private var radius: Double = 9000.0

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_physical_circle)
    mapView.onCreate(savedInstanceState)
    mapView.getMapAsync { mapboxMap ->

      mapboxMap.cameraPosition = CameraPosition.Builder()
        .target(LatLng(22.928207, 15.011543))
        .zoom(10.0)
        .build()

      source = GeoJsonSource(SOURCE_ID, TurfTransformation.circle(
        Point.fromLngLat(0.0, 0.0), 9000.0, 10, "meters")
      )

      stepsBar.setOnSeekBarChangeListener(this)
      radiusBar.setOnSeekBarChangeListener(this)

      mapboxMap.setStyle(Style.Builder()
        .fromUrl(Style.SATELLITE_STREETS)
        .withLayer(FillLayer(LAYER_ID, SOURCE_ID).withProperties(fillColor(Color.RED)))
        .withSource(source)
      )
    }
  }

  override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
    seekBar?.let {
      if (it.id == stepsBar.id) {
        steps = progress
      } else {
        radius = progress.toDouble()
      }

      source.setGeoJson(TurfTransformation.circle(
        Point.fromLngLat(0.0, 0.0), radius, steps, "meters")
      )
    }
  }

  override fun onStartTrackingTouch(seekBar: SeekBar?) {
    // no-op
  }

  override fun onStopTrackingTouch(seekBar: SeekBar?) {
    // no-op
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

  override fun onSaveInstanceState(outState: Bundle?) {
    super.onSaveInstanceState(outState)
    outState?.let {
      mapView.onSaveInstanceState(it)
    }
  }
}