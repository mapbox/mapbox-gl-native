package com.mapbox.mapboxsdk.testapp.activity.fragment

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.maps.SupportMapFragment
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_backstack_fragment.*

/**
 * Test activity showcasing using the MapFragment API as part of a backstacked fragment.
 */
class FragmentBackStackActivity : AppCompatActivity() {

    private lateinit var mapFragment: SupportMapFragment

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_backstack_fragment)

        mapFragment = SupportMapFragment.newInstance()
        mapFragment.getMapAsync { initMap(it) }

        supportFragmentManager.beginTransaction().apply {
            add(R.id.container, mapFragment)
        }.commit()

        button.setOnClickListener { handleClick(it) }
    }

    private fun initMap(mapboxMap: MapboxMap) {
        mapboxMap.setStyle(Style.SATELLITE) {
            mapboxMap.setPadding(300, 300, 300, 300)
        }
    }

    private fun handleClick(button: View) {
        supportFragmentManager.beginTransaction().apply {
            replace(R.id.container, NestedViewPagerActivity.ItemAdapter.EmptyFragment())
            addToBackStack("map_empty_fragment")
        }.commit()
    }
}