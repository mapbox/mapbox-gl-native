package com.mapbox.mapboxsdk.location

import android.animation.ValueAnimator
import io.mockk.every
import io.mockk.mockk
import io.mockk.verify
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class MapboxAnimatorTest {

  @Test
  fun fps_unlimited() {
    val valueAnimator = mockk<ValueAnimator>()
    every { valueAnimator.animatedValue } answers { 5f }
    val listener = mockk<MapboxAnimator.AnimationsValueChangeListener<Float>>()
    every { listener.onNewAnimationValue(any()) } answers {}
    val mapboxAnimator = MapboxFloatAnimator(0f, 10f, listener, Int.MAX_VALUE)

    for (i in 0 until 5)
      mapboxAnimator.onAnimationUpdate(valueAnimator)

    verify(exactly = 5) { listener.onNewAnimationValue(5f) }
  }

  @Test
  fun fps_limited() {
    val valueAnimator = mockk<ValueAnimator>()
    every { valueAnimator.animatedValue } answers { 5f }
    val listener = mockk<MapboxAnimator.AnimationsValueChangeListener<Float>>()
    every { listener.onNewAnimationValue(any()) } answers {}
    val mapboxAnimator = MapboxFloatAnimator(0f, 10f, listener, 5)

    for (i in 0 until 5) {
      mapboxAnimator.onAnimationUpdate(valueAnimator)
      Thread.sleep(150)
    }

    verify(exactly = 3) { listener.onNewAnimationValue(5f) }
  }
}