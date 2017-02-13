package com.mapbox.mapboxsdk.fs;

import android.content.Context;

import com.mapbox.mapboxsdk.offline.OfflineManager;

import java.lang.ref.WeakReference;

/**
 * Holds a central reference to the core's DefaultFileSource for as long as
 * there are active mapviews / offline managers
 */
public class FileSource {

  // Use weak reference to avoid blocking GC on this reference.
  // Should only block when mapview / Offline manager instances
  // are alive
  private static WeakReference<FileSource> INSTANCE;

  public static synchronized FileSource getInstance(Context context) {
    if (INSTANCE == null || INSTANCE.get() == null) {
      String cachePath = OfflineManager.getDatabasePath(context);
      String apkPath = context.getPackageCodePath();
      INSTANCE = new WeakReference<>(new FileSource(cachePath, apkPath));
    }

    return INSTANCE.get();
  }

  private long nativePtr;

  private FileSource(String cachePath, String apkPath) {
    initialize(cachePath, apkPath);
  }

  private native void initialize(String cachePath, String apkPath);

  @Override
  protected native void finalize() throws Throwable;

}
