package com.mapbox.mapboxsdk.testapp.utils;

import com.mapbox.mapboxsdk.log.LoggerDefinition;
import timber.log.Timber;

public class TimberLogger implements LoggerDefinition {

  @Override
  public void v(String tag, String msg) {
    Timber.tag(tag).v(msg);
  }

  @Override
  public void v(String tag, String msg, Throwable tr) {
    Timber.tag(tag).v(tr, msg);
  }

  @Override
  public void d(String tag, String msg) {
    Timber.tag(tag).d(msg);
  }

  @Override
  public void d(String tag, String msg, Throwable tr) {
    Timber.tag(tag).d(tr, msg);
  }

  @Override
  public void i(String tag, String msg) {
    Timber.tag(tag).i(msg);
  }

  @Override
  public void i(String tag, String msg, Throwable tr) {
    Timber.tag(tag).i(tr, msg);
  }

  @Override
  public void w(String tag, String msg) {
    Timber.tag(tag).w(msg);
  }

  @Override
  public void w(String tag, String msg, Throwable tr) {
    Timber.tag(tag).w(tr, msg);
  }

  @Override
  public void e(String tag, String msg) {
    Timber.tag(tag).e(msg);
  }

  @Override
  public void e(String tag, String msg, Throwable tr) {
    Timber.tag(tag).e(tr, msg);
  }
}
