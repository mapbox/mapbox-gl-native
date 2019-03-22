package com.mapbox.mapboxsdk.testapp.activity.fragment

import android.os.Bundle
import android.support.v4.app.Fragment
import android.support.v4.app.FragmentManager
import android.support.v4.app.FragmentStatePagerAdapter
import android.support.v7.app.AppCompatActivity
import com.mapbox.mapboxsdk.camera.CameraPosition
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.maps.MapboxMapOptions
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.maps.SupportMapFragment
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_viewpager.*

/**
 * Test activity showcasing using the Android SDK ViewPager API to show MapFragments.
 */
class ViewPagerActivity : AppCompatActivity() {

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_viewpager)
    viewPager.adapter = MapFragmentAdapter(supportFragmentManager)
  }

  override fun onRestoreInstanceState(savedInstanceState: Bundle) {
    super.onRestoreInstanceState(savedInstanceState)

    val currentPosition = viewPager.currentItem
    var mapFragment: SupportMapFragment

    if (Math.abs(0 - currentPosition) <= 1) {
      mapFragment = viewPager.adapter?.instantiateItem(viewPager, 0) as SupportMapFragment
      mapFragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.MAPBOX_STREETS) }
    }

    if (Math.abs(1 - currentPosition) <= 1) {
      mapFragment = viewPager.adapter?.instantiateItem(viewPager, 1) as SupportMapFragment
      mapFragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.DARK) }
    }

    if (Math.abs(2 - currentPosition) <= 1) {
      mapFragment = viewPager.adapter?.instantiateItem(viewPager, 2) as SupportMapFragment
      mapFragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.SATELLITE) }
    }

    if (Math.abs(3 - currentPosition) <= 1) {
      mapFragment = viewPager.adapter?.instantiateItem(viewPager, 3) as SupportMapFragment
      mapFragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.SATELLITE) }
    }

    if (Math.abs(4 - currentPosition) <= 1) {
      mapFragment = viewPager.adapter?.instantiateItem(viewPager, 4) as SupportMapFragment
      mapFragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.SATELLITE) }
    }
  }

  internal class MapFragmentAdapter(fragmentManager: FragmentManager) : FragmentStatePagerAdapter(fragmentManager) {

    override fun getCount(): Int {
      return NUM_ITEMS
    }

    override fun getItem(position: Int): Fragment? {
      var fragment: SupportMapFragment? = null
      val options = MapboxMapOptions()
      options.textureMode(true)

      when (position) {
        0 -> {
          options.camera(CameraPosition.Builder().target(LatLng(34.920526, 102.634774)).zoom(3.0).build())
          fragment = SupportMapFragment.newInstance(options)
          fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.MAPBOX_STREETS) }
        }
        1 -> {
          options.camera(CameraPosition.Builder().target(LatLng(62.326440, 92.764913)).zoom(3.0).build())
          fragment = SupportMapFragment.newInstance(options)
          fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.DARK) }
        }
        2 -> {
          options.camera(CameraPosition.Builder().target(LatLng(-25.007786, 133.623852)).zoom(3.0).build())
          fragment = SupportMapFragment.newInstance(options)
          fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.SATELLITE) }
        }
        3 -> {
          options.camera(CameraPosition.Builder().target(LatLng(62.326440, 92.764913)).zoom(3.0).build())
          fragment = SupportMapFragment.newInstance(options)
          fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.LIGHT) }
        }
        4 -> {
          options.camera(CameraPosition.Builder().target(LatLng(34.920526, 102.634774)).zoom(3.0).build())
          fragment = SupportMapFragment.newInstance(options)
          fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(Style.TRAFFIC_NIGHT) }
        }
      }
      return fragment
    }

    override fun getPageTitle(position: Int): CharSequence? {
      return "Page $position"
    }

    companion object {

      private val NUM_ITEMS = 5
    }
  }
}
