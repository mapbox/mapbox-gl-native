package com.mapbox.mapboxsdk.testapp.activity.style

import android.graphics.drawable.Drawable
import android.os.Bundle
import android.support.v4.content.res.ResourcesCompat
import android.support.v7.app.AppCompatActivity
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.style.layers.PropertyFactory.*
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.R
import java.net.URI
import kotlinx.android.synthetic.main.activity_map_simple.*

/**
 * Activity showcasing how to load symbols on a map without a Style URI or Style JSON.
 */
class NoStyleActivity : AppCompatActivity() {

  private val imageIcon: Drawable by lazy {
    ResourcesCompat.getDrawable(resources, R.drawable.ic_add_white, theme)!!
  }

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_map_simple)
    mapView.getMapAsync { map ->
      map.moveCamera(CameraUpdateFactory.newLatLngZoom(cameraTarget, cameraZoom))
      map.setStyle(
        Style.Builder()
          .withImage(imageId, imageIcon)
          .withSource(GeoJsonSource(sourceId, URI("asset://points-sf.geojson")))
          .withLayer(SymbolLayer(layerId, sourceId).withProperties(iconImage(imageId)))
      )
    }
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

  companion object {
    const val layerId = "custom-layer-id"
    const val sourceId = "custom-source-id"
    const val imageId = "image-id"
    const val cameraZoom = 10.0
    val cameraTarget = LatLng(37.758912, -122.442578)
  }
}