package com.mapbox.mapboxsdk.testapp.activity.location

import android.annotation.SuppressLint
import android.app.Fragment
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import com.mapbox.android.core.permissions.PermissionsListener
import com.mapbox.android.core.permissions.PermissionsManager
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.log.Logger
import com.mapbox.mapboxsdk.maps.MapView
import com.mapbox.mapboxsdk.offline.OfflineManager
import com.mapbox.mapboxsdk.offline.OfflineRegion
import com.mapbox.mapboxsdk.testapp.R

class LocationFragmentActivity : AppCompatActivity() {
  private lateinit var permissionsManager: PermissionsManager

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_location_layer_fragment)
    supportActionBar?.setDisplayHomeAsUpEnabled(true)

    if (savedInstanceState == null) {
      if (PermissionsManager.areLocationPermissionsGranted(this)) {
        addLocationFragment()
      } else {
        permissionsManager = PermissionsManager(object : PermissionsListener {
          override fun onExplanationNeeded(permissionsToExplain: MutableList<String>?) {
            Toast.makeText(this@LocationFragmentActivity, "You need to accept location permissions.",
              Toast.LENGTH_SHORT).show()
          }

          override fun onPermissionResult(granted: Boolean) {
            if (granted) {
              addLocationFragment()
            } else {
              finish()
            }
          }
        })
        permissionsManager.requestLocationPermissions(this)
      }
    }
  }

  private fun addLocationFragment() {
    fragmentManager
      .beginTransaction()
      .replace(R.id.container, LocationFragment.newInstance(), LocationFragment.FRAGMENT_TAG)
      .commit()
  }

  override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
    super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    permissionsManager.onRequestPermissionsResult(requestCode, permissions, grantResults)
  }

  class LocationFragment : Fragment() {
    companion object {
      const val FRAGMENT_TAG = "LFragment"
      private const val LOG_TAG = "Mbgl-LocationFragment"
      private const val TEST_DB_FILE_PATH = "/storage/emulated/0/Download/local_db.db"
      private const val TEST_STYLE_FILE_PATH = "file:///storage/emulated/0/Download/local_style.json"

      fun newInstance(): LocationFragment {
        return LocationFragment()
      }
    }

    private lateinit var mapView: MapView

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
      mapView = MapView(inflater.context)
      return mapView
    }

    @SuppressLint("MissingPermission")
    override fun onViewCreated(view: View?, savedInstanceState: Bundle?) {
      mapView.onCreate(savedInstanceState)

      mapView.setStyleUrl(TEST_STYLE_FILE_PATH)

      mapView.getMapAsync { mapboxMap ->
        mapboxMap.locationComponent.activateLocationComponent(activity)
        mapboxMap.locationComponent.isLocationComponentEnabled = true

        mapboxMap.addOnCameraMoveListener {
          Logger.d(LOG_TAG, String.format("camera: %s", mapboxMap.cameraPosition))
        }
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(48.11, 11.5), 9.0))
      }

      mergeDb()
    }

    private fun mergeDb() {
      OfflineManager.getInstance(activity).mergeOfflineRegions(TEST_DB_FILE_PATH,
        object : OfflineManager.MergeOfflineRegionsCallback {
          override fun onMerge(offlineRegions: Array<OfflineRegion>) {
            mapView.setStyleUrl(TEST_STYLE_FILE_PATH)
            Toast.makeText(
              activity,
              String.format("Merged %d regions.", offlineRegions.size),
              Toast.LENGTH_LONG).show()
            Logger.e(LOG_TAG, "success")
          }

          override fun onError(error: String) {
            Toast.makeText(
              activity,
              String.format("Offline DB merge error."),
              Toast.LENGTH_LONG).show()
            Logger.e(LOG_TAG, error)
          }
        })
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
    }
  }
}
