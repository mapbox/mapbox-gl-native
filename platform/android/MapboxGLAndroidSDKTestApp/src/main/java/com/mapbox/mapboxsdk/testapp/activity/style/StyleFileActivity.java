package com.mapbox.mapboxsdk.testapp.activity.style;

import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.RawRes;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;

import timber.log.Timber;

/**
 * Example on how to use a file:// resource for the style.json
 */
public class StyleFileActivity extends AppCompatActivity {

  private MapboxMap mapboxMap;
  private MapView mapView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_style_file);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(@NonNull final MapboxMap map) {
        mapboxMap = map;

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setColorFilter(ContextCompat.getColor(StyleFileActivity.this, R.color.primary));
        fab.setOnClickListener(new View.OnClickListener() {
          @Override
          public void onClick(View view) {
            Timber.i("Loading style file");
            new CreateStyleFileTask().execute();
          }
        });
      }
    });
  }

  /**
   * Task to write a style file to local disk and load it in the map view
   */
  private class CreateStyleFileTask extends AsyncTask<Void, Integer, Long> {
    private File cacheStyleFile;

    @Override
    protected Long doInBackground(Void... params) {
      try {
        cacheStyleFile = File.createTempFile("my-", ".style.json");
        cacheStyleFile.createNewFile();
        Timber.i("Writing style file to: " + cacheStyleFile.getAbsolutePath());
        writeToFile(cacheStyleFile, readRawResource(R.raw.local_style));
      } catch (Exception exception) {
        Toast.makeText(StyleFileActivity.this, "Could not create style file in cache dir", Toast.LENGTH_SHORT).show();
      }
      return 1L;
    }

    protected void onPostExecute(Long result) {
      //Actual file:// usage
      mapboxMap.setStyleUrl("file://" + cacheStyleFile.getAbsolutePath());
    }

    private String readRawResource(@RawRes int rawResource) throws IOException {
      InputStream is = getResources().openRawResource(rawResource);
      Writer writer = new StringWriter();
      char[] buffer = new char[1024];
      try {
        Reader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"));
        int numRead;
        while ((numRead = reader.read(buffer)) != -1) {
          writer.write(buffer, 0, numRead);
        }
      } finally {
        is.close();
      }

      return writer.toString();
    }

    private void writeToFile(File file, String contents) throws IOException {
      BufferedWriter writer = null;
      try {
        writer = new BufferedWriter(new FileWriter(file));
        writer.write(contents);
      } finally {
        if (writer != null) {
          writer.close();
        }
      }
    }
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
  public void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  public void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }
}