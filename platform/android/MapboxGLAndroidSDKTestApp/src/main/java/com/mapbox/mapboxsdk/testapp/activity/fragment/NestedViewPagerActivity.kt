package com.mapbox.mapboxsdk.testapp.activity.fragment

import android.annotation.SuppressLint
import android.content.Context
import android.os.Bundle
import android.support.v4.app.Fragment
import android.support.v4.app.FragmentManager
import android.support.v4.app.FragmentStatePagerAdapter
import android.support.v4.view.ViewPager
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.MapboxMapOptions
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.maps.SupportMapFragment
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_recyclerview.*

/**
 * TestActivity showcasing how to integrate a MapView in a RecyclerView.
 * <p>
 * It requires calling the correct lifecycle methods when detaching and attaching the View to
 * the RecyclerView with onViewAttachedToWindow and onViewDetachedFromWindow.
 * </p>
 */
@SuppressLint("ClickableViewAccessibility")
class NestedViewPagerActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_recyclerview)
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = ItemAdapter(this, LayoutInflater.from(this), supportFragmentManager)
    }

    class ItemAdapter(private val context: Context, private val inflater: LayoutInflater, private val fragmentManager: FragmentManager) : RecyclerView.Adapter<RecyclerView.ViewHolder>() {

        private val items = listOf(
                "one", "two", "three", ViewPagerItem(), "four", "five", "six", "seven", "eight", "nine", "ten",
                "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen",
                "nineteen", "twenty", "twenty-one"
        )

        private var mapHolder: ViewPagerHolder? = null

        companion object {
            const val TYPE_VIEWPAGER = 0
            const val TYPE_TEXT = 1
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): RecyclerView.ViewHolder {
            return if (viewType == TYPE_VIEWPAGER) {
                val viewPager = inflater.inflate(R.layout.item_viewpager, parent, false) as ViewPager
                mapHolder = ViewPagerHolder(context, viewPager, fragmentManager)
                return mapHolder as ViewPagerHolder
            } else {
                TextHolder(inflater.inflate(android.R.layout.simple_list_item_1, parent, false) as TextView)
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
            return if (items[position] is ViewPagerItem) {
                TYPE_VIEWPAGER
            } else {
                TYPE_TEXT
            }
        }

        class TextHolder(val textView: TextView) : RecyclerView.ViewHolder(textView) {
            fun bind(item: String) {
                textView.text = item
            }
        }

        class ViewPagerItem
        class ViewPagerHolder(context: Context, private val viewPager: ViewPager, fragmentManager: FragmentManager) : RecyclerView.ViewHolder(viewPager) {
            init {
                viewPager.adapter = MapPagerAdapter(context, fragmentManager)
                viewPager.setOnTouchListener { view, motionEvent ->
                    // Disallow the touch request for recyclerView scroll
                    view.parent.requestDisallowInterceptTouchEvent(true)
                    viewPager.onTouchEvent(motionEvent)
                    false
                }
            }
        }

        class MapPagerAdapter(private val context: Context, fm: FragmentManager?) : FragmentStatePagerAdapter(fm) {

            override fun getItem(position: Int): Fragment {
                val options = MapboxMapOptions.createFromAttributes(context)
                options.textureMode(true)
                options.doubleTapGesturesEnabled(false)
                options.rotateGesturesEnabled(false)
                options.tiltGesturesEnabled(false)
                options.scrollGesturesEnabled(false)
                options.zoomGesturesEnabled(false)
                when (position) {
                    0 -> {
                        options.camera(CameraPosition.Builder().target(LatLng(34.920526, 102.634774)).zoom(3.0).build())
                        val fragment = SupportMapFragment.newInstance(options)
                        fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.MAPBOX_STREETS) }
                        return fragment
                    }
                    1 -> {
                        return EmptyFragment.newInstance()
                    }
                    2 -> {
                        options.camera(CameraPosition.Builder().target(LatLng(62.326440, 92.764913)).zoom(3.0).build())
                        val fragment = SupportMapFragment.newInstance(options)
                        fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.DARK) }
                        return fragment
                    }
                    3 -> {
                        return EmptyFragment.newInstance()
                    }
                    4 -> {
                        options.camera(CameraPosition.Builder().target(LatLng(-25.007786, 133.623852)).zoom(3.0).build())
                        val fragment = SupportMapFragment.newInstance(options)
                        fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.SATELLITE) }
                        return fragment
                    }
                    5 -> {
                        return EmptyFragment.newInstance()
                    }
                }
                throw IllegalAccessError()
            }

            override fun getCount(): Int {
                return 6
            }
        }

        class EmptyFragment : Fragment() {
            companion object {
                fun newInstance(): EmptyFragment {
                    return EmptyFragment()
                }
            }

            override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View {
                val textView = TextView(inflater.context)
                textView.text = "This is an empty Fragment"
                return textView
            }
        }
    }
}