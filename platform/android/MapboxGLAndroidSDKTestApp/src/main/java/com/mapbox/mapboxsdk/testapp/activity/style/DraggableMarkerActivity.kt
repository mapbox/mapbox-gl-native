package com.mapbox.mapboxsdk.testapp.activity.style

import android.graphics.PointF
import android.os.Bundle
import android.support.design.widget.Snackbar
import android.support.v7.app.AppCompatActivity
import android.view.MotionEvent
import android.view.View
import com.mapbox.android.gestures.AndroidGesturesManager
import com.mapbox.android.gestures.MoveGestureDetector
import com.mapbox.geojson.Feature
import com.mapbox.geojson.FeatureCollection
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.annotations.IconFactory
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.style.layers.PropertyFactory.*
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_draggable_marker.*

/**
 * An Activity that showcases how to make symbols draggable.
 */
class DraggableMarkerActivity : AppCompatActivity() {
  companion object {
    private const val sourceId = "source_draggable"
    private const val layerId = "layer_draggable"
    private const val markerImageId = "marker_icon_draggable"

    private var latestId: Long = 0
    fun generateMarkerId(): String {
      if (latestId == Long.MAX_VALUE) {
        throw RuntimeException("You've added too many markers.")
      }
      return latestId++.toString()
    }
  }

  private val actionBarHeight: Int by lazy {
    supportActionBar?.height ?: 0
  }

  // View property is required for activity sanity tests
  // we perform reflection on this requires using findViewById
  private lateinit var mapView: MapView
  private lateinit var mapboxMap: MapboxMap
  private val featureCollection = FeatureCollection.fromFeatures(mutableListOf())
  private val source = GeoJsonSource(sourceId, featureCollection)
  private val layer = SymbolLayer(layerId, sourceId)
    .withProperties(
      iconImage(markerImageId),
      iconAllowOverlap(true),
      iconIgnorePlacement(true))

  private var draggableSymbolsManager: DraggableSymbolsManager? = null

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_draggable_marker)

    mapView = findViewById(R.id.mapView)
    mapView.onCreate(savedInstanceState)
    mapView.getMapAsync { mapboxMap ->
      this.mapboxMap = mapboxMap

      mapboxMap.setStyle(
        Style.Builder()
          .fromUri(Style.MAPBOX_STREETS)
          .withImage(markerImageId, IconFactory.getInstance(this).defaultMarker().bitmap)
          .withSource(source)
          .withLayer(layer)
      )

      // Add initial markers
      addMarker(LatLng(52.407210, 16.924324))
      addMarker(LatLng(41.382679, 2.181555))
      addMarker(LatLng(51.514886, -0.112589))

      // Initial camera position
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(
        LatLng(45.0, 8.0), 3.0
      ))

      mapboxMap.addOnMapClickListener {
        // Adding a marker on map click
        val features = mapboxMap.queryRenderedSymbols(it, layerId)
        if (features.isEmpty()) {
          addMarker(it)
        } else {
          // Displaying marker info on marker click
          Snackbar.make(
            mapView,
            "Marker's position: %.4f, %.4f".format(it.latitude, it.longitude),
            Snackbar.LENGTH_LONG)
            .show()
        }

        false
      }

      draggableSymbolsManager = DraggableSymbolsManager(
        mapView, mapboxMap, featureCollection, source, layerId, actionBarHeight, 0)

      // Adding symbol drag listeners
      draggableSymbolsManager?.addOnSymbolDragListener(object : DraggableSymbolsManager.OnSymbolDragListener {
        override fun onSymbolDragStarted(id: String) {
          draggedMarkerPositionTv.visibility = View.VISIBLE
          Snackbar.make(
            mapView,
            "Marker drag started (%s)".format(id),
            Snackbar.LENGTH_SHORT)
            .show()
        }

        override fun onSymbolDrag(id: String) {
          val point = featureCollection.features()?.find {
            it.id() == id
          }?.geometry() as Point
          draggedMarkerPositionTv.text = "Dragged marker's position: %.4f, %.4f".format(point.latitude(), point.longitude())
        }

        override fun onSymbolDragFinished(id: String) {
          draggedMarkerPositionTv.visibility = View.GONE
          Snackbar.make(
            mapView,
            "Marker drag finished (%s)".format(id),
            Snackbar.LENGTH_SHORT)
            .show()
        }
      })
    }
  }

  private fun addMarker(latLng: LatLng) {
    featureCollection.features()?.add(
      Feature.fromGeometry(Point.fromLngLat(latLng.longitude, latLng.latitude), null, generateMarkerId()))
    source.setGeoJson(featureCollection)
  }

  override fun dispatchTouchEvent(ev: MotionEvent?): Boolean {
    // Dispatching parent's touch events to the manager
    draggableSymbolsManager?.onParentTouchEvent(ev)
    return super.dispatchTouchEvent(ev)
  }

  /**
   * A manager, that allows dragging symbols after they are long clicked.
   * Since this manager lives outside of the Maps SDK, we need to intercept parent's motion events
   * and pass them with [DraggableSymbolsManager.onParentTouchEvent].
   * If we were to try and overwrite [AppCompatActivity.onTouchEvent], those events would've been
   * consumed by the map.
   *
   * We also need to setup a [DraggableSymbolsManager.androidGesturesManager],
   * because after disabling map's gestures and starting the drag process
   * we still need to listen for move gesture events which map won't be able to provide anymore.
   *
   * @param mapView the mapView
   * @param mapboxMap the mapboxMap
   * @param symbolsCollection the collection that contains all the symbols that we want to be draggable
   * @param symbolsSource the source that contains the [symbolsCollection]
   * @param symbolsLayerId the ID of the layer that the symbols are displayed on
   * @param touchAreaShiftX X-axis padding that is applied to the parent's window motion event,
   * as that window can be bigger than the [mapView].
   * @param touchAreaShiftY Y-axis padding that is applied to the parent's window motion event,
   * as that window can be bigger than the [mapView].
   * @param touchAreaMaxX maximum value of X-axis motion event
   * @param touchAreaMaxY maximum value of Y-axis motion event
   */
  class DraggableSymbolsManager(
    mapView: MapView,
    private val mapboxMap: MapboxMap,
    private val symbolsCollection: FeatureCollection,
    private val symbolsSource: GeoJsonSource,
    private val symbolsLayerId: String,
    private val touchAreaShiftY: Int = 0,
    private val touchAreaShiftX: Int = 0,
    private val touchAreaMaxX: Int = mapView.width,
    private val touchAreaMaxY: Int = mapView.height
  ) {

    private val androidGesturesManager: AndroidGesturesManager = AndroidGesturesManager(mapView.context, false)
    private var draggedSymbolId: String? = null
    private val onSymbolDragListeners: MutableList<OnSymbolDragListener> = mutableListOf<OnSymbolDragListener>()

    init {
      mapboxMap.addOnMapLongClickListener {
        // Starting the drag process on long click
        draggedSymbolId = mapboxMap.queryRenderedSymbols(it, symbolsLayerId).firstOrNull()?.id()?.also { id ->
          mapboxMap.uiSettings.setAllGesturesEnabled(false)
          mapboxMap.gesturesManager.moveGestureDetector.interrupt()
          notifyOnSymbolDragListeners {
            onSymbolDragStarted(id)
          }
        }
        false
      }

      androidGesturesManager.setMoveGestureListener(MyMoveGestureListener())
    }

    inner class MyMoveGestureListener : MoveGestureDetector.OnMoveGestureListener {
      override fun onMoveBegin(detector: MoveGestureDetector): Boolean {
        return true
      }

      override fun onMove(detector: MoveGestureDetector, distanceX: Float, distanceY: Float): Boolean {
        if (detector.pointersCount > 1) {
          // Stopping the drag when we don't work with a simple, on-pointer move anymore
          stopDragging()
          return true
        }

        // Updating symbol's position
        draggedSymbolId?.also { draggedSymbolId ->
          val moveObject = detector.getMoveObject(0)
          val point = PointF(moveObject.currentX - touchAreaShiftX, moveObject.currentY - touchAreaShiftY)

          if (point.x < 0 || point.y < 0 || point.x > touchAreaMaxX || point.y > touchAreaMaxY) {
            stopDragging()
          }

          val latLng = mapboxMap.projection.fromScreenLocation(point)

          symbolsCollection.features()?.indexOfFirst {
            it.id() == draggedSymbolId
          }?.also { index ->
            symbolsCollection.features()?.get(index)?.also { oldFeature ->
              val properties = oldFeature.properties()
              val newFeature = Feature.fromGeometry(
                Point.fromLngLat(latLng.longitude, latLng.latitude),
                properties,
                draggedSymbolId
              )
              symbolsCollection.features()?.set(index, newFeature)
              symbolsSource.setGeoJson(symbolsCollection)
              notifyOnSymbolDragListeners {
                onSymbolDrag(draggedSymbolId)
              }
              return true
            }
          }
        }

        return false
      }

      override fun onMoveEnd(detector: MoveGestureDetector, velocityX: Float, velocityY: Float) {
        // Stopping the drag when move ends
        stopDragging()
      }
    }

    private fun stopDragging() {
      mapboxMap.uiSettings.setAllGesturesEnabled(true)
      draggedSymbolId?.let {
        notifyOnSymbolDragListeners {
          onSymbolDragFinished(it)
        }
      }
      draggedSymbolId = null
    }

    fun onParentTouchEvent(ev: MotionEvent?) {
      androidGesturesManager.onTouchEvent(ev)
    }

    private fun notifyOnSymbolDragListeners(action: OnSymbolDragListener.() -> Unit) {
      onSymbolDragListeners.forEach(action)
    }

    fun addOnSymbolDragListener(listener: OnSymbolDragListener) {
      onSymbolDragListeners.add(listener)
    }

    fun removeOnSymbolDragListener(listener: OnSymbolDragListener) {
      onSymbolDragListeners.remove(listener)
    }

    interface OnSymbolDragListener {
      fun onSymbolDragStarted(id: String)
      fun onSymbolDrag(id: String)
      fun onSymbolDragFinished(id: String)
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
}

private fun MapboxMap.queryRenderedSymbols(latLng: LatLng, layerId: String): List<Feature> {
  return this.queryRenderedFeatures(this.projection.toScreenLocation(latLng), layerId)
}