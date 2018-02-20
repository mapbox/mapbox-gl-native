package com.mapbox.mapboxsdk.maps;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.support.annotation.NonNull;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Toast;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.attribution.Attribution;
import com.mapbox.mapboxsdk.attribution.AttributionParser;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Set;

/**
 * Responsible for managing attribution interactions on the map.
 * <p>
 * When the user clicks the attribution icon, {@link AttributionDialogManager#onClick(View)} will be invoked.
 * An attribution dialog will be shown to the user with contents based on the attributions found in the map style.
 * Additionally an telemetry option item is shown to configure telemetry settings.
 * </p>
 */
public class AttributionDialogManager implements View.OnClickListener, DialogInterface.OnClickListener {

  private static final String MAP_FEEDBACK_URL = "https://www.mapbox.com/map-feedback";
  private static final String MAP_FEEDBACK_LOCATION_FORMAT = MAP_FEEDBACK_URL + "/#/%f/%f/%d";

  private final Context context;
  private final MapboxMap mapboxMap;
  private Set<Attribution> attributionSet;

  public AttributionDialogManager(@NonNull Context context, @NonNull MapboxMap mapboxMap) {
    this.context = context;
    this.mapboxMap = mapboxMap;
  }

  // Called when someone presses the attribution icon on the map
  @Override
  public void onClick(View view) {
    attributionSet = new AttributionBuilder(mapboxMap).build();

    boolean isActivityFinishing = false;
    if (context instanceof Activity) {
      isActivityFinishing = ((Activity) context).isFinishing();
    }

    // check is hosting activity isn't finishing
    // https://github.com/mapbox/mapbox-gl-native/issues/11238
    if (!isActivityFinishing) {
      showAttributionDialog(getAttributionTitles());
    }
  }

  protected void showAttributionDialog(String[] attributionTitles) {
    AlertDialog.Builder builder = new AlertDialog.Builder(context);
    builder.setTitle(R.string.mapbox_attributionsDialogTitle);
    builder.setAdapter(new ArrayAdapter<>(context, R.layout.mapbox_attribution_list_item, attributionTitles), this);
    builder.show();
  }

  private String[] getAttributionTitles() {
    List<String> titles = new ArrayList<>();
    for (Attribution attribution : attributionSet) {
      titles.add(attribution.getTitle());
    }
    return titles.toArray(new String[titles.size()]);
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
    return attributionKeyIndex == getAttributionTitles().length - 1;
  }

  private void showTelemetryDialog() {
    AlertDialog.Builder builder = new AlertDialog.Builder(context);
    builder.setTitle(R.string.mapbox_attributionTelemetryTitle);
    builder.setMessage(R.string.mapbox_attributionTelemetryMessage);
    builder.setPositiveButton(R.string.mapbox_attributionTelemetryPositive, new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        Events.obtainTelemetry().enable();
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
        Events.obtainTelemetry().disable();
        dialog.cancel();
      }
    });
    builder.show();
  }

  private void showMapFeedbackWebPage(int which) {
    Attribution[] attributions = attributionSet.toArray(new Attribution[attributionSet.size()]);
    String url = attributions[which].getUrl();
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

    private final MapboxMap mapboxMap;

    AttributionBuilder(MapboxMap mapboxMap) {
      this.mapboxMap = mapboxMap;
    }

    private Set<Attribution> build() {
      List<String> attributions = new ArrayList<>();
      for (Source source : mapboxMap.getSources()) {
        attributions.add(source.getAttribution());
      }

      return new AttributionParser.Options()
        .withCopyrightSign(true)
        .withImproveMap(true)
        .withTelemetryAttribution(true)
        .withAttributionData(attributions.toArray(new String[attributions.size()]))
        .build().getAttributions();
    }
  }
}
