package com.mapbox.mapboxsdk.utils;

import android.os.Looper;
import com.mapbox.mapboxsdk.exceptions.CalledFromWorkerThreadException;

public class ThreadUtils {

  /**
   * Validates if execution is occuring on the main thread.
   */
  public static void checkThread(String origin) {
    if (Looper.myLooper() != Looper.getMainLooper()) {
      throw new CalledFromWorkerThreadException(
        String.format("%s interactions should happen on the UI thread.",origin));
    }
  }
}
