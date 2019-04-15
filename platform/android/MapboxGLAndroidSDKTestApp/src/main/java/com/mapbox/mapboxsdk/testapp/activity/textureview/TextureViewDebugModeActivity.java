package com.mapbox.mapboxsdk.testapp.activity.textureview;

import android.view.MenuItem;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.activity.maplayout.DebugModeActivity;
import com.mapbox.mapboxsdk.testapp.utils.NavUtils;

/**
 * Test activity showcasing the different debug modes and allows to cycle between the default map styles.
 */
public class TextureViewDebugModeActivity extends DebugModeActivity implements OnMapReadyCallback {

  @Override
  protected MapboxMapOptions setupMapboxMapOptions() {
    MapboxMapOptions mapboxMapOptions = super.setupMapboxMapOptions();
    mapboxMapOptions.textureMode(true);
    return mapboxMapOptions;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case android.R.id.home:
        // activity uses singleInstance for testing purposes
        // code below provides a default navigation when using the app
        onBackPressed();
        return true;
    }
    return super.onOptionsItemSelected(item);
  }


  @Override
  public void onBackPressed() {
    // activity uses singleInstance for testing purposes
    // code below provides a default navigation when using the app
    NavUtils.navigateHome(this);
  }
}
