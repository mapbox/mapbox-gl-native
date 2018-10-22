package com.mapbox.mapboxsdk.net;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.log.Logger;

import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * Interface definition for a callback to be invoked when connectivity changes.
 * Not public api.
 */
public class ConnectivityReceiver extends BroadcastReceiver {

  private static final String TAG = "Mbgl-ConnectivityReceiver";

  @SuppressLint("StaticFieldLeak")
  private static ConnectivityReceiver INSTANCE;

  /**
   * Get a single instance of ConnectivityReceiver.
   *
   * @param context the context to extract the application context from
   * @return single instance of ConnectivityReceiver
   */
  public static synchronized ConnectivityReceiver instance(Context context) {
    if (INSTANCE == null) {
      // Register new instance
      INSTANCE = new ConnectivityReceiver(context.getApplicationContext());
      // Add default listeners
      INSTANCE.addListener(new NativeConnectivityListener());
    }

    return INSTANCE;
  }

  private List<ConnectivityListener> listeners = new CopyOnWriteArrayList<>();
  private Context context;
  private int activationCounter;

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
      context.registerReceiver(INSTANCE, new IntentFilter("android.net.conn.CONNECTIVITY_CHANGE"));
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
  public void onReceive(Context context, Intent intent) {
    boolean connected = isConnected(context);
    Logger.v(TAG, String.format("Connected: %s", connected));

    // Loop over listeners
    for (ConnectivityListener listener : listeners) {
      listener.onNetworkStateChanged(connected);
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
   * @param context current Context
   * @return true if connected
   */
  public boolean isConnected(Context context) {
    Boolean connected = Mapbox.isConnected();
    if (connected != null) {
      // Connectivity state overridden by app
      return connected;
    }

    ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
    NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
    return (activeNetwork != null && activeNetwork.isConnected());
  }
}
