package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Parcel;

import com.mapbox.android.telemetry.Event;

/**
 * Base event class for telemetry events.
 */
@SuppressLint("ParcelCreator")
public abstract class MapBaseEvent extends Event {
  final String event;
  final String created;

  MapBaseEvent(PhoneState phoneState) {
    event = getEventName();
    this.created = phoneState.getCreated();
  }

  abstract String getEventName();

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
  }
}
