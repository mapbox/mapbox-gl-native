package com.mapbox.mapboxsdk.maps;

import android.os.Bundle;
import android.support.annotation.UiThread;

interface State {

  @UiThread
  void onSaveInstanceState(Bundle outState);

  @UiThread
  void onRestoreInstanceState(Bundle savedInstanceState);
}
