package com.mapbox.mapboxsdk.net;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.Mapbox;

/**
 * Interface definition for a callback to be invoked when connectivity changes.
 * Not public api.
 */
public class ConnectivityReceiver extends BroadcastReceiver {
  private static ConnectivityReceiver INSTANCE;
  private final Context context;
  private final NativeConnectivityListener nativeConnectivityListener = new NativeConnectivityListener();
  private int counter;

  /**
   * Get or create the singleton instance
   */
  public static synchronized ConnectivityReceiver instance(Context context) {
    if (INSTANCE == null) {
      INSTANCE = new ConnectivityReceiver(context);
    }
    return INSTANCE;
  }

  private ConnectivityReceiver(@NonNull Context context) {
    this.context = context.getApplicationContext();
  }

  /**
   * Activate listening to connectivity change events.
   */
  public void activate() {
    if (counter == 0) {
      context.registerReceiver(INSTANCE, new IntentFilter("android.net.conn.CONNECTIVITY_CHANGE"));
    }
    counter++;
  }

  /**
   * Deactivate listening to connectivity change events.
   */
  public void deactivate() {
    counter--;
    if (counter == 0) {
      context.unregisterReceiver(INSTANCE);
    }
  }

  /**
   * @see BroadcastReceiver#onReceive(Context, Intent)
   */
  @Override
  public void onReceive(Context context, Intent intent) {
    nativeConnectivityListener.nativeOnConnectivityStateChanged(isConnected());
  }

  /**
   * Get current connectivity state
   *
   * @return true if connected
   */
  public boolean isConnected() {
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
