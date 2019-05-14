package com.mapbox.mapboxsdk.offline;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.StringDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * An Offline Region error
 */
public class OfflineRegionError {

  /**
   * Error code, as a string, self-explanatory.
   */
  @StringDef( {REASON_SUCCESS, REASON_NOT_FOUND, REASON_SERVER, REASON_CONNECTION, REASON_OTHER})
  @Retention(RetentionPolicy.SOURCE)
  public @interface ErrorReason {
  }

  public static final String REASON_SUCCESS = "REASON_SUCCESS";
  public static final String REASON_NOT_FOUND = "REASON_NOT_FOUND";
  public static final String REASON_SERVER = "REASON_SERVER";
  public static final String REASON_CONNECTION = "REASON_CONNECTION";
  public static final String REASON_OTHER = "REASON_OTHER";

  @NonNull
  @ErrorReason
  private final String reason;

  /**
   * /* An error message from the request handler, e.g. a server message or a system message
   * /* informing the user about the reason for the failure.
   */
  @NonNull
  private final String message;

  // Constructors
  @Keep
  private OfflineRegionError(@NonNull String reason, @NonNull String message) {
    // For JNI use only
    this.reason = reason;
    this.message = message;
  }

  // Getters

  @NonNull
  @ErrorReason
  public String getReason() {
    return reason;
  }

  @NonNull
  public String getMessage() {
    return message;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    OfflineRegionError that = (OfflineRegionError) o;

    if (!reason.equals(that.reason)) {
      return false;
    }
    return message.equals(that.message);
  }

  @Override
  public int hashCode() {
    int result = reason.hashCode();
    result = 31 * result + message.hashCode();
    return result;
  }

  @Override
  public String toString() {
    return "OfflineRegionError{"
      + "reason='" + reason + '\''
      + ", message='" + message + '\''
      + '}';
  }
}