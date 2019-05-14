package com.mapbox.mapboxsdk.net;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.UiThread;

import com.mapbox.mapboxsdk.log.Logger;

import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * Interface definition for a callback to be invoked when connectivity changes.
 * Not public api.
 */
public class ConnectivityReceiver extends BroadcastReceiver {

  private static final String TAG = "Mbgl-ConnectivityReceiver";
  private static final String LOG_CONNECTED = "connected - true";
  private static final String LOG_NOT_CONNECTED = "connected - false";

  @SuppressLint("StaticFieldLeak")
  private static ConnectivityReceiver INSTANCE;

  /**
   * Get a single instance of ConnectivityReceiver.
   *
   * @param context the context to extract the application context from
   * @return single instance of ConnectivityReceiver
   */
  public static synchronized ConnectivityReceiver instance(@NonNull Context context) {
    if (INSTANCE == null) {
      // Register new instance
      INSTANCE = new ConnectivityReceiver(context.getApplicationContext());
      // Add default listeners
      INSTANCE.addListener(new NativeConnectivityListener());
    }

    return INSTANCE;
  }

  @NonNull
  private List<ConnectivityListener> listeners = new CopyOnWriteArrayList<>();
  private Context context;
  private int activationCounter;
  @Nullable
  private Boolean connected;

  private ConnectivityReceiver(@NonNull Context context) {
    this.context = context;
  }

  /**
   * Activates the connectivity receiver.
   * <p>
   * if the underlying connectivity receiver isn't active, register the connectivity receiver.
   * </p>
   */
  @UiThread
  public void activate() {
    if (activationCounter == 0) {
      context.registerReceiver(this, new IntentFilter("android.net.conn.CONNECTIVITY_CHANGE"));
    }
    activationCounter++;
  }

  /**
   * Deactivates the connectivity receiver.
   * <p>
   * if no other components are listening, unregister the underlying connectivity receiver.
   * </p>
   */
  @UiThread
  public void deactivate() {
    activationCounter--;
    if (activationCounter == 0) {
      context.unregisterReceiver(INSTANCE);
    }
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void onReceive(@NonNull Context context, Intent intent) {
    if (connected != null) {
      // Connectivity state overridden by app
      return;
    }

    notifyListeners(isNetworkActive());
  }

  /**
   * Overwrites system connectivity state. To set, use {@link com.mapbox.mapboxsdk.Mapbox#setConnected(Boolean)}.
   *
   * @param connected flag to determine the connectivity state, true for connected, false for
   *                  disconnected, and null for ConnectivityManager to determine.
   */
  public void setConnected(Boolean connected) {
    this.connected = connected;

    boolean state;
    if (connected != null) {
      state = connected;
    } else {
      state = isNetworkActive();
    }
    notifyListeners(state);
  }

  private void notifyListeners(boolean isConnected) {
    Logger.v(TAG, isConnected ? LOG_CONNECTED : LOG_NOT_CONNECTED);

    // Loop over listeners
    for (ConnectivityListener listener : listeners) {
      listener.onNetworkStateChanged(isConnected);
    }
  }

  /**
   * Add a listener to be notified
   *
   * @param listener the listener to add
   */
  public void addListener(@NonNull ConnectivityListener listener) {
    listeners.add(listener);
  }

  /**
   * Remove a listener
   *
   * @param listener the listener to remove
   */
  public void removeListener(@NonNull ConnectivityListener listener) {
    listeners.remove(listener);
  }

  /**
   * Get current connectivity state
   *
   * @return true if connected
   */
  public boolean isConnected() {
    return connected != null ? connected : isNetworkActive();
  }

  private boolean isNetworkActive() {
    ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
    NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
    return (activeNetwork != null && activeNetwork.isConnected());
  }
}
