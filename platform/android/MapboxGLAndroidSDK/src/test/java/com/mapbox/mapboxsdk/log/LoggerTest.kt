package com.mapbox.mapboxsdk.log

import io.mockk.mockk
import io.mockk.verify
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class LoggerTest {

  private val logger: LoggerDefinition = mockk(relaxed = true)

  @Before
  fun setUp() {
    Logger.setLoggerDefinition(logger)
  }

  @Test
  fun verbosityLogLevel() {
    Logger.setVerbosity(Logger.VERBOSE)
    Logger.v(TAG, MESSAGE)
    verify { logger.v(TAG, MESSAGE) }
  }

  @Test
  fun verbosityLogLevelIgnore() {
    Logger.setVerbosity(Logger.DEBUG)
    Logger.v(TAG, MESSAGE)
    verify(exactly = 0) { logger.v(TAG, MESSAGE) }
  }

  @Test
  fun debugLogLevel() {
    Logger.setVerbosity(Logger.VERBOSE)
    Logger.d(TAG, MESSAGE)
    verify { logger.d(TAG, MESSAGE) }
  }

  @Test
  fun debugLogLevelIgnore() {
    Logger.setVerbosity(Logger.WARN)
    Logger.d(TAG, MESSAGE)
    verify(exactly = 0) { logger.d(TAG, MESSAGE) }
  }

  @Test
  fun warnLogLevel() {
    Logger.setVerbosity(Logger.WARN)
    Logger.w(TAG, MESSAGE)
    verify { logger.w(TAG, MESSAGE) }
  }

  @Test
  fun warnLogLevelIgnore() {
    Logger.setVerbosity(Logger.ERROR)
    Logger.w(TAG, MESSAGE)
    verify(exactly = 0) { logger.w(TAG, MESSAGE) }
  }

  @Test
  fun errorLogLevel() {
    Logger.setVerbosity(Logger.ERROR)
    Logger.e(TAG, MESSAGE)
    verify { logger.e(TAG, MESSAGE) }
  }

  @Test
  fun errorLogLevelIgnore() {
    Logger.setVerbosity(Logger.NONE)
    Logger.e(TAG, MESSAGE)
    verify(exactly = 0) { logger.e(TAG, MESSAGE) }
  }

  @Test
  fun noneLogLevelIgnore() {
    Logger.setVerbosity(Logger.NONE)
    Logger.v(TAG, MESSAGE)
    Logger.d(TAG, MESSAGE)
    Logger.w(TAG, MESSAGE)
    Logger.e(TAG, MESSAGE)
    verify(exactly = 0) { logger.v(TAG, MESSAGE) }
    verify(exactly = 0) { logger.d(TAG, MESSAGE) }
    verify(exactly = 0) { logger.w(TAG, MESSAGE) }
    verify(exactly = 0) { logger.e(TAG, MESSAGE) }
  }

  companion object {
    const val TAG: String = "TAG"
    const val MESSAGE: String = "message"
  }
}