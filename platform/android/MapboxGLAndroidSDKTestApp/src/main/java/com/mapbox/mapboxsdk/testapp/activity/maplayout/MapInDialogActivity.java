package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.testapp.R;

public class MapInDialogActivity extends AppCompatActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_in_dialog);

    Button button = (Button) findViewById(R.id.button_open_dialog);
    button.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        FragmentManager fm = getSupportFragmentManager();
        MapDialogFragment editNameDialogFragment = MapDialogFragment.newInstance("Map Dialog");
        editNameDialogFragment.show(fm, "fragment_dialog_map");
      }
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
      return inflater.inflate(R.layout.fragment_dialog_map, container);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
      super.onViewCreated(view, savedInstanceState);

      mapView = (MapView) view.findViewById(R.id.mapView);
      mapView.onCreate(savedInstanceState);
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
    public void onDestroy() {
      super.onDestroy();
      mapView.onDestroy();
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