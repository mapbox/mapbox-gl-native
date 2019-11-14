package com.mapbox.mapboxsdk.testapp.activity.telemetry;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.module.http.HttpRequestUtil;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import okhttp3.Call;
import okhttp3.OkHttpClient;
import timber.log.Timber;

/**
 * Test activity showcasing gathering performance measurement data.
 */
public class PerformanceMeasurementActivity extends AppCompatActivity {

  private MapView mapView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);

    EventListener eventListener = new EventListener();
    OkHttpClient okHttpClient = new OkHttpClient.Builder()
            .eventListener(eventListener)
            .build();
    HttpRequestUtil.setOkHttpClient(okHttpClient);

    mapView.getMapAsync(mapboxMap -> mapboxMap.setStyle(
      new Style.Builder().fromUri(Style.MAPBOX_STREETS)));
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
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onDestroy() {
    HttpRequestUtil.setOkHttpClient(null);
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  private static void triggerPerformanceEvent(String style, long elapsed) {
    List<Attribute<String>> attributes = new ArrayList<>();
    attributes.add(
            new Attribute<>("style_id", style));
    attributes.add(
            new Attribute<>("test_perf_event", "true"));

    List<Attribute<Long>> counters = new ArrayList();
    counters.add(new Attribute<>("elapsed", elapsed));


    JsonObject metaData = new JsonObject();
    metaData.addProperty("os", "android");
    metaData.addProperty("manufacturer", Build.MANUFACTURER);
    metaData.addProperty("brand", Build.BRAND);
    metaData.addProperty("device", Build.MODEL);
    metaData.addProperty("version", Build.VERSION.RELEASE);
    metaData.addProperty("abi", Build.CPU_ABI);
    metaData.addProperty("country", Locale.getDefault().getISO3Country());
    metaData.addProperty("ram", getRam());
    metaData.addProperty("screenSize", getWindowSize());

    Gson gson = new Gson();

    Bundle bundle = new Bundle();
    bundle.putString("attributes", gson.toJson(attributes));
    bundle.putString("counters", gson.toJson(counters));
    bundle.putString("metadata", metaData.toString());

    Mapbox.getTelemetry().onPerformanceEvent(bundle);
  }

  private static String getRam() {
    ActivityManager actManager =
            (ActivityManager) Mapbox.getApplicationContext().getSystemService(Context.ACTIVITY_SERVICE);
    ActivityManager.MemoryInfo memInfo = new ActivityManager.MemoryInfo();
    actManager.getMemoryInfo(memInfo);
    return String.valueOf(memInfo.totalMem);
  }

  private static String getWindowSize() {
    WindowManager windowManager =
            (WindowManager) Mapbox.getApplicationContext().getSystemService(Context.WINDOW_SERVICE);
    Display display = windowManager.getDefaultDisplay();
    DisplayMetrics metrics = new DisplayMetrics();
    display.getMetrics(metrics);
    int width = metrics.widthPixels;
    int height = metrics.heightPixels;

    return "{" + width + "," + height + "}";
  }

  private static class EventListener extends okhttp3.EventListener {

    private Map<String, Long> startTimes = new HashMap<>();

    @Override
    public void callStart(Call call) {
      String url = call.request().url().toString();
      startTimes.put(url, System.nanoTime());
      super.callStart(call);
      Timber.e("callStart: %s", url);
    }

    @Override
    public void callEnd(Call call) {
      String url = call.request().url().toString();
      Timber.e("callEnd: %s", url);
      Long start = startTimes.get(url);
      if (start != null) {
        long elapsed = System.nanoTime() - start;
        triggerPerformanceEvent(url.substring(0, url.indexOf('?')), elapsed);
        startTimes.remove(start);
        Timber.e("callEnd: %s took %d", url, elapsed);
      }
      super.callEnd(call);
    }
  }

  private static class Attribute<T> {

    private String name;

    private T value;

    Attribute(String name, T value) {
      this.name = name;
      this.value = value;
    }
  }

}
