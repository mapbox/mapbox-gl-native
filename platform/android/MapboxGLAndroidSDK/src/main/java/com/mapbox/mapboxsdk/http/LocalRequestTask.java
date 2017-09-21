package com.mapbox.mapboxsdk.http;

import android.content.res.AssetManager;
import android.os.AsyncTask;
import com.mapbox.mapboxsdk.Mapbox;
import timber.log.Timber;

import java.io.IOException;
import java.io.InputStream;

class LocalRequestTask extends AsyncTask<String, Void, byte[]> {

  private OnLocalRequestResponse requestResponse;

  LocalRequestTask(OnLocalRequestResponse requestResponse) {
    this.requestResponse = requestResponse;
  }

  @Override
  protected byte[] doInBackground(String... strings) {
    return loadFile(Mapbox.getApplicationContext().getAssets(),
      "integration/" + strings[0]
        .substring(8)
        .replaceAll("%20", " ")
        .replaceAll("%2c", ","));
  }

  @Override
  protected void onPostExecute(byte[] bytes) {
    super.onPostExecute(bytes);
    if (bytes != null && requestResponse != null) {
      requestResponse.onResponse(bytes);
    }
  }

  private static byte[] loadFile(AssetManager assets, String path) {
    byte[] buffer = null;
    try (InputStream input = assets.open(path)) {
      int size = input.available();
      buffer = new byte[size];
      input.read(buffer);
    } catch (IOException exception) {
      Timber.e(exception);
    }
    return buffer;
  }

  public interface OnLocalRequestResponse {
    void onResponse(byte[] bytes);
  }
}
