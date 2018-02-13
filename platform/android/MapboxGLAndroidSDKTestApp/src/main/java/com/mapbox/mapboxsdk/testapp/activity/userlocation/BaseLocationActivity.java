package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;

import com.mapbox.android.core.permissions.PermissionsListener;
import com.mapbox.android.core.permissions.PermissionsManager;

import java.util.List;

/**
 * Base class for location aware activities.
 */
public abstract class BaseLocationActivity extends AppCompatActivity implements PermissionsListener {

  private PermissionsManager permissionsManager;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    permissionsManager = new PermissionsManager(this);
  }

  @UiThread
  protected final void toggleGps(boolean enableGps) {
    if (enableGps) {
      if (!isRuntimePermissionsRequired()) {
        permissionsManager.requestLocationPermissions(this);
      } else {
        enableLocation(true);
      }
    } else {
      enableLocation(false);
    }
  }

  @Override
  public void onExplanationNeeded(List<String> list) {
    Snackbar.make(
      findViewById(android.R.id.content),
      TextUtils.join("", list.toArray()),
      Snackbar.LENGTH_SHORT).show();
  }

  @Override
  public void onPermissionResult(boolean isPermissionAccepted) {
    enableLocation(isPermissionAccepted);
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    permissionsManager.onRequestPermissionsResult(requestCode, permissions, grantResults);
  }

  private boolean isRuntimePermissionsRequired() {
    return android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M;
  }

  protected abstract void enableLocation(boolean enabled);
}
