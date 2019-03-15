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
    val currentPosition = viewPager!!.currentItem
    for (i in 0 until FRAGMENT_COUNT) {
      if (Math.abs(i - currentPosition) <= 1) {
        viewPager.adapter?.instantiateItem(viewPager, 0)
        break
      }
    }
  }

  internal class MapFragmentAdapter(fragmentManager: FragmentManager) : FragmentStatePagerAdapter(fragmentManager) {

    override fun getCount(): Int {
      return FRAGMENT_COUNT
    }

    override fun getItem(position: Int): Fragment {
      return when (position) {
        0 -> createFragment(Style.MAPBOX_STREETS, LatLng(34.920526, 102.634774))
        1 -> createFragment(Style.DARK, LatLng(62.326440, 92.764913))
        2 -> createFragment(Style.SATELLITE, LatLng(-25.007786, 133.623852))
        3 -> createFragment(Style.LIGHT, LatLng(-19.922070, -43.939676))
        4 -> createFragment(Style.OUTDOORS, LatLng(46.881020, 9.098098))
        else -> createFragment(Style.TRAFFIC_DAY, LatLng(40.722517, -74.000029))
      }
    }

    private fun createFragment(@Style.StyleUrl style: String, latLng: LatLng): Fragment {
      val options = MapboxMapOptions().camera(CameraPosition.Builder().target(latLng).zoom(3.0).build())
      val fragment = SupportMapFragment.newInstance(options)
      fragment.getMapAsync { mapboxMap -> mapboxMap.setStyle(style) }
      return fragment
    }

    override fun getPageTitle(position: Int): CharSequence? {
      return "Page $position"
    }
  }

  companion object {
    const val FRAGMENT_COUNT = 6
  }
}

