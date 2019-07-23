package com.mapbox.mapboxsdk.testapp.activity.fragment

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.maps.SupportMapFragment
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.utils.NavUtils
import kotlinx.android.synthetic.main.activity_backstack_fragment.*

/**
 * Test activity showcasing using the MapFragment API as part of a backstacked fragment.
 */
class FragmentBackStackActivity : AppCompatActivity() {

  companion object {
    private const val FRAGMENT_TAG = "map_fragment"
  }

  private lateinit var mapFragment: SupportMapFragment

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_backstack_fragment)

    if (savedInstanceState == null) {
      mapFragment = SupportMapFragment.newInstance()
      mapFragment.getMapAsync { initMap(it) }

      supportFragmentManager.beginTransaction().apply {
        add(R.id.container, mapFragment, FRAGMENT_TAG)
      }.commit()
    } else {
      supportFragmentManager.findFragmentByTag(FRAGMENT_TAG)?.also { fragment ->
        if (fragment is SupportMapFragment) {
          fragment.getMapAsync { initMap(it) }
        }
      }
    }

    button.setOnClickListener { handleClick() }
  }

  private fun initMap(mapboxMap: MapboxMap) {
    mapboxMap.setStyle(Style.SATELLITE) {
      mapboxMap.setPadding(300, 300, 300, 300)
    }
  }

  private fun handleClick() {
    supportFragmentManager.beginTransaction().apply {
      replace(R.id.container, NestedViewPagerActivity.ItemAdapter.EmptyFragment())
      addToBackStack("map_empty_fragment")
    }.commit()
  }

  override fun onBackPressed() {
    if (supportFragmentManager.backStackEntryCount == 0) {
      // activity uses singleInstance for testing purposes
      // code below provides a default navigation when using the app
      NavUtils.navigateHome(this)
    } else {
      super.onBackPressed()
    }
  }
}