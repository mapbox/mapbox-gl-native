package com.mapbox.mapboxsdk.utils;

import android.os.AsyncTask;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.log.Logger;

import java.io.File;
import java.lang.ref.WeakReference;

public class FileUtils {

  private static final String TAG = "Mbgl-FileUtils";

  /**
   * Task checking whether app's process can read a file.
   */
  public static class CheckFileReadPermissionTask extends AsyncTask<File, Void, Boolean> {
    @NonNull
    private final WeakReference<OnCheckFileReadPermissionListener> listenerWeakReference;

    public CheckFileReadPermissionTask(OnCheckFileReadPermissionListener listener) {
      this.listenerWeakReference = new WeakReference<>(listener);
    }

    @Override
    protected Boolean doInBackground(File... files) {
      try {
        return files[0].canRead();
      } catch (Exception ex) {
        return false;
      }
    }

    @Override
    protected void onCancelled() {
      OnCheckFileReadPermissionListener listener = listenerWeakReference.get();
      if (listener != null) {
        listener.onError();
      }
    }

    @Override
    protected void onPostExecute(Boolean result) {
      OnCheckFileReadPermissionListener listener = listenerWeakReference.get();
      if (listener != null) {
        if (result) {
          listener.onReadPermissionGranted();
        } else {
          listener.onError();
        }
      }
    }
  }

  /**
   * Interface definition for a callback invoked when checking file's read permissions.
   */
  public interface OnCheckFileReadPermissionListener {

    /**
     * Invoked when app's process has a permission to read a file.
     */
    void onReadPermissionGranted();

    /**
     * Invoked when app's process doesn't have a permission to read a file or an error occurs.
     */
    void onError();
  }

  /**
   * Task checking whether app's process can write to a file.
   */
  public static class CheckFileWritePermissionTask extends AsyncTask<File, Void, Boolean> {
    @NonNull
    private final WeakReference<OnCheckFileWritePermissionListener> listenerWeakReference;

    public CheckFileWritePermissionTask(OnCheckFileWritePermissionListener listener) {
      this.listenerWeakReference = new WeakReference<>(listener);
    }

    @Override
    protected Boolean doInBackground(File... files) {
      try {
        return files[0].canWrite();
      } catch (Exception ex) {
        return false;
      }
    }

    @Override
    protected void onCancelled() {
      OnCheckFileWritePermissionListener listener = listenerWeakReference.get();
      if (listener != null) {
        listener.onError();
      }
    }

    @Override
    protected void onPostExecute(Boolean result) {
      OnCheckFileWritePermissionListener listener = listenerWeakReference.get();
      if (listener != null) {
        if (result) {
          listener.onWritePermissionGranted();
        } else {
          listener.onError();
        }
      }
    }
  }

  /**
   * Interface definition for a callback invoked when checking file's write permissions.
   */
  public interface OnCheckFileWritePermissionListener {

    /**
     * Invoked when app's process has a permission to write to a file.
     */
    void onWritePermissionGranted();

    /**
     * Invoked when app's process doesn't have a permission to write to a file or an error occurs.
     */
    void onError();
  }

  /**
   * Deletes a file asynchronously in a separate thread.
   *
   * @param path the path of the file that should be deleted
   */
  public static void deleteFile(@NonNull final String path) {
    // Delete the file in a separate thread to avoid affecting the UI
    new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          File file = new File(path);
          if (file.exists()) {
            if (file.delete()) {
              Logger.d(TAG, "File deleted to save space: " + path);
            } else {
              Logger.e(TAG, "Failed to delete file: " + path);
            }
          }
        } catch (Exception exception) {
          Logger.e(TAG, "Failed to delete file: ", exception);
        }
      }
    }).start();
  }
}
