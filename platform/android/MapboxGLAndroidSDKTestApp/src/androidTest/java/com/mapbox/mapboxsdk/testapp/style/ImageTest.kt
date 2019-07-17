package com.mapbox.mapboxsdk.testapp.style

import android.graphics.Bitmap
import android.graphics.drawable.BitmapDrawable
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.testapp.R
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction
import com.mapbox.mapboxsdk.testapp.activity.EspressoTest
import java.util.*
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith

/**
 * CRUD tests around Image
 */
@RunWith(AndroidJUnit4::class)
class ImageTest : EspressoTest() {

  companion object {
    private const val IMAGE_ID = "test.image"
  }

  @Test
  fun testAddGetImage() {
    validateTestSetup()
    MapboxMapAction.invoke(mapboxMap) { uiController, mapboxMap ->
      val drawable = rule.activity.resources.getDrawable(R.drawable.ic_launcher_round)
      assertTrue(drawable is BitmapDrawable)

      val bitmapSet = (drawable as BitmapDrawable).bitmap
      mapboxMap.style!!.addImage(IMAGE_ID, bitmapSet)

      // adding an image requires converting the image with an asynctask
      uiController.loopMainThreadForAtLeast(200)

      val bitmapGet = mapboxMap.style!!.getImage(IMAGE_ID)
      assertTrue(bitmapGet!!.similarTo(bitmapSet))

      mapboxMap.style!!.removeImage(IMAGE_ID)
      assertNull(mapboxMap.style!!.getImage(IMAGE_ID))
    }
  }
}

/**
 * Alternative implementation of Bitmap.sameAs #14060
 */
fun Bitmap.similarTo(other: Bitmap): Boolean {
  if (invalidConfig(other)) {
    return false
  }

  // Allocate arrays
  val argb = IntArray(width * height)
  val argbOther = IntArray(other.width * other.height)
  getPixels(argb, 0, width, 0, 0, width, height)
  other.getPixels(argbOther, 0, width, 0, 0, width, height)

  // Alpha channel special check
  if (config == Bitmap.Config.ALPHA_8) {
    // in this case we have to manually compare the alpha channel as the rest is garbage.
    val length = width * height
    for (i in 0 until length) {
      if (argb[i] and -0x1000000 != argbOther[i] and -0x1000000) {
        return false
      }
    }
    return true
  }
  return Arrays.equals(argb, argbOther)
}

fun Bitmap.invalidConfig(other: Bitmap): Boolean = this.config != other.config || this.width != other.width || this.height != other.height