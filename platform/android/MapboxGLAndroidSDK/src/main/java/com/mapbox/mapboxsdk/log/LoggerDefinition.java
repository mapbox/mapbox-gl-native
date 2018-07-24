package com.mapbox.mapboxsdk.log;

import android.util.Log;

public interface LoggerDefinition {

  void v(String msg);

  void v(String msg, Throwable tr);

  void d(String msg);

  void d(String msg, Throwable tr);

  void i(String msg);

  void i(String msg, Throwable tr);

  void w(String msg);

  void w(String msg, Throwable tr);

  void w(Throwable tr);

  void e(String msg);

  void e(String msg, Throwable tr);

  static void log(int severity, String message) {
    switch (severity) {
      case Log.VERBOSE:
        Logger.v(message);
        break;
      case Log.DEBUG:
        Logger.d(message);
        break;
      case Log.INFO:
        Logger.i(message);
        break;
      case Log.WARN:
        Logger.w(message);
        break;
      case Log.ERROR:
        Logger.e(message);
        break;
      default:
        throw new UnsupportedOperationException();
    }
  }
}
