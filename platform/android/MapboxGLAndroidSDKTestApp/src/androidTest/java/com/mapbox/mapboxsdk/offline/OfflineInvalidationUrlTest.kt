package com.mapbox.mapboxsdk.offline

import android.support.test.espresso.UiController
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory
import com.mapbox.mapboxsdk.geometry.LatLng
import com.mapbox.mapboxsdk.log.Logger
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.module.http.HttpRequestUtil
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke
import com.mapbox.mapboxsdk.testapp.activity.BaseTest
import com.mapbox.mapboxsdk.testapp.activity.espresso.PixelTestActivity
import okhttp3.Interceptor
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import org.junit.Assert.assertTrue
import org.junit.Test
import java.io.IOException
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import java.util.concurrent.TimeoutException

class OfflineInvalidationUrlTest : BaseTest(), OfflineRegion.OfflineRegionObserver {

  private lateinit var offlineRegion: OfflineRegion
  private val countDownLatch = CountDownLatch(1)

  override fun getActivityClass(): Class<*> {
    return PixelTestActivity::class.java
  }

  override
  fun beforeTest() {
    super.beforeTest()
    mapView = (rule.activity as PixelTestActivity).mapView
  }

  @Test
  fun offlineInvalidationRequestUrlTest() {
    OfflineManager.getInstance(rule.activity).createOfflineRegion(
      createTestRegionDefinition(),
      ByteArray(0),
      object : OfflineManager.CreateOfflineRegionCallback {
        override fun onCreate(region: OfflineRegion?) {
          region?.let {
            offlineRegion = it
            offlineRegion.setDownloadState(OfflineRegion.STATE_ACTIVE)
            offlineRegion.setObserver(this@OfflineInvalidationUrlTest)
            offlineRegion.invalidate(
              object : OfflineRegion.OfflineRegionInvalidateCallback {
                override fun onInvalidate() {
                  validateTestSetup()
                  invoke(mapboxMap) { _: UiController, mapboxMap: MapboxMap ->

                    mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(LatLng(
                      getTestRegionDefinitionPoint().latitude(),
                      getTestRegionDefinitionPoint().longitude()),
                      getTestRegionDefinitionZoom()))

                    // TODO: validate that the tile request includes a SKU token and doesn't include offline=true

                    val okHttpClient = OkHttpClient.Builder()
                      .addNetworkInterceptor(TileRequestUrlInterceptor())
                      .build()
                    HttpRequestUtil.setOkHttpClient(okHttpClient)
                  }
                }

                override fun onError(error: String?) {
                  Logger.e(TAG, "Error while invalidating offline region: $error")
                }
              })
          }
        }

        override fun onError(error: String?) {
          Logger.e(TAG, "Error while creating offline region: $error")
        }
      })

    if (!countDownLatch.await(30, TimeUnit.SECONDS)) {
      throw TimeoutException()
    }
  }

  /**
   * This Interceptor allows the capturing of the tile request URL
   */
  internal class TileRequestUrlInterceptor : Interceptor {

    @Throws(IOException::class)
    override fun intercept(chain: Interceptor.Chain): Response {
      val requestUrl = getUrl(chain.request())
      logRequestUrl(requestUrl)
      assertTrue(!requestUrl.contains("offline=true"))
      return chain.proceed(chain.request())
    }

    private fun logRequestUrl(urlRequest: String) {
      Logger.i(TAG, urlRequest)
    }

    companion object {
      private fun getUrl(request: Request): String {
        val url = request.url().toString()
        return url.substring(0, url.indexOf('?'))
      }
    }
  }

  override fun onStatusChanged(status: OfflineRegionStatus?) {
    status?.let {
      if (it.isComplete) {
        offlineRegion.setDownloadState(OfflineRegion.STATE_INACTIVE)
        countDownLatch.countDown()
      }
    }
  }

  override fun onError(error: OfflineRegionError?) {
    Logger.e(TAG, "Error while downloading offline region: $error")
  }

  override fun mapboxTileCountLimitExceeded(limit: Long) {
    Logger.e(TAG, "Tile count limited exceeded: $limit")
  }

  private fun createTestRegionDefinition(): OfflineRegionDefinition {
    return OfflineGeometryRegionDefinition(
      Style.MAPBOX_STREETS,
      getTestRegionDefinitionPoint(),
      getTestRegionDefinitionZoom(),
      getTestRegionDefinitionZoom(),
      1.0f,
      false
    )
  }

  fun getTestRegionDefinitionPoint(): Point {
    return Point.fromLngLat(50.847857, 4.360137)
  }

  fun getTestRegionDefinitionZoom(): Double {
    return 17.0
  }

  companion object {
    const val TAG = "OfflineInvalidationUrlTest"
  }
}