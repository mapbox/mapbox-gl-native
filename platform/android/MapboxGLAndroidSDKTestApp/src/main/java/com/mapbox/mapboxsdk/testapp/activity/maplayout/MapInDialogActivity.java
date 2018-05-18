package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.app.Dialog;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing showing a Map inside of a DialogFragment.
 * <p>
 * Uses the deprecated TextureView API to workaround the issue of seeing a grey background before the gl surface.
 * </p>
 */
public class MapInDialogActivity extends AppCompatActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_in_dialog);

    Button button = (Button) findViewById(R.id.button_open_dialog);
    button.setOnClickListener(view -> {
      FragmentManager fm = getSupportFragmentManager();
      MapDialogFragment editNameDialogFragment = MapDialogFragment.newInstance("Map Dialog");
      editNameDialogFragment.show(fm, "fragment_dialog_map");
    });
  }

  public static class MapDialogFragment extends DialogFragment {

    private MapView mapView;

    public MapDialogFragment() {
    }

    public static MapDialogFragment newInstance(String title) {
      MapDialogFragment frag = new MapDialogFragment();
      Bundle args = new Bundle();
      args.putString("title", title);
      frag.setArguments(args);
      return frag;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
      MapboxMapOptions options = new MapboxMapOptions();
      options.styleUrl("mapbox://styles/mapbox/streets-v10");
      options.camera(new CameraPosition.Builder()
        .target(new LatLng(47.6077, -122.3421))
        .zoom(11)
        .build());
      mapView = new MapView(getActivity(), options);

      LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.fragment_dialog_map, container);
      layout.addView(mapView);

      return layout;
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
      super.onViewCreated(view, savedInstanceState);

      mapView.onCreate(savedInstanceState);
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
      return new Dialog(getActivity(), getTheme()) {
        boolean destroyed = false;

        @Override
        public void dismiss() {
          if (mapView != null && !destroyed) {
            mapView.onDestroy();
            destroyed = true;
          }
          super.dismiss();
        }
      };
    }

    @Override
    public void onStart() {
      super.onStart();
      mapView.onStart();
    }

    @Override
    public void onResume() {
      super.onResume();
      mapView.onResume();
    }

    @Override
    public void onPause() {
      super.onPause();
      mapView.onPause();
    }

    @Override
    public void onStop() {
      super.onStop();
      mapView.onStop();
    }

    @Override
    public void onLowMemory() {
      super.onLowMemory();
      mapView.onLowMemory();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
      super.onSaveInstanceState(outState);
      mapView.onSaveInstanceState(outState);
    }
  }
}
