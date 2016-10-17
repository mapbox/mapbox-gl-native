package com.mapbox.weartestapp.model;

import android.content.Intent;

public class Feature {

  public int title;
  public Intent activity;

  public int getTitle() {
    return title;
  }

  public void setTitle(int title) {
    this.title = title;
  }

  public Intent getActivity() {
    return activity;
  }

  public void setActivity(Intent activity) {
    this.activity = activity;
  }

  public Feature(int title, Intent activity) {
    this.title = title;
    this.activity = activity;
  }
}
