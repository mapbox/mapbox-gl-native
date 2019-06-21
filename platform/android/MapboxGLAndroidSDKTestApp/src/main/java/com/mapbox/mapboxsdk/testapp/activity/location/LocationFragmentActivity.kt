package com.mapbox.mapboxsdk.testapp.activity.location

import android.annotation.SuppressLint
import android.app.Fragment
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import android.widget.Toast
import com.mapbox.android.core.location.LocationEngineCallback
import com.mapbox.android.core.location.LocationEngineResult
import com.mapbox.android.core.permissions.PermissionsListener
import com.mapbox.android.core.permissions.PermissionsManager
import com.mapbox.geojson.Point
import com.mapbox.geojson.shifter.CoordinateShifter
import com.mapbox.geojson.shifter.CoordinateShifterManager
import com.mapbox.mapboxsdk.location.LocationComponentActivationOptions
import com.mapbox.mapboxsdk.location.modes.CameraMode.TRACKING
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_location_layer_fragment.*
import java.util.ArrayList

class LocationFragmentActivity : AppCompatActivity() {
  private lateinit var permissionsManager: PermissionsManager
  private var shift = false
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_location_layer_fragment)

    fab.setOnClickListener {
      val fragment = fragmentManager.findFragmentByTag(EmptyFragment.TAG)
      if (fragment == null) {
        fragmentManager
          .beginTransaction()
          .replace(R.id.container, EmptyFragment.newInstance(), EmptyFragment.TAG)
          .addToBackStack("transaction2")
          .commit()
      } else {
        this.onBackPressed()
      }
    }

    shifter.setOnClickListener{
      if(!shift) {
        Toast.makeText(this, "Shifter is on.",Toast.LENGTH_LONG).show()
        CoordinateShifterManager.setCoordinateShifter(coordinateShifter())
      }else{
        Toast.makeText(this, "Shifter is off.",Toast.LENGTH_LONG).show()
        CoordinateShifterManager.setCoordinateShifter(null)
      }
      shift = !shift
    }

    supportActionBar?.setDisplayHomeAsUpEnabled(true)

    if (PermissionsManager.areLocationPermissionsGranted(this)) {
      if (savedInstanceState == null) {
        fragmentManager
          .beginTransaction()
          .replace(R.id.container, LocationFragment.newInstance(), LocationFragment.TAG)
          .commit()
      }
    } else {
      permissionsManager = PermissionsManager(object : PermissionsListener {
        override fun onExplanationNeeded(permissionsToExplain: MutableList<String>?) {
          Toast.makeText(this@LocationFragmentActivity, "You need to accept location permissions.",
            Toast.LENGTH_SHORT).show()
        }

        override fun onPermissionResult(granted: Boolean) {
          if (granted) {
            if (savedInstanceState == null) {
              fragmentManager
                .beginTransaction()
                .replace(R.id.container, LocationFragment.newInstance(), LocationFragment.TAG)
                .commit()
            }
          } else {
            finish()
          }
        }
      })
      permissionsManager.requestLocationPermissions(this)
    }
  }

  private fun coordinateShifter(): CoordinateShifter {
    return object : CoordinateShifter {
      override fun shiftLonLat(lon: Double, lat: Double): List<Double> {
        return shift(lon, lat)
      }

      override fun shiftLonLatAlt(lon: Double, lat: Double, altitude: Double): List<Double> {
        val shiftedLongLat = shift(lon, lat)
        shiftedLongLat.add(altitude)
        return shiftedLongLat
      }

      override fun unshiftPoint(shiftedPoint: Point): List<Double> {
        val shiftedCoordinates = ArrayList<Double>(2)
        shiftedCoordinates.add(shiftedPoint.longitude())
        shiftedCoordinates.add(shiftedPoint.latitude())
        return shiftedCoordinates
      }

      override fun unshiftPoint(shiftedCoordinates: List<Double>): List<Double> {
        // Left empty on purpose
        return shiftedCoordinates
      }

      private fun shift(lon: Double, lat: Double): MutableList<Double> {
        val shiftedCoordinates = ArrayList<Double>(3)
        val shiftedLon = lon + 0.5
        val shiftedLat = lat + 0.5
        shiftedCoordinates.add(shiftedLon)
        shiftedCoordinates.add(shiftedLat)
        return shiftedCoordinates
      }
    }
  }

  override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
    super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    permissionsManager.onRequestPermissionsResult(requestCode, permissions, grantResults)
  }

  class LocationFragment : Fragment(), LocationEngineCallback<LocationEngineResult> {
    companion object {
      const val TAG = "LFragment"
      fun newInstance(): LocationFragment {
        return LocationFragment()
      }
    }

    private lateinit var mapView: MapView
    private lateinit var mapboxMap: MapboxMap

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
      mapView = MapView(inflater.context)
      return mapView
    }

    @SuppressLint("MissingPermission")
    override fun onViewCreated(view: View?, savedInstanceState: Bundle?) {
      mapView.onCreate(savedInstanceState)
      mapView.getMapAsync {
        mapboxMap = it
        mapboxMap.setMinZoomPreference(12.0)
        it.setStyle(Style.MAPBOX_STREETS) { style ->
          val component = mapboxMap.locationComponent

          component.activateLocationComponent(LocationComponentActivationOptions
                  .builder(activity, style)
                  .useDefaultLocationEngine(true)
                  .build())

          component.isLocationComponentEnabled = true
          component.locationEngine?.getLastLocation(this)
          component.cameraMode = TRACKING
        }
      }
    }

    override fun onSuccess(result: LocationEngineResult?) {
//        mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(LatLng(result?.lastLocation), 12.0))
    }

    override fun onFailure(exception: Exception) {
      //noop
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

    override fun onSaveInstanceState(outState: Bundle) {
      super.onSaveInstanceState(outState)
      mapView.onSaveInstanceState(outState)
    }

    override fun onStop() {
      super.onStop()
      mapView.onStop()
    }

    override fun onLowMemory() {
      super.onLowMemory()
      mapView.onLowMemory()
    }

    override fun onDestroyView() {
      super.onDestroyView()
      mapView.onDestroy()
      CoordinateShifterManager.setCoordinateShifter(null)
    }
  }

  class EmptyFragment : Fragment() {
    companion object {
      const val TAG = "EmptyFragment"
      fun newInstance(): EmptyFragment {
        return EmptyFragment()
      }
    }

    override fun onCreateView(inflater: LayoutInflater?, container: ViewGroup?, savedInstanceState: Bundle?): View {
      val textView = TextView(inflater?.context)
      textView.text = "This is an empty Fragment"
      return textView
    }
  }
}
