package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.google.gson.JsonElement;
import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;


import java.util.List;
import java.util.Map;

import timber.log.Timber;

/**
 * Test activity showcasing using the query rendered features API to query feature properties on Map click.
 */
public class QueryRenderedFeaturesPropertiesActivity extends AppCompatActivity {

  public MapView mapView;
  private MapboxMap mapboxMap;
  private Marker marker;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_query_features_point);

    final float density = getResources().getDisplayMetrics().density;

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      QueryRenderedFeaturesPropertiesActivity.this.mapboxMap = mapboxMap;

      // Add custom window adapter
      addCustomInfoWindowAdapter(mapboxMap);

      // Add a click listener
      mapboxMap.setOnMapClickListener(point -> {
        // Query
        final PointF pixel = mapboxMap.getProjection().toScreenLocation(point);
        Timber.i(
          "Requesting features for %sx%s (%sx%s adjusted for density)",
          pixel.x, pixel.y, pixel.x / density, pixel.y / density
        );
        List<Feature> features = mapboxMap.queryRenderedFeatures(pixel);

        // Debug output
        debugOutput(features);

        // Remove any previous markers
        if (marker != null) {
          mapboxMap.removeMarker(marker);
        }

        // Add a marker on the clicked point
        marker = mapboxMap.addMarker(new CustomMarkerOptions().position(point).features(features));
        mapboxMap.selectMarker(marker);
      });
    });

  }

  private void debugOutput(List<Feature> features) {
    Timber.i("Got %s features", features.size());
    for (Feature feature : features) {
      if (feature != null) {
        Timber.i("Got feature %s with %s properties and Geometry %s",
          feature.id(),
          feature.properties() != null ? feature.properties().entrySet().size() : "<null>",
          feature.geometry() != null ? feature.geometry().getClass().getSimpleName() : "<null>"
        );
        if (feature.properties() != null) {
          for (Map.Entry<String, JsonElement> entry : feature.properties().entrySet()) {
            Timber.i("Prop %s - %s", entry.getKey(), entry.getValue());
          }
        }
      } else {
        Timber.i("Got NULL feature");
      }
    }
  }

  private void addCustomInfoWindowAdapter(MapboxMap mapboxMap) {
    mapboxMap.setInfoWindowAdapter(new MapboxMap.InfoWindowAdapter() {

      private TextView row(String text) {
        TextView view = new TextView(QueryRenderedFeaturesPropertiesActivity.this);
        view.setText(text);
        return view;
      }

      @Override
      public View getInfoWindow(@NonNull Marker marker) {
        CustomMarker customMarker = (CustomMarker) marker;
        LinearLayout view = new LinearLayout(QueryRenderedFeaturesPropertiesActivity.this);
        view.setOrientation(LinearLayout.VERTICAL);
        view.setBackgroundColor(Color.WHITE);

        if (customMarker.features.size() > 0) {
          view.addView(row(String.format("Found %s features", customMarker.features.size())));
          Feature feature = customMarker.features.get(0);
          for (Map.Entry<String, JsonElement> prop : feature.properties().entrySet()) {
            view.addView(row(String.format("%s: %s", prop.getKey(), prop.getValue())));
          }
        } else {
          view.addView(row("No features here"));
        }

        return view;
      }
    });
  }

  public MapboxMap getMapboxMap() {
    return mapboxMap;
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  protected void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  protected void onPause() {
    super.onPause();
    mapView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  private static class CustomMarker extends Marker {

    private final List<Feature> features;

    CustomMarker(BaseMarkerOptions baseMarkerOptions, List<Feature> features) {
      super(baseMarkerOptions);
      this.features = features;
    }
  }

  private static class CustomMarkerOptions extends BaseMarkerOptions<CustomMarker, CustomMarkerOptions> {


    private List<Feature> features;

    public CustomMarkerOptions features(List<Feature> features) {
      this.features = features;
      return this;
    }

    CustomMarkerOptions() {
    }

    private CustomMarkerOptions(Parcel in) {
      // Should implement this
    }

    @Override
    public CustomMarkerOptions getThis() {
      return this;
    }

    @Override
    public CustomMarker getMarker() {
      return new CustomMarker(this, features);
    }

    public static final Parcelable.Creator<CustomMarkerOptions> CREATOR =
      new Parcelable.Creator<CustomMarkerOptions>() {
        public CustomMarkerOptions createFromParcel(Parcel in) {
          return new CustomMarkerOptions(in);
        }

        public CustomMarkerOptions[] newArray(int size) {
          return new CustomMarkerOptions[size];
        }
      };

    @Override
    public int describeContents() {
      return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
      // Should implement this
    }
  }
}
