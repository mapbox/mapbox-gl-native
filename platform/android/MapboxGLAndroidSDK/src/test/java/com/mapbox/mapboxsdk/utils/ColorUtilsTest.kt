package com.mapbox.mapboxsdk.utils

import android.graphics.Color
import junit.framework.Assert
import org.junit.Test

class ColorUtilsTest {

  @Test
  fun rgbaToColor_decimalComponent() {
    val input = "rgba(255,128.0000952303,0,0.7)"
    val result = ColorUtils.rgbaToColor(input)
    Assert.assertEquals(Color.argb(255, 128, 0, (0.7 * 255).toInt()), result)
  }

  @Test
  fun rgbaToColor_decimalComponent_floor() {
    val input = "rgba(255,128.70123,0,0.7)"
    val result = ColorUtils.rgbaToColor(input)
    Assert.assertEquals(Color.argb(255, 128, 0, (0.7 * 255).toInt()), result)
  }
}