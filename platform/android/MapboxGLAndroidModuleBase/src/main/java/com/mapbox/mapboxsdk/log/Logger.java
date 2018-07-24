package com.mapbox.mapboxsdk.log;

import android.util.Log;

public final class Logger {

  private static final LoggerDefinition DEFAULT = new LoggerDefinition() {

    @Override
    public void v(String tag, String msg) {
      Log.v(tag, msg);
    }

    @Override
    public void v(String tag, String msg, Throwable tr) {
      Log.v(tag, msg, tr);
    }

    @Override
    public void d(String tag, String msg) {
      Log.d(tag, msg);
    }

    @Override
    public void d(String tag, String msg, Throwable tr) {
      Log.d(tag, msg, tr);
    }

    @Override
    public void i(String tag, String msg) {
      Log.i(tag, msg);
    }

    @Override
    public void i(String tag, String msg, Throwable tr) {
      Log.i(tag, msg, tr);
    }

    @Override
    public void w(String tag, String msg) {
      Log.w(tag, msg);
    }

    @Override
    public void w(String tag, String msg, Throwable tr) {
      Log.w(tag, msg, tr);
    }

    @Override
    public void w(String tag, Throwable tr) {
      Log.w(tag, tr);
    }

    @Override
    public void e(String tag, String msg) {
      Log.e(tag, msg);
    }

    @Override
    public void e(String tag, String msg, Throwable tr) {
      Log.e(tag, msg, tr);
    }
  };

  private static volatile LoggerDefinition logger = DEFAULT;

  public static void setLoggerDefinition(LoggerDefinition loggerDefinition) {
    logger = loggerDefinition;
  }

  public static void v(String tag, String msg) {
    logger.v(tag, msg);
  }

  public static void v(String tag, String msg, Throwable tr) {
    logger.v(tag, msg, tr);
  }

  public static void d(String tag, String msg) {
    logger.d(tag, msg);
  }

  public static void d(String tag, String msg, Throwable tr) {
    logger.d(tag, msg, tr);
  }

  public static void i(String tag, String msg) {
    logger.i(tag, msg);
  }

  public static void i(String tag, String msg, Throwable tr) {
    logger.i(tag, msg, tr);
  }

  public static void w(String tag, String msg) {
    logger.w(tag, msg);
  }

  public static void w(String tag, String msg, Throwable tr) {
    logger.w(tag, msg, tr);
  }

  public static void w(String tag, Throwable tr) {
    logger.w(tag, tr);
  }

  public static void e(String tag, String msg) {
    logger.e(tag, msg);
  }

  public static void e(String tag, String msg, Throwable tr) {
    logger.e(tag, msg, tr);
  }

  public static void log(int severity, String tag, String message) {
    LoggerDefinition.log(severity, tag, message);
  }
}