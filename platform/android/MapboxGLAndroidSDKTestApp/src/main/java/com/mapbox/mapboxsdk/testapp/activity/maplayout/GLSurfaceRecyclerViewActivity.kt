package com.mapbox.mapboxsdk.testapp.activity.maplayout

import android.annotation.SuppressLint
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.TextView
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_recyclerview.*

/**
 * TestActivity showcasing how to integrate a GLSurfaceView MapView in a RecyclerView.
 * <p>
 * It requires calling the correct lifecycle methods when detaching and attaching the View to
 * the RecyclerView with onViewAttachedToWindow and onViewDetachedFromWindow.
 * </p>
 */
@SuppressLint("ClickableViewAccessibility")
open class GLSurfaceRecyclerViewActivity : AppCompatActivity() {

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_recyclerview)
    recyclerView.layoutManager = LinearLayoutManager(this)
    recyclerView.adapter = ItemAdapter(this, LayoutInflater.from(this), savedInstanceState)
  }

  override fun onSaveInstanceState(outState: Bundle?) {
    super.onSaveInstanceState(outState)
    // to save state, we need to call MapView#onSaveInstanceState
    (recyclerView.adapter as ItemAdapter).onSaveInstanceState(outState)
  }

  override fun onLowMemory() {
    super.onLowMemory()
    // to release memory, we need to call MapView#onLowMemory
    (recyclerView.adapter as ItemAdapter).onLowMemory()
  }

  override fun onDestroy() {
    super.onDestroy()
    // to perform cleanup, we need to call MapView#onDestroy
    (recyclerView.adapter as ItemAdapter).onDestroy()
  }

  open fun getMapItemLayoutId(): Int {
    return R.layout.item_map_gl
  }

  class ItemAdapter(private val activity: GLSurfaceRecyclerViewActivity, private val inflater: LayoutInflater, val savedInstanceState: Bundle?) : RecyclerView.Adapter<RecyclerView.ViewHolder>() {

    private val items = listOf(
      "one", "two", "three", MapItem(), "four", "five", "six", "seven", "eight", "nine", "ten",
      "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen",
      "nineteen", "twenty", "twenty-one"
    )

    private var mapHolder: MapHolder? = null

    companion object {
      const val TYPE_MAP = 0
      const val TYPE_TEXT = 1
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): RecyclerView.ViewHolder {
      return if (viewType == TYPE_MAP) {
        val mapView = inflater.inflate(activity.getMapItemLayoutId(), parent, false) as MapView
        mapView.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.MAPBOX_STREETS) }
        mapHolder = MapHolder(mapView, savedInstanceState)
        return mapHolder as MapHolder
      } else {
        TextHolder(inflater.inflate(android.R.layout.simple_list_item_1, parent, false) as TextView)
      }
    }

    override fun onViewAttachedToWindow(holder: RecyclerView.ViewHolder) {
      super.onViewAttachedToWindow(holder)
      if (holder is MapHolder) {
        val mapView = holder.mapView
        mapView.onStart()
        mapView.onResume()
      }
    }

    override fun onViewDetachedFromWindow(holder: RecyclerView.ViewHolder) {
      super.onViewDetachedFromWindow(holder)
      if (holder is MapHolder) {
        val mapView = holder.mapView
        mapView.onPause()
        mapView.onStop()
      }
    }

    override fun getItemCount(): Int {
      return items.count()
    }

    override fun onBindViewHolder(holder: RecyclerView.ViewHolder, position: Int) {
      if (holder.itemViewType == TYPE_TEXT) {
        val textHolder = holder as TextHolder
        textHolder.bind(items[position] as String)
      }
    }

    override fun getItemViewType(position: Int): Int {
      return if (items[position] is MapItem) {
        TYPE_MAP
      } else {
        TYPE_TEXT
      }
    }

    fun onSaveInstanceState(savedInstanceState: Bundle?) {
      savedInstanceState?.let {
        mapHolder?.mapView?.onSaveInstanceState(it)
      }
    }

    fun onLowMemory() {
      mapHolder?.mapView?.onLowMemory()
    }

    fun onDestroy() {
      mapHolder?.mapView?.let {
        it.onPause()
        it.onStop()
        it.onDestroy()
      }
    }

    class MapItem
    class MapHolder(val mapView: MapView, bundle: Bundle?) : RecyclerView.ViewHolder(mapView) {
      init {
        mapView.onCreate(bundle)
        mapView.setOnTouchListener { view, motionEvent ->
          // Disallow the touch request for recyclerView scroll
          view.parent.requestDisallowInterceptTouchEvent(true)
          mapView.onTouchEvent(motionEvent)
          true
        }
      }
    }

    class TextHolder(val textView: TextView) : RecyclerView.ViewHolder(textView) {
      fun bind(item: String) {
        textView.text = item
      }
    }
  }
}
