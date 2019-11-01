package com.mapbox.mapboxsdk.maps

import android.graphics.PointF
import android.os.SystemClock
import android.support.test.espresso.Espresso
import android.support.test.espresso.InjectEventSecurityException
import android.support.test.espresso.UiController
import android.support.test.espresso.ViewAction
import android.support.test.espresso.matcher.ViewMatchers
import android.view.MotionEvent
import android.view.View
import org.hamcrest.Matcher

object GesturesUiTestUtils {

  private const val DEFAULT_GESTURE_DURATION = 500L

  fun pinch(
    startSpan: Float,
    endSpan: Float,
    center: PointF? = null,
    duration: Long = DEFAULT_GESTURE_DURATION
  ): ViewAction {
    return object : ViewAction {
      override fun getConstraints(): Matcher<View> {
        return ViewMatchers.isEnabled()
      }

      override fun getDescription(): String = "Pinch $startSpan -> $endSpan"

      override fun perform(uiController: UiController, view: View) {
        var middlePosition = center
        if (middlePosition == null) {
          middlePosition = getCenterPointF(view)
        }

        val startPoint1 = PointF(middlePosition.x - startSpan / 2f, middlePosition.y)
        val startPoint2 = PointF(middlePosition.x + startSpan / 2f, middlePosition.y)
        val endPoint1 = PointF(middlePosition.x - endSpan / 2f, middlePosition.y)
        val endPoint2 = PointF(middlePosition.x + endSpan / 2f, middlePosition.y)

        performPinch(uiController, startPoint1, startPoint2, endPoint1, endPoint2, duration)
      }
    }
  }

  fun quickScale(
    deltaY: Float,
    startPoint: PointF? = null,
    withVelocity: Boolean = true,
    duration: Long = DEFAULT_GESTURE_DURATION,
    interrupt: Boolean = false
  ): ViewAction {
    return object : ViewAction {
      override fun getConstraints(): Matcher<View> = ViewMatchers.isEnabled()

      override fun getDescription(): String = "quick scale ${deltaY}Y"

      override fun perform(uiController: UiController, view: View) {
        var middlePosition = startPoint
        if (middlePosition == null) {
          middlePosition = getCenterPointF(view)
        }

        val endPoint = PointF(middlePosition.x, middlePosition.y + deltaY)

        performQuickScale(uiController, middlePosition, endPoint, withVelocity, duration, interrupt)
      }
    }
  }

  fun move(
    deltaX: Float,
    deltaY: Float,
    startPoint: PointF? = null,
    withVelocity: Boolean = true,
    duration: Long = DEFAULT_GESTURE_DURATION
  ): ViewAction {
    return object : ViewAction {
      override fun getConstraints(): Matcher<View> = ViewMatchers.isEnabled()

      override fun getDescription(): String = "move ${deltaX}X, ${deltaY}Y"

      override fun perform(uiController: UiController, view: View) {
        var middlePosition = startPoint
        if (middlePosition == null) {
          middlePosition = getCenterPointF(view)
        }

        val endPoint = PointF(middlePosition.x + deltaX, middlePosition.y + deltaY)

        performMove(uiController, middlePosition, endPoint, withVelocity, duration)
      }
    }
  }

  private fun getCenterPointF(view: View): PointF {
    val locationOnScreen = IntArray(2)
    view.getLocationOnScreen(locationOnScreen)
    val viewHeight = view.height * view.scaleY
    val viewWidth = view.width * view.scaleX
    return PointF(
      (locationOnScreen[0] + viewWidth / 2).toInt().toFloat(),
      (locationOnScreen[1] + viewHeight / 2).toInt().toFloat()
    )
  }

  // https://stackoverflow.com/a/46443628/9126211
  private fun performPinch(
    uiController: UiController,
    startPoint1: PointF,
    startPoint2: PointF,
    endPoint1: PointF,
    endPoint2: PointF,
    duration: Long
  ) {
    val eventMinInterval: Long = 10
    val startTime = SystemClock.uptimeMillis()
    var eventTime = startTime
    var event: MotionEvent
    var eventX1: Float = startPoint1.x
    var eventY1: Float = startPoint1.y
    var eventX2: Float = startPoint2.x
    var eventY2: Float = startPoint2.y

    // Specify the property for the two touch points
    val properties = arrayOfNulls<MotionEvent.PointerProperties>(2)
    val pp1 = MotionEvent.PointerProperties()
    pp1.id = 0
    pp1.toolType = MotionEvent.TOOL_TYPE_FINGER
    val pp2 = MotionEvent.PointerProperties()
    pp2.id = 1
    pp2.toolType = MotionEvent.TOOL_TYPE_FINGER

    properties[0] = pp1
    properties[1] = pp2

    // Specify the coordinations of the two touch points
    // NOTE: you MUST set the pressure and size value, or it doesn't work
    val pointerCoords = arrayOfNulls<MotionEvent.PointerCoords>(2)
    val pc1 = MotionEvent.PointerCoords()
    pc1.x = eventX1
    pc1.y = eventY1
    pc1.pressure = 1f
    pc1.size = 1f
    val pc2 = MotionEvent.PointerCoords()
    pc2.x = eventX2
    pc2.y = eventY2
    pc2.pressure = 1f
    pc2.size = 1f
    pointerCoords[0] = pc1
    pointerCoords[1] = pc2

    /*
     * Events sequence of zoom gesture:
     *
     * 1. Send ACTION_DOWN event of one start point
     * 2. Send ACTION_POINTER_DOWN of two start points
     * 3. Send ACTION_MOVE of two middle points
     * 4. Repeat step 3 with updated middle points (x,y), until reach the end points
     * 5. Send ACTION_POINTER_UP of two end points
     * 6. Send ACTION_UP of one end point
     */

    try {
      // Step 1
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_DOWN, 1, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)

      // Step 2
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_POINTER_DOWN + (pp2.id shl MotionEvent.ACTION_POINTER_INDEX_SHIFT), 2,
        properties, pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)

      // Step 3, 4
      val moveEventNumber = duration / eventMinInterval

      val stepX1: Float
      val stepY1: Float
      val stepX2: Float
      val stepY2: Float

      stepX1 = (endPoint1.x - startPoint1.x) / moveEventNumber
      stepY1 = (endPoint1.y - startPoint1.y) / moveEventNumber
      stepX2 = (endPoint2.x - startPoint2.x) / moveEventNumber
      stepY2 = (endPoint2.y - startPoint2.y) / moveEventNumber

      for (i in 0 until moveEventNumber) {
        // Update the move events
        eventTime += eventMinInterval
        eventX1 += stepX1
        eventY1 += stepY1
        eventX2 += stepX2
        eventY2 += stepY2

        pc1.x = eventX1
        pc1.y = eventY1
        pc2.x = eventX2
        pc2.y = eventY2

        pointerCoords[0] = pc1
        pointerCoords[1] = pc2

        event = MotionEvent.obtain(
          startTime, eventTime,
          MotionEvent.ACTION_MOVE, 2, properties,
          pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
        )
        injectMotionEventToUiController(uiController, event)
      }

      // Step 5
      pc1.x = endPoint1.x
      pc1.y = endPoint1.y
      pc2.x = endPoint2.x
      pc2.y = endPoint2.y
      pointerCoords[0] = pc1
      pointerCoords[1] = pc2

      eventTime += eventMinInterval
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_POINTER_UP + (pp2.id shl MotionEvent.ACTION_POINTER_INDEX_SHIFT), 2, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)

      // Step 6
      eventTime += eventMinInterval
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_UP, 1, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)
    } catch (ex: InjectEventSecurityException) {
      throw RuntimeException("Could not perform pinch", ex)
    }
  }

  private fun performQuickScale(
    uiController: UiController,
    startPoint: PointF,
    endPoint: PointF,
    withVelocity: Boolean,
    duration: Long,
    interrupt: Boolean
  ) {
    val eventMinInterval: Long = 10
    val tapDownMinInterval: Long = 40 // ViewConfiguration#DOUBLE_TAP_MIN_TIME = 40
    val startTime = SystemClock.uptimeMillis()
    var eventTime = startTime
    var event: MotionEvent
    var eventX1: Float = startPoint.x
    var eventY1: Float = startPoint.y

    var properties = arrayOfNulls<MotionEvent.PointerProperties>(1)
    val pp1 = MotionEvent.PointerProperties()
    pp1.id = 0
    pp1.toolType = MotionEvent.TOOL_TYPE_FINGER
    properties[0] = pp1

    var pointerCoords = arrayOfNulls<MotionEvent.PointerCoords>(1)
    val pc1 = MotionEvent.PointerCoords()
    pc1.x = eventX1
    pc1.y = eventY1
    pc1.pressure = 1f
    pc1.size = 1f
    pointerCoords[0] = pc1

    /*
     * Events sequence of quick scale gesture:
     *
     * 1. Send ACTION_DOWN
     * 2. Send ACTION_UP
     * 3. Send ACTION_DOWN
     * 4. Send ACTION_MOVE with updated middle points (x,y), until reach the end points
     * 5. Send ACTION_UP of one end point
     */
    try {
      // Step 1
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_DOWN, 1, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)

      // Step 2
      eventTime += eventMinInterval
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_UP, 1, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)

      // Step 3
      eventTime += tapDownMinInterval
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_DOWN, 1, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)

      // Step 4
      val moveEventNumber = duration / eventMinInterval

      val stepX1: Float
      val stepY1: Float

      stepX1 = (endPoint.x - startPoint.x) / moveEventNumber
      stepY1 = (endPoint.y - startPoint.y) / moveEventNumber

      var interrupted = false

      for (i in 0 until moveEventNumber) {
        // Update the move events
        eventTime += eventMinInterval
        eventX1 += stepX1
        eventY1 += stepY1

        pc1.x = eventX1
        pc1.y = eventY1

        pointerCoords[0] = pc1

        event = MotionEvent.obtain(
          startTime, eventTime,
          MotionEvent.ACTION_MOVE, if (interrupted) 2 else 1, properties,
          pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
        )
        injectMotionEventToUiController(uiController, event)

        if (interrupt && i == moveEventNumber / 2) {
          // Specify the property for the two touch points
          properties = arrayOfNulls<MotionEvent.PointerProperties>(2)
          val pp2 = MotionEvent.PointerProperties()
          pp2.id = 1
          pp2.toolType = MotionEvent.TOOL_TYPE_FINGER

          properties[0] = pp1
          properties[1] = pp2

          // Specify the coordinations of the two touch points
          // NOTE: you MUST set the pressure and size value, or it doesn't work
          pointerCoords = arrayOfNulls<MotionEvent.PointerCoords>(2)
          val pc2 = MotionEvent.PointerCoords()
          pc2.x = startPoint.x
          pc2.y = startPoint.y
          pc2.pressure = 1f
          pc2.size = 1f
          pointerCoords[0] = pc1
          pointerCoords[1] = pc2

          eventTime += eventMinInterval
          event = MotionEvent.obtain(
            startTime, eventTime,
            MotionEvent.ACTION_POINTER_DOWN + (pp2.id shl MotionEvent.ACTION_POINTER_INDEX_SHIFT), 2,
            properties, pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
          )
          injectMotionEventToUiController(uiController, event)
          interrupted = true
        }
      }

      if (!withVelocity) {
        eventTime += eventMinInterval
        event = MotionEvent.obtain(
          startTime, eventTime,
          MotionEvent.ACTION_MOVE, 1, properties,
          pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
        )
        injectMotionEventToUiController(uiController, event)
      }

      // Step 5
      eventTime += eventMinInterval
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_UP, 1, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)
    } catch (ex: InjectEventSecurityException) {
      throw RuntimeException("Could not perform quick scale", ex)
    }
  }

  private fun performMove(
    uiController: UiController,
    startPoint: PointF,
    endPoint: PointF,
    withVelocity: Boolean,
    duration: Long
  ) {
    val eventMinInterval: Long = 10
    val startTime = SystemClock.uptimeMillis()
    var eventTime = startTime
    var event: MotionEvent
    var eventX1: Float = startPoint.x
    var eventY1: Float = startPoint.y

    val properties = arrayOfNulls<MotionEvent.PointerProperties>(1)
    val pp1 = MotionEvent.PointerProperties()
    pp1.id = 0
    pp1.toolType = MotionEvent.TOOL_TYPE_FINGER
    properties[0] = pp1

    val pointerCoords = arrayOfNulls<MotionEvent.PointerCoords>(1)
    val pc1 = MotionEvent.PointerCoords()
    pc1.x = eventX1
    pc1.y = eventY1
    pc1.pressure = 1f
    pc1.size = 1f
    pointerCoords[0] = pc1

    /*
     * Events sequence of move gesture:
     *
     * 1. Send ACTION_DOWN
     * 2. Send ACTION_MOVE with updated middle points (x,y), until reach the end points
     * 3. Send ACTION_UP
     */
    try {
      // Step 1
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_DOWN, 1, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)

      // Step 2
      val moveEventNumber = duration / eventMinInterval

      val stepX1: Float
      val stepY1: Float

      stepX1 = (endPoint.x - startPoint.x) / moveEventNumber
      stepY1 = (endPoint.y - startPoint.y) / moveEventNumber

      for (i in 0 until moveEventNumber) {
        // Update the move events
        eventTime += eventMinInterval
        eventX1 += stepX1
        eventY1 += stepY1

        pc1.x = eventX1
        pc1.y = eventY1

        pointerCoords[0] = pc1

        event = MotionEvent.obtain(
          startTime, eventTime,
          MotionEvent.ACTION_MOVE, 1, properties,
          pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
        )
        injectMotionEventToUiController(uiController, event)
      }

      if (!withVelocity) {
        eventTime += eventMinInterval
        event = MotionEvent.obtain(
          startTime, eventTime,
          MotionEvent.ACTION_MOVE, 1, properties,
          pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
        )
        injectMotionEventToUiController(uiController, event)
      }

      // Step 3
      eventTime += eventMinInterval
      event = MotionEvent.obtain(
        startTime, eventTime,
        MotionEvent.ACTION_UP, 1, properties,
        pointerCoords, 0, 0, 1f, 1f, 0, 0, 0, 0
      )
      injectMotionEventToUiController(uiController, event)
    } catch (ex: InjectEventSecurityException) {
      throw RuntimeException("Could not perform quick scale", ex)
    }
  }

  /**
   * Safely call uiController.injectMotionEvent(event): Detect any error and "convert" it to an
   * IllegalStateException
   */
  @Throws(InjectEventSecurityException::class)
  private fun injectMotionEventToUiController(uiController: UiController, event: MotionEvent) {
    val injectEventSucceeded = uiController.injectMotionEvent(event)
    if (!injectEventSucceeded) {
      throw IllegalStateException("Error performing event $event")
    }
  }
}

fun Int.loopFor(millis: Long) {
  Espresso.onView(ViewMatchers.withId(this)).perform(object : ViewAction {
    override fun getDescription(): String = "waiting for $millis"

    override fun getConstraints(): Matcher<View> = ViewMatchers.isEnabled()

    override fun perform(uiController: UiController?, view: View?) {
      uiController?.loopMainThreadForAtLeast(millis)
    }
  })
}