package com.mapbox.mapboxsdk.net;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.annotation.NonNull;
import android.util.Log;

import com.mapbox.mapboxsdk.MapboxAccountManager;

import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * Interface definition for a callback to be invoked when connectivity changes.
 * Not public api.
 */
public class ConnectivityReceiver extends BroadcastReceiver {
    private static final String TAG = ConnectivityReceiver.class.getSimpleName();
    private static ConnectivityReceiver INSTANCE;

    /**
     * Get or create the singleton instance.
     *
     * @param context the Context in which the receiver is running.
     * @return instance.
     */
    public static synchronized ConnectivityReceiver instance(Context context) {
        if (INSTANCE == null) {
            //Register new instance
            INSTANCE = new ConnectivityReceiver();
            context.registerReceiver(INSTANCE, new IntentFilter("android.net.conn.CONNECTIVITY_CHANGE"));

            //Add default listeners
            INSTANCE.addListener(new NativeConnectivityListener());
        }

        return INSTANCE;
    }

    private List<ConnectivityListener> listeners = new CopyOnWriteArrayList<>();

    private ConnectivityReceiver() {
    }

    /**
     * Prints whether the Connectivity receiver is connect of not.
     *
     * @param context the Context in which the receiver is running.
     * @param intent  the Intent being received.
     */
    @Override
    public void onReceive(Context context, Intent intent) {
        boolean connected = isConnected(context);
        Log.v(TAG, "Connected: " + connected);

        //Loop over listeners
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
     * @param context the Context in which the receiver is running.
     * @return true if connected
     */
    public boolean isConnected(Context context) {
        Boolean connected = MapboxAccountManager.getInstance().isConnected();
        if (connected != null) {
            // Connectivity state overridden by app
            return connected;
        }

        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
        return (activeNetwork != null && activeNetwork.isConnected());
    }

}
