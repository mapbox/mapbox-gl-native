package com.mapbox.mapboxsdk.log;

/**
 * Definition of a logger for the Mapbox Maps SDK for Android.
 */
public interface LoggerDefinition {

  /**
   * Send a verbose log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  void v(String tag, String msg);

  /**
   * Send a verbose log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  void v(String tag, String msg, Throwable tr);

  /**
   * Send a debug log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  void d(String tag, String msg);

  /**
   * Send a debug log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  void d(String tag, String msg, Throwable tr);

  /**
   * Send an info log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  void i(String tag, String msg);

  /**
   * Send an info log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  void i(String tag, String msg, Throwable tr);

  /**
   * Send a warning log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  void w(String tag, String msg);

  /**
   * Send a warning log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  void w(String tag, String msg, Throwable tr);

  /**
   * Send an error log message.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   */
  void e(String tag, String msg);

  /**
   * Send an error log message and log the exception.
   *
   * @param tag Used to identify the source of a log message.  It usually identifies
   *            the class or activity where the log call occurs.
   * @param msg The message you would like logged.
   * @param tr  An exception to log
   */
  void e(String tag, String msg, Throwable tr);
}