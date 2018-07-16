package com.mapbox.mapboxsdk.testapp.utils;

import com.mapbox.mapboxsdk.log.LoggerDefinition;
import timber.log.Timber;

public class TimberLogger implements LoggerDefinition {

  @Override
  public void v(String msg) {
    Timber.v(msg);
  }

  @Override
  public void v(String msg, Throwable tr) {
    Timber.v(tr, msg);
  }

  @Override
  public void d(String msg) {
    Timber.d(msg);
  }

  @Override
  public void d(String msg, Throwable tr) {
    Timber.d(tr, msg);
  }

  @Override
  public void i(String msg) {
    Timber.i(msg);
  }

  @Override
  public void i(String msg, Throwable tr) {
    Timber.i(tr, msg);
  }

  @Override
  public void w(String msg) {
    Timber.w(msg);
  }

  @Override
  public void w(String msg, Throwable tr) {
    Timber.w(tr, msg);
  }

  @Override
  public void w(Throwable tr) {
    Timber.w(tr);
  }

  @Override
  public void e(String msg) {
    Timber.e(msg);
  }

  @Override
  public void e(String msg, Throwable tr) {
    Timber.e(tr, msg);
  }
}
