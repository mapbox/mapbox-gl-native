package com.mapbox.mapboxsdk.testapp.activity.storage

import android.os.Bundle
import android.os.Looper
import android.support.design.widget.Snackbar
import android.support.v7.app.AppCompatActivity
import com.mapbox.mapboxsdk.offline.OfflineManager
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_cache_management.*

/**
 * Test activity showcasing the cache management APIs
 */
class CacheManagementActivity : AppCompatActivity() {

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_cache_management)

    val fileSource = OfflineManager.getInstance(this)
    resetDatabaseButton.setOnClickListener {
      fileSource.resetDatabase(object : OfflineManager.FileSourceCallback {
        override fun onSuccess() {
          showSnackbar("Reset database success")
        }

        override fun onError(message: String) {
          showSnackbar("Reset database fail: $message")
        }
      })
    }

    invalidateAmbientCacheButton.setOnClickListener {
      fileSource.invalidateAmbientCache(object : OfflineManager.FileSourceCallback {
        override fun onSuccess() {
          showSnackbar("Invalidate ambient cache success")
        }

        override fun onError(message: String) {
          showSnackbar("Invalidate ambient cache fail: $message")
        }
      })
    }

    clearAmbientCacheButton.setOnClickListener {
      fileSource.clearAmbientCache(object : OfflineManager.FileSourceCallback {
        override fun onSuccess() {
          showSnackbar("Clear ambient cache success")
        }

        override fun onError(message: String) {
          showSnackbar("Clear ambient cache fail: $message")
        }
      })
    }

    setMaximumAmbientCacheSizeButton.setOnClickListener {
      fileSource.setMaximumAmbientCacheSize(5000000, object : OfflineManager.FileSourceCallback {
        override fun onSuccess() {
          showSnackbar("Set maximum ambient cache size success")
        }

        override fun onError(message: String) {
          showSnackbar("Set maximum ambient cache size fail: $message")
        }
      })
    }
  }

  fun showSnackbar(message: String) {
    // validate that all callbacks occur on main thread
    assert(Looper.myLooper() == Looper.getMainLooper())

    // show snackbar
    Snackbar.make(container, message, Snackbar.LENGTH_SHORT).show()
  }
}