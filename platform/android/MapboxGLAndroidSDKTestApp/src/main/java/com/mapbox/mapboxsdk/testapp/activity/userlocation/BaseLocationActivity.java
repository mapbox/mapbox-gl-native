package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.services.android.telemetry.permissions.PermissionsManager;

public abstract class BaseLocationActivity extends AppCompatActivity {

  private static final int PERMISSIONS_LOCATION = 0;

  @UiThread
  protected final void toggleGps(boolean enableGps) {
    if (enableGps) {
      if (!PermissionsManager.areLocationPermissionsGranted(this)) {
        ActivityCompat.requestPermissions(this, new String[] {Manifest.permission.ACCESS_COARSE_LOCATION,
          Manifest.permission.ACCESS_FINE_LOCATION}, PERMISSIONS_LOCATION);
      } else {
        enableLocation(true);
      }
    } else {
      enableLocation(false);
    }
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    if (requestCode == PERMISSIONS_LOCATION) {
      if (!isRuntimePermissionsRequired() || isPermissionAccepted(grantResults)) {
        enableLocation(true);
      }
    }
  }

  private boolean isPermissionAccepted(int[] grantResults) {
    return grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED;
  }

  private boolean isRuntimePermissionsRequired() {
    return android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M;
  }

  protected abstract void enableLocation(boolean enabled);
}
