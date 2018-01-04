package com.mapbox.mapboxsdk.testapp.utils;
/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import android.os.SystemClock;

import java.util.ArrayList;

import timber.log.Timber;

/**
 * A utility class to help log timings splits throughout a method call.
 * Typical usage is:
 * <p>
 * <pre>
 *     TimingLogger timings = new TimingLogger(TAG, "methodA");
 *     // ... do some work A ...
 *     timings.addSplit("work A");
 *     // ... do some work B ...
 *     timings.addSplit("work B");
 *     // ... do some work C ...
 *     timings.addSplit("work C");
 *     timings.dumpToLog();
 * </pre>
 * <p>
 * <p>The dumpToLog call would add the following to the log:</p>
 * <p>
 * <pre>
 *     D/TAG     ( 3459): methodA: begin
 *     D/TAG     ( 3459): methodA:      9 ms, work A
 *     D/TAG     ( 3459): methodA:      1 ms, work B
 *     D/TAG     ( 3459): methodA:      6 ms, work C
 *     D/TAG     ( 3459): methodA: end, 16 ms
 * </pre>
 */
public class TimingLogger {
  /**
   * The Log tag to use for checking Log.isLoggable and for
   * logging the timings.
   */
  private String tag;
  /**
   * A label to be included in every log.
   */
  private String label;
  /**
   * Used to track whether Log.isLoggable was enabled at reset time.
   */
  private boolean disabled;
  /**
   * Stores the time of each split.
   */
  private ArrayList<Long> splits;
  /**
   * Stores the labels for each split.
   */
  private ArrayList<String> splitLabels;

  /**
   * Create and initialize a TimingLogger object that will log using
   * the specific tag. If the Log.isLoggable is not enabled to at
   * least the Log.VERBOSE level for that tag at creation time then
   * the addSplit and dumpToLog call will do nothing.
   *
   * @param tag   the log tag to use while logging the timings
   * @param label a string to be displayed with each log
   */
  public TimingLogger(String tag, String label) {
    reset(tag, label);
  }

  /**
   * Clear and initialize a TimingLogger object that will log using
   * the specific tag. If the Log.isLoggable is not enabled to at
   * least the Log.VERBOSE level for that tag at creation time then
   * the addSplit and dumpToLog call will do nothing.
   *
   * @param tag   the log tag to use while logging the timings
   * @param label a string to be displayed with each log
   */
  public void reset(String tag, String label) {
    this.tag = tag;
    this.label = label;
    reset();
  }

  /**
   * Clear and initialize a TimingLogger object that will log using
   * the tag and label that was specified previously, either via
   * the constructor or a call to reset(tag, label). If the
   * Log.isLoggable is not enabled to at least the Log.VERBOSE
   * level for that tag at creation time then the addSplit and
   * dumpToLog call will do nothing.
   */
  public void reset() {
    disabled = false; // !Log.isLoggable(tag, Log.VERBOSE);
    if (disabled) {
      return;
    }
    if (splits == null) {
      splits = new ArrayList<Long>();
      splitLabels = new ArrayList<String>();
    } else {
      splits.clear();
      splitLabels.clear();
    }
    addSplit(null);
  }

  /**
   * Add a split for the current time, labeled with splitLabel. If
   * Log.isLoggable was not enabled to at least the Log.VERBOSE for
   * the specified tag at construction or reset() time then this
   * call does nothing.
   *
   * @param splitLabel a label to associate with this split.
   */
  public void addSplit(String splitLabel) {
    if (disabled) {
      return;
    }
    long now = SystemClock.elapsedRealtime();
    splits.add(now);
    splitLabels.add(splitLabel);
  }

  /**
   * Dumps the timings to the log using Timber.d(). If Log.isLoggable was
   * not enabled to at least the Log.VERBOSE for the specified tag at
   * construction or reset() time then this call does nothing.
   */
  public void dumpToLog() {
    if (disabled) {
      return;
    }
    Timber.d("%s: begin", label);
    final long first = splits.get(0);
    long now = first;
    for (int i = 1; i < splits.size(); i++) {
      now = splits.get(i);
      final String splitLabel = splitLabels.get(i);
      final long prev = splits.get(i - 1);
      Timber.d("%s:      %s ms, %s", label, (now - prev), splitLabel);
    }
    Timber.d("%s: end, %s ms", label, (now - first));
  }
}
