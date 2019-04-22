package com.mapbox.mapboxsdk.module.telemetry;

import com.google.gson.Gson;

import com.google.gson.JsonObject;

import com.google.gson.reflect.TypeToken;
import com.mapbox.android.telemetry.Event;

import android.os.Bundle;
import android.os.Parcel;


import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

/**
 * Generic Performance Event that can be used for performance measurements.
 * Customer measurements can be added to the bundle.
 *
 * Bundle is expected to have following properties:
 * "attributes", "counters", and "metadata" with String values.
 *
 * Attributes: a string representing an array of name/string value pair objects.
 * Counters: a string representing an array of name/number value pair objects.
 * Metadata is a string representation of a JsonObject with string values.
 *
 *  Here is an example of a Performance event bundle data:
 *
 * "attributes": [{ "name": "style_id", "value": "mapbox://styles/mapbox/streets-v10"}]
 *
 * "counters": [{"name": "fps_average", "value": 90.7655486547093},
 *              {"name": "fps_deviation", "value": 29.301809631465574}]
 * “metadata”: {
 *       “version”: “9”,
 *       “screenSize”: “1080x1794”,
 *       “country”: “US”,
 *       “device”: “Pixel 2”,
 *       “abi”: “arm64-v8a”,
 *       “brand”: “google”,
 *       “ram”: “3834167296”,
 *       “os”: “android”,
 *       “gpu”: “Qualcomm, Adreno (TM) 540, OpenGL ES 3.2 V@313.0 (GIT@7bf2852, Ie32bfa6f6f)“,
 *       “manufacturer”: “Google”
 *   }
 */
public class PerformanceEvent extends Event {

  private static final String PERFORMANCE_TRACE = "mobile.performance_trace";

  private final String event;

  private final String created;

  private final String sessionId;

  private final List<Attribute<String>> attributes;

  private final List<Attribute<Double>> counters;

  private final JsonObject metadata;


  private static final SimpleDateFormat DATE_FORMAT =
          new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSZ", Locale.US);

  PerformanceEvent(String sessionId, Bundle bundle) {

    this.event = PERFORMANCE_TRACE;
    this.created = DATE_FORMAT.format(new Date());
    this.sessionId = sessionId;
    this.attributes = initList(bundle.getString("attributes"),
      new TypeToken<ArrayList<Attribute<String>>>() {});
    this.counters = initList(bundle.getString("counters"),
      new TypeToken<ArrayList<Attribute<Double>>>() {});
    this.metadata = initMetaData(bundle.getString("metadata"));
  }

  private PerformanceEvent(Parcel in) {
    this.event = in.readString();
    this.created = in.readString();
    this.sessionId = in.readString();

    this.attributes = initList(in.readString(), new TypeToken<ArrayList<Attribute<String>>>() {});
    this.counters = initList(in.readString(), new TypeToken<ArrayList<Attribute<Double>>>() {});
    this.metadata = initMetaData(in.readString());
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel parcel, int i) {
    parcel.writeString(event);
    parcel.writeString(created);
    parcel.writeString(sessionId);

    Gson gson = new Gson();

    parcel.writeString(gson.toJson(attributes));
    parcel.writeString(gson.toJson(counters));

    if (metadata != null) {
      parcel.writeString(metadata.toString());
    }
  }

  private <T> ArrayList<Attribute<T>> initList(String fromString, TypeToken typeToken) {
    if (fromString == null || fromString.isEmpty()) {
      return new ArrayList<>();
    }
    return new Gson().fromJson(fromString, typeToken.getType());
  }

  private JsonObject initMetaData(String fromString) {
    if (fromString == null) {
      return new JsonObject();
    } else {
      return new Gson().fromJson(fromString, JsonObject.class);
    }
  }

  public static final Creator<PerformanceEvent> CREATOR = new Creator<PerformanceEvent>() {
    @Override
    public PerformanceEvent createFromParcel(Parcel in) {
      return new PerformanceEvent(in);
    }

    @Override
    public PerformanceEvent[] newArray(int size) {
      return new PerformanceEvent[size];
    }
  };


  private class Attribute<T> {
    private final String name;
    private final T value;

    Attribute(String name, T value) {
      this.name = name;
      this.value = value;
    }
  }
}
