package com.mapbox.mapboxsdk.log;

import android.support.annotation.IntDef;
import android.support.annotation.Keep;
import android.util.Log;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;


/**
 * Logger for the Mapbox Maps SDK for Android
 * <p>
 * Default implementation relies on {@link Log}.
 * Alternative implementations can be set with {@link #setLoggerDefinition(LoggerDefinition)}.
 * </p>
 */
@Keep
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
    public void e(String tag, String msg) {
      Log.e(tag, msg);
    }

    @Override
    public void e(String tag, String msg, Throwable tr) {
      Log.e(tag, msg, tr);
    }
  };

  private static volatile LoggerDefinition logger = DEFAULT;

  @LogLevel
  private static int logLevel;

  /**
   * Set the verbosity of the Logger.
   * <p>
   * This configuration can be used to have more granular control over which logs are emitted by the
   * Mapbox Maps SDK for Android.
   * </p>
   *
   * @param logLevel the verbosity level
   */
  public static void setVerbosity(@LogLevel int logLevel) {
    Logger.logLevel = logLevel;
  }

  /**
   * Replace the current used logger definition.
   *
   * @param loggerDefinition the definition of the logger
   */
  public static void setLoggerDefinition(LoggerDefinition loggerDefinition) {
    logger = loggerDefinition;
  }

  /**
   * Send a verbose log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  public static void v(String tag, String msg) {
    if (logLevel <= VERBOSE) {
      logger.v(tag, msg);
    }
  }

  /**
   * Send a verbose log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  public static void v(String tag, String msg, Throwable tr) {
    if (logLevel <= VERBOSE) {
      logger.v(tag, msg, tr);
    }
  }

  /**
   * Send a debug log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  public static void d(String tag, String msg) {
    if (logLevel <= DEBUG) {
      logger.d(tag, msg);
    }
  }

  /**
   * Send a debug log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  public static void d(String tag, String msg, Throwable tr) {
    if (logLevel <= DEBUG) {
      logger.d(tag, msg, tr);
    }
  }

  /**
   * Send an info log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  public static void i(String tag, String msg) {
    if (logLevel <= INFO) {
      logger.i(tag, msg);
    }
  }

  /**
   * Send an info log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  public static void i(String tag, String msg, Throwable tr) {
    if (logLevel <= INFO) {
      logger.i(tag, msg, tr);
    }
  }

  /**
   * Send a warning log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  public static void w(String tag, String msg) {
    if (logLevel <= WARN) {
      logger.w(tag, msg);
    }
  }

  /**
   * Send a warning log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  public static void w(String tag, String msg, Throwable tr) {
    if (logLevel <= WARN) {
      logger.w(tag, msg, tr);
    }
  }

  /**
   * Send an error log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  public static void e(String tag, String msg) {
    if (logLevel <= ERROR) {
      logger.e(tag, msg);
    }
  }

  /**
   * Send an error log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  public static void e(String tag, String msg, Throwable tr) {
    if (logLevel <= ERROR) {
      logger.e(tag, msg, tr);
    }
  }

  /**
   * Send a log message based on severity.
   *
   * @param severity the log severity
   * @param tag      Used to identify the source of a log message.  It usually identifies
   *                 the class or activity where the log call occurs.
   * @param message  The message you would like logged.
   */
  public static void log(int severity, String tag, String message) {
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

  /**
   * Priority constant for the println method; use Logger.v
   * <p>
   * This log level will print all logs.
   * </p>
   */
  public static final int VERBOSE = Log.VERBOSE;

  /**
   * Priority constant for the println method; use Logger.d.
   * <p>
   * This log level will print all logs except verbose.
   * </p>
   */
  public static final int DEBUG = Log.DEBUG;

  /**
   * Priority constant for the println method; use Logger.i.
   * <p>
   * This log level will print all logs except verbose and debug.
   * </p>
   */
  public static final int INFO = Log.INFO;

  /**
   * Priority constant for the println method; use Logger.w.
   * <p>
   * This log level will print only warn and error logs.
   * </p>
   */
  public static final int WARN = Log.WARN;

  /**
   * Priority constant for the println method; use Logger.e.
   * <p>
   * This log level will print only error logs.
   * </p>
   */
  public static final int ERROR = Log.ERROR;

  /**
   * Priority constant for the println method.
   * <p>
   * This log level won't print any logs.
   * </p>
   */
  public static final int NONE = 99;

  /**
   * Log level indicates which logs are allowed to be emitted by the Mapbox Maps SDK for Android.
   */
  @IntDef( {VERBOSE, DEBUG, INFO, WARN, ERROR, NONE})
  @Retention(RetentionPolicy.SOURCE)
  public @interface LogLevel {
  }
}