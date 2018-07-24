package com.mapbox.mapboxsdk.log;

import android.util.Log;

public interface LoggerDefinition {

  void v(String tag, String msg);

  void v(String tag, String msg, Throwable tr);

  void d(String tag, String msg);

  void d(String tag, String msg, Throwable tr);

  void i(String tag, String msg);

  void i(String tag, String msg, Throwable tr);

  void w(String tag, String msg);

  void w(String tag, String msg, Throwable tr);

  void w(String tag, Throwable tr);

  void e(String tag, String msg);

  void e(String tag, String msg, Throwable tr);

  static void log(int severity, String tag, String message) {
    switch (severity) {
      case Log.VERBOSE:
        Logger.v(tag, message);
        break;
      case Log.DEBUG:
        Logger.d(tag, message);
        break;
      case Log.INFO:
        Logger.i(tag, message);
        break;
      case Log.WARN:
        Logger.w(tag, message);
        break;
      case Log.ERROR:
        Logger.e(tag, message);
        break;
      default:
        throw new UnsupportedOperationException();
    }
  }
}