package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Bundle;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.reflect.TypeToken;

import java.util.ArrayList;
import java.util.List;

/**
 * Generic Performance Event that can be used for performance measurements.
 * Customer measurements can be added to the bundle.
 * <p>
 * Bundle is expected to have following properties:
 * "attributes", "counters", and "metadata" with String values.
 * </p>
 * <p>
 * Attributes: a string representing an array of name/string value pair objects.
 * Counters: a string representing an array of name/number value pair objects.
 * Metadata is a string representation of a JsonObject with string values.
 * </p>
 * <p>
 * Here is an example of a Performance event bundle data:
 * </p>
 * <p>
 * "attributes": [{ "name": "style_id", "value": "mapbox://styles/mapbox/streets-v10"}]
 * </p>
 * <p>
 * "counters": [{"name": "fps_average", "value": 90.7655486547093},
 * {"name": "fps_deviation", "value": 29.301809631465574}]
 * “metadata”: {
 * “version”: “9”,
 * “screenSize”: “1080x1794”,
 * “country”: “US”,
 * “device”: “Pixel 2”,
 * “abi”: “arm64-v8a”,
 * “brand”: “google”,
 * “ram”: “3834167296”,
 * “os”: “android”,
 * “gpu”: “Qualcomm, Adreno (TM) 540, OpenGL ES 3.2 V@313.0 (GIT@7bf2852, Ie32bfa6f6f)“,
 * “manufacturer”: “Google”
 * }
 * </p>
 */
@SuppressLint("ParcelCreator")
class PerformanceEvent extends MapBaseEvent {
  private static final String PERFORMANCE_TRACE = "mobile.performance_trace";
  private final String sessionId;
  private final List<PerformanceAttribute<String>> attributes;
  private final List<PerformanceAttribute<Double>> counters;
  private final JsonObject metadata;

  PerformanceEvent(PhoneState phoneState, String sessionId, Bundle bundle) {
    super(phoneState);
    this.sessionId = sessionId;
    this.attributes = initList(bundle.getString("attributes"),
      new TypeToken<ArrayList<PerformanceAttribute<String>>>() {
      });
    this.counters = initList(bundle.getString("counters"),
      new TypeToken<ArrayList<PerformanceAttribute<Double>>>() {
      });
    this.metadata = initMetaData(bundle.getString("metadata"));
  }

  private <T> ArrayList<PerformanceAttribute<T>> initList(String fromString, TypeToken typeToken) {
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

  @Override
  String getEventName() {
    return PERFORMANCE_TRACE;
  }

  String getSessionId() {
    return sessionId;
  }

  List<PerformanceAttribute<String>> getAttributes() {
    return attributes;
  }

  List<PerformanceAttribute<Double>> getCounters() {
    return counters;
  }

  JsonObject getMetadata() {
    return metadata;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    PerformanceEvent that = (PerformanceEvent) o;

    if (sessionId != null ? !sessionId.equals(that.sessionId) : that.sessionId != null) {
      return false;
    }
    if (attributes != null ? !attributes.equals(that.attributes) : that.attributes != null) {
      return false;
    }
    if (counters != null ? !counters.equals(that.counters) : that.counters != null) {
      return false;
    }
    return metadata != null ? metadata.equals(that.metadata) : that.metadata == null;
  }

  @Override
  public int hashCode() {
    int result = sessionId != null ? sessionId.hashCode() : 0;
    result = 31 * result + (attributes != null ? attributes.hashCode() : 0);
    result = 31 * result + (counters != null ? counters.hashCode() : 0);
    result = 31 * result + (metadata != null ? metadata.hashCode() : 0);
    return result;
  }

  @Override
  public String toString() {
    return "PerformanceEvent{"
      + "sessionId='" + sessionId + '\''
      + ", attributes=" + attributes
      + ", counters=" + counters
      + ", metadata=" + metadata
      + '}';
  }

  static class PerformanceAttribute<T> {
    private final String name;
    private final T value;

    PerformanceAttribute(String name, T value) {
      this.name = name;
      this.value = value;
    }

    @Override
    public boolean equals(Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }

      PerformanceAttribute<?> that = (PerformanceAttribute<?>) o;

      if (name != null ? !name.equals(that.name) : that.name != null) {
        return false;
      }
      return value != null ? value.equals(that.value) : that.value == null;
    }

    @Override
    public int hashCode() {
      int result = name != null ? name.hashCode() : 0;
      result = 31 * result + (value != null ? value.hashCode() : 0);
      return result;
    }
  }
}
