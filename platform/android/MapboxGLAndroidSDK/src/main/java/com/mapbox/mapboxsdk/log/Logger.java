package com.mapbox.mapboxsdk.log;

import android.util.Log;

public final class Logger {

  private static final LoggerDefinition DEFAULT = new LoggerDefinition() {

    private static final String TAG = "MapboxLog";

    @Override
    public void v(String msg) {
      Log.v(TAG, msg);
    }

    @Override
    public void v(String msg, Throwable tr) {
      Log.v(TAG, msg, tr);
    }

    @Override
    public void d(String msg) {
      Log.d(TAG, msg);
    }

    @Override
    public void d(String msg, Throwable tr) {
      Log.d(TAG, msg, tr);
    }

    @Override
    public void i(String msg) {
      Log.i(TAG, msg);
    }

    @Override
    public void i(String msg, Throwable tr) {
      Log.i(TAG, msg, tr);
    }

    @Override
    public void w(String msg) {
      Log.w(TAG, msg);
    }

    @Override
    public void w(String msg, Throwable tr) {
      Log.w(TAG, msg, tr);
    }

    @Override
    public void w(Throwable tr) {
      Log.w(TAG, tr);
    }

    @Override
    public void e(String msg) {
      Log.e(TAG, msg);
    }

    @Override
    public void e(String msg, Throwable tr) {
      Log.e(TAG, msg, tr);
    }
  };

  private static volatile LoggerDefinition logger = DEFAULT;

  public static void setLoggerDefinition(LoggerDefinition loggerDefinition) {
    logger = loggerDefinition;
  }

  public static void v(String msg) {
    logger.v(msg);
  }

  public static void v(String msg, Throwable tr) {
    logger.v(msg, tr);
  }

  public static void d(String msg) {
    logger.d(msg);
  }

  public static void d(String msg, Throwable tr) {
    logger.d(msg, tr);
  }

  public static void i(String msg) {
    logger.i(msg);
  }

  public static void i(String msg, Throwable tr) {
    logger.i(msg, tr);
  }

  public static void w(String msg) {
    logger.w(msg);
  }

  public static void w(String msg, Throwable tr) {
    logger.w(msg, tr);
  }

  public static void w(Throwable tr) {
    logger.w(tr);
  }

  public static void e(String msg) {
    logger.e(msg);
  }

  public static void e(String msg, Throwable tr) {
    logger.e(msg, tr);
  }

  public static void log(int severity, String message){
    LoggerDefinition.log(severity, message);
  }
}