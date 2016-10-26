package com.mapbox.mapboxsdk.net;

/**
 * Receives updates on connectivity state
 */
public interface ConnectivityListener {

    void onNetworkStateChanged(boolean connected);

}
