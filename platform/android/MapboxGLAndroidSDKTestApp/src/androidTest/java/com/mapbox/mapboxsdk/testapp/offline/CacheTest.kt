package com.mapbox.mapboxsdk.testapp.offline

import android.content.Context
import android.support.test.rule.ActivityTestRule
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.offline.OfflineManager
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity
import java.util.concurrent.CountDownLatch
import org.junit.Assert
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class CacheTest {

  @Rule
  @JvmField
  var rule = ActivityTestRule(FeatureOverviewActivity::class.java)

  private val context: Context by lazy { rule.activity }

  private val countDownLatch = CountDownLatch(1)

  @Test
  fun testSetMaximumAmbientCacheSize() {
    rule.activity.runOnUiThread {
      OfflineManager.getInstance(context).setMaximumAmbientCacheSize(10000000, object : OfflineManager.FileSourceCallback {
        override fun onSuccess() {
          countDownLatch.countDown()
        }

        override fun onError(message: String) {
          Assert.assertNull("onError should not be called", message)
        }
      })
    }
    countDownLatch.await()
  }

  @Test
  fun testSetClearAmbientCache() {
    rule.activity.runOnUiThread {
      OfflineManager.getInstance(context).clearAmbientCache(object : OfflineManager.FileSourceCallback {
        override fun onSuccess() {
          countDownLatch.countDown()
        }

        override fun onError(message: String) {
          Assert.assertNull("onError should not be called", message)
        }
      })
    }
    countDownLatch.await()
  }

  @Test
  fun testSetInvalidateAmbientCache() {
    rule.activity.runOnUiThread {
      OfflineManager.getInstance(context).invalidateAmbientCache(object : OfflineManager.FileSourceCallback {
        override fun onSuccess() {
          countDownLatch.countDown()
        }

        override fun onError(message: String) {
          Assert.assertNull("onError should not be called", message)
        }
      })
    }
    countDownLatch.await()
  }

  @Test
  fun testSetResetDatabase() {
    rule.activity.runOnUiThread {
      OfflineManager.getInstance(context).resetDatabase(object : OfflineManager.FileSourceCallback {
        override fun onSuccess() {
          countDownLatch.countDown()
        }

        override fun onError(message: String) {
          Assert.assertNull("onError should not be called", message)
        }
      })
    }
    countDownLatch.await()
  }
}