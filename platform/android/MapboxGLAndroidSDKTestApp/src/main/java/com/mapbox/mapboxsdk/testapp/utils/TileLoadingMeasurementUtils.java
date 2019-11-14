package com.mapbox.mapboxsdk.testapp.utils;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.os.Build;
import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.annotation.StringDef;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.module.http.HttpRequestUtil;

import java.io.IOException;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import okhttp3.Interceptor;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import timber.log.Timber;

import static com.mapbox.mapboxsdk.constants.MapboxConstants.DEFAULT_MEASURE_TILE_DOWNLOAD_ON;
import static com.mapbox.mapboxsdk.constants.MapboxConstants.KEY_META_DATA_MEASURE_TILE_DOWNLOAD_ON;

public class TileLoadingMeasurementUtils {

  private static final String ATTRIBUTE_REQUEST_URL = "requestUrl";


  public static void setUpTileLoadingMeasurement() {
    if (isTileLoadingMeasurementOn()) {
      OkHttpClient okHttpClient = new OkHttpClient.Builder()
              .addNetworkInterceptor(new TileLoadingInterceptor())
              .build();
      HttpRequestUtil.setOkHttpClient(okHttpClient);
    }
  }

  private static boolean isTileLoadingMeasurementOn() {
    return isBooleanMetaDataValueOn(KEY_META_DATA_MEASURE_TILE_DOWNLOAD_ON,
            DEFAULT_MEASURE_TILE_DOWNLOAD_ON);
  }

  private static boolean isBooleanMetaDataValueOn(@NonNull String propKey, boolean defaultValue) {

    try {
      // Try getting a custom value from the app Manifest
      Context context = Mapbox.getApplicationContext();
      ApplicationInfo appInfo = context.getPackageManager().getApplicationInfo(context.getPackageName(),
              PackageManager.GET_META_DATA);
      if (appInfo.metaData != null) {
        return appInfo.metaData.getBoolean(propKey, defaultValue);
      }
    } catch (PackageManager.NameNotFoundException exception) {
      Timber.e("Failed to read the package metadata: " + exception);
      MapStrictMode.strictModeViolation(exception);
    } catch (Exception exception) {
      Timber.e("Failed to read key: " + propKey + " " + exception);
      MapStrictMode.strictModeViolation(exception);
    }
    return defaultValue;
  }


  /**
   * This Interceptor allows to measure time spent getting a response object over network.
   * The following data will be collected:
   *  responseCode, elapsedMS
   *  requestUrl (request string till the question mark),
   *  and device metadata.
   */
  static class TileLoadingInterceptor implements Interceptor {

    private static String metadata = null;

    @StringDef( {CONNECTION_NONE, CONNECTION_CELLULAR, CONNECTION_WIFI})
    @Retention(RetentionPolicy.SOURCE)
    @interface ConnectionState {
    }
    private static final String CONNECTION_NONE = "none";
    private static final String CONNECTION_CELLULAR = "cellular";
    private static final String CONNECTION_WIFI = "wifi";

    @Override
    public Response intercept(Chain chain) throws IOException {
      Request request = chain.request();
      long elapsed = System.nanoTime();

      Response response = chain.proceed(request);
      elapsed = System.nanoTime() - elapsed;

      triggerPerformanceEvent(response, elapsed / 1000000);

      return response;
    }

    private void triggerPerformanceEvent(Response response, long elapsedMs) {
      List<Attribute<String>> attributes = new ArrayList<>();
      String request = getUrl(response.request());
      attributes.add(new Attribute<>("requestUrl", request));
      attributes.add(new Attribute<>("responseCode", String.valueOf(response.code())));
      attributes.add(
              new Attribute<>("connectionState", getConnectionState()));

      List<Attribute<Long>> counters = new ArrayList();
      counters.add(new Attribute<>("elapsedMS", elapsedMs));

      Bundle bundle = new Bundle();
      Gson gson = new Gson();
      bundle.putString("attributes", gson.toJson(attributes));
      bundle.putString("counters", gson.toJson(counters));
      bundle.putString("metadata", getMetadata());

      Mapbox.getTelemetry().onPerformanceEvent(bundle);
    }

    private static String getUrl(Request request) {
      String url = request.url().toString();
      return url.substring(0, url.indexOf('?'));
    }

    private static String getMetadata() {
      if (metadata == null) {
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

        metadata = metaData.toString();
      }
      return metadata;
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

    @ConnectionState
    private static String getConnectionState() {
      Context appContext = Mapbox.getApplicationContext();
      ConnectivityManager connectivityManager =
              (ConnectivityManager) appContext.getSystemService(Context.CONNECTIVITY_SERVICE);

      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        if (connectivityManager != null) {
          NetworkCapabilities capabilities =
                  connectivityManager.getNetworkCapabilities(connectivityManager.getActiveNetwork());
          if (capabilities != null) {
            if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI)) {
              return CONNECTION_WIFI;
            } else if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR)) {
              return CONNECTION_CELLULAR;
            }
          }
        }
      } else {
        if (connectivityManager != null) {
          NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
          if (activeNetwork != null) {
            if (activeNetwork.getType() == ConnectivityManager.TYPE_WIFI) {
              return CONNECTION_WIFI;
            } else if (activeNetwork.getType() == ConnectivityManager.TYPE_MOBILE) {
              return CONNECTION_CELLULAR;
            }
          }
        }
      }
      return CONNECTION_NONE;
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
