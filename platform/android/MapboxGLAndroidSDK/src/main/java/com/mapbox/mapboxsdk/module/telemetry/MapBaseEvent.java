package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Parcel;

import com.mapbox.android.telemetry.Event;

/**
 * Base event class for telemetry events.
 */
@SuppressLint("ParcelCreator")
abstract class MapBaseEvent extends Event {
  private final String event;
  private final String created;

  MapBaseEvent(PhoneState phoneState) {
    this.event = getEventName();
    this.created = phoneState.getCreated();
  }

  abstract String getEventName();

  String getEvent() {
    return event;
  }

  String getCreated() {
    return created;
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
  }
}
