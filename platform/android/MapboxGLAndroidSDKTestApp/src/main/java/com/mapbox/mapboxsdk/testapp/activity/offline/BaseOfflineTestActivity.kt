package com.mapbox.mapboxsdk.testapp.activity.offline

import android.os.Bundle
import android.os.Handler
import android.support.v7.app.AppCompatActivity
import android.widget.TextView
import com.mapbox.mapboxsdk.offline.OfflineManager
import com.mapbox.mapboxsdk.offline.OfflineRegion
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.coroutines.*
import timber.log.Timber
import kotlin.coroutines.CoroutineContext
import kotlin.coroutines.resume

abstract class BaseOfflineTestActivity : AppCompatActivity(), CoroutineScope {

  protected lateinit var handler: Handler
  protected lateinit var offlineManager: OfflineManager

  private var job = SupervisorJob()
  override val coroutineContext: CoroutineContext
    get() = Dispatchers.Main + job

  private val logView: TextView by lazy {
    findViewById<TextView>(R.id.log_text)
  }

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)

    handler = Handler()
    offlineManager = OfflineManager.getInstance(this)
  }

  override fun onDestroy() {
    handler.removeCallbacksAndMessages(null)
    coroutineContext.cancel()
    super.onDestroy()
  }

  protected fun logMessage(message: String) {
    Timber.d(message)
    logView.append(message)
    logView.append("\n")
  }

  fun deleteOldOfflineRegions(onCompleted: () -> Unit) {
    offlineManager.listOfflineRegions(object : OfflineManager.ListOfflineRegionsCallback {
      override fun onList(offlineRegions: Array<out OfflineRegion>) {
        val count = offlineRegions.size
        var remainingCount = count
        if (count > 0) {
          logMessage("Deleting $count old regions...")
          offlineRegions.forEach {
            it.delete(object : OfflineRegion.OfflineRegionDeleteCallback {
              override fun onDelete() {
                Timber.d("Deleted region with id ${it.id}")
                onProcessed()
              }

              override fun onError(error: String) {
                Timber.e("Failed to delete region: $error")
                onProcessed()
              }

              private fun onProcessed() {
                remainingCount--
                if (remainingCount == 0) {
                  logMessage("Done deleting")
                  onCompleted()
                }
              }
            })
          }
        } else {
          onCompleted()
        }
      }

      override fun onError(error: String) {
        logMessage("Failed to list offline regions: $error")
        onCompleted()
      }
    })
  }

  suspend fun deleteOldOfflineRegions() {
    return suspendCancellableCoroutine { continuation ->
      deleteOldOfflineRegions { continuation.resume(Unit) }
    }
  }
}