package com.mapbox.mapboxsdk.maps;

import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.support.annotation.NonNull;
import android.text.Html;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.text.style.URLSpan;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Toast;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.services.android.telemetry.MapboxTelemetry;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Locale;

/**
 * Responsible for managing attribution interactions on the map.
 * <p>
 * When the user clicks the attribution icon, {@link AttributionDialogManager#onClick(View)} will be invoked.
 * An attribution dialog will be shown to the user with contents based on the attributions found in the map style.
 * Additionally an telemetry option item is shown to configure telemetry settings.
 * </p>
 */
class AttributionDialogManager implements View.OnClickListener, DialogInterface.OnClickListener {

  private static final String MAP_FEEDBACK_URL = "https://www.mapbox.com/map-feedback";
  private static final String MAP_FEEDBACK_LOCATION_FORMAT = MAP_FEEDBACK_URL + "/#/%f/%f/%d";

  private final Context context;
  private final MapboxMap mapboxMap;
  private String[] attributionKeys;
  private HashMap<String, String> attributionMap;

  AttributionDialogManager(@NonNull Context context, @NonNull MapboxMap mapboxMap) {
    this.context = context;
    this.mapboxMap = mapboxMap;
  }

  // Called when someone presses the attribution icon on the map
  @Override
  public void onClick(View view) {
    attributionMap = new AttributionBuilder(context, mapboxMap).build();
    showAttributionDialog();
  }

  private void showAttributionDialog() {
    attributionKeys = attributionMap.keySet().toArray(new String[attributionMap.size()]);
    AlertDialog.Builder builder = new AlertDialog.Builder(context);
    builder.setTitle(R.string.mapbox_attributionsDialogTitle);
    builder.setAdapter(new ArrayAdapter<>(context, R.layout.mapbox_attribution_list_item, attributionKeys), this);
    builder.show();
  }

  // Called when someone selects an attribution or telemetry settings from the dialog
  @Override
  public void onClick(DialogInterface dialog, int which) {
    if (isLatestEntry(which)) {
      showTelemetryDialog();
    } else {
      showMapFeedbackWebPage(which);
    }
  }

  private boolean isLatestEntry(int attributionKeyIndex) {
    return attributionKeyIndex == attributionKeys.length - 1;
  }

  private void showTelemetryDialog() {
    AlertDialog.Builder builder = new AlertDialog.Builder(context);
    builder.setTitle(R.string.mapbox_attributionTelemetryTitle);
    builder.setMessage(R.string.mapbox_attributionTelemetryMessage);
    builder.setPositiveButton(R.string.mapbox_attributionTelemetryPositive, new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        MapboxTelemetry.getInstance().setTelemetryEnabled(true);
        dialog.cancel();
      }
    });
    builder.setNeutralButton(R.string.mapbox_attributionTelemetryNeutral, new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        showWebPage(context.getResources().getString(R.string.mapbox_telemetryLink));
        dialog.cancel();
      }
    });
    builder.setNegativeButton(R.string.mapbox_attributionTelemetryNegative, new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        MapboxTelemetry.getInstance().setTelemetryEnabled(false);
        dialog.cancel();
      }
    });
    builder.show();
  }

  private void showMapFeedbackWebPage(int which) {
    String url = attributionMap.get(attributionKeys[which]);
    if (url.contains(MAP_FEEDBACK_URL)) {
      url = buildMapFeedbackMapUrl(mapboxMap.getCameraPosition());
    }
    showWebPage(url);
  }

  private String buildMapFeedbackMapUrl(CameraPosition cameraPosition) {
    // appends current location to the map feedback url if available
    return cameraPosition != null ? String.format(Locale.getDefault(),
      MAP_FEEDBACK_LOCATION_FORMAT, cameraPosition.target.getLongitude(), cameraPosition.target.getLatitude(),
      (int) cameraPosition.zoom) : MAP_FEEDBACK_URL;
  }

  private void showWebPage(@NonNull String url) {
    try {
      Intent intent = new Intent(Intent.ACTION_VIEW);
      intent.setData(Uri.parse(url));
      context.startActivity(intent);
    } catch (ActivityNotFoundException exception) {
      // explicitly handling if the device hasn't have a web browser installed. #8899
      Toast.makeText(context, R.string.mapbox_attributionErrorNoBrowser, Toast.LENGTH_LONG).show();
    }
  }

  private static class AttributionBuilder {

    private final HashMap<String, String> map = new LinkedHashMap<>();
    private final Context context;
    private final MapboxMap mapboxMap;

    AttributionBuilder(Context context, MapboxMap mapboxMap) {
      this.context = context.getApplicationContext();
      this.mapboxMap = mapboxMap;
    }

    private HashMap<String, String> build() {
      for (Source source : mapboxMap.getSources()) {
        parseAttribution(source.getAttribution());
      }
      addTelemetryEntryToAttributionMap();
      return map;
    }

    private void parseAttribution(String attributionSource) {
      if (!TextUtils.isEmpty(attributionSource)) {
        SpannableStringBuilder htmlBuilder = (SpannableStringBuilder) Html.fromHtml(attributionSource);
        URLSpan[] urlSpans = htmlBuilder.getSpans(0, htmlBuilder.length(), URLSpan.class);
        for (URLSpan urlSpan : urlSpans) {
          map.put(resolveAnchorValue(htmlBuilder, urlSpan), urlSpan.getURL());
        }
      }
    }

    private String resolveAnchorValue(SpannableStringBuilder htmlBuilder, URLSpan urlSpan) {
      int start = htmlBuilder.getSpanStart(urlSpan);
      int end = htmlBuilder.getSpanEnd(urlSpan);
      int length = end - start;
      char[] charKey = new char[length];
      htmlBuilder.getChars(start, end, charKey, 0);
      return String.valueOf(charKey);
    }

    private void addTelemetryEntryToAttributionMap() {
      String telemetryKey = context.getString(R.string.mapbox_telemetrySettings);
      String telemetryLink = context.getString(R.string.mapbox_telemetryLink);
      map.put(telemetryKey, telemetryLink);
    }
  }
}
