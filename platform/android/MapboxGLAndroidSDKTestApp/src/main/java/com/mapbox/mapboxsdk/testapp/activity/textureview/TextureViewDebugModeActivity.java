package com.mapbox.mapboxsdk.testapp.activity.textureview;

import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.activity.maplayout.DebugModeActivity;

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
}
