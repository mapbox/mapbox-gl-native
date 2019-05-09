package com.mapbox.mapboxsdk.testapp.storage

import android.app.Activity
import android.support.annotation.WorkerThread
import com.mapbox.mapboxsdk.storage.FileSource
import junit.framework.Assert
import java.io.File
import java.util.concurrent.CountDownLatch

class FileSourceTestUtils(private val activity: Activity) {
  val originalPath = FileSource.getResourcesCachePath(activity)
  val testPath = "$originalPath/test"

  fun setup() {
    val testFile = File(testPath)
    testFile.mkdirs()
  }

  @WorkerThread
  fun cleanup() {
    val currentPath = FileSource.getResourcesCachePath(activity)
    if (currentPath != originalPath) {
      changePath(originalPath)
    }
    val testFile = File(testPath)
    if (testFile.exists()) {
      testFile.deleteRecursively()
    }
  }

  @WorkerThread
  fun changePath(path: String) {
    val latch = CountDownLatch(1)
    activity.runOnUiThread {
      FileSource.setResourcesCachePath(
        path,
        object : FileSource.ResourcesCachePathChangeCallback {
          override fun onSuccess(path: String) {
            latch.countDown()
          }

          override fun onError(message: String) {
            Assert.fail("Resource path change failed - path: $path, message: $message")
          }
        })
    }
    latch.await()
  }
}