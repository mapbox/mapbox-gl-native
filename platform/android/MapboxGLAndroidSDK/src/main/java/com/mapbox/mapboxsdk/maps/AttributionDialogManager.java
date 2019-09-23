package com.mapbox.mapboxsdk.maps;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Toast;
import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.attribution.Attribution;
import com.mapbox.mapboxsdk.attribution.AttributionParser;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Locale;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Responsible for managing attribution interactions on the map.
 * <p>
 * When the user clicks the attribution icon, {@link AttributionDialogManager#onClick(View)} will be invoked.
 * An attribution dialog will be shown to the user with contents based on the attributions found in the map style.
 * Additionally an telemetry option item is shown to configure telemetry settings.
 * </p>
 */
public class AttributionDialogManager implements View.OnClickListener, DialogInterface.OnClickListener {
  private static final String MAP_FEEDBACK_URL = "https://apps.mapbox.com/feedback";
  private static final String MAP_FEEDBACK_URL_OLD = "https://www.mapbox.com/map-feedback";
  private static final String MAP_FEEDBACK_URL_LOCATION_FRAGMENT_FORMAT = "/%f/%f/%f/%f/%d";
  private static final String MAP_FEEDBACK_STYLE_URI_REGEX = "^(.*://[^:^/]*)/(.*)/(.*)";

  @NonNull
  private final Context context;
  @NonNull
  private final MapboxMap mapboxMap;
  private Set<Attribution> attributionSet;
  private AlertDialog dialog;

  public AttributionDialogManager(@NonNull Context context, @NonNull MapboxMap mapboxMap) {
    this.context = context;
    this.mapboxMap = mapboxMap;
  }

  // Called when someone presses the attribution icon on the map
  @Override
  public void onClick(@NonNull View view) {
    attributionSet = new AttributionBuilder(mapboxMap, view.getContext()).build();

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

  protected void showAttributionDialog(@NonNull String[] attributionTitles) {
    AlertDialog.Builder builder = new AlertDialog.Builder(context);
    builder.setTitle(R.string.mapbox_attributionsDialogTitle);
    builder.setAdapter(new ArrayAdapter<>(context, R.layout.mapbox_attribution_list_item, attributionTitles), this);
    dialog = builder.show();
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
      showMapAttributionWebPage(which);
    }
  }

  public void onStop() {
    if (dialog != null && dialog.isShowing()) {
      dialog.dismiss();
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
      public void onClick(@NonNull DialogInterface dialog, int which) {
        TelemetryDefinition telemetry = Mapbox.getTelemetry();
        if (telemetry != null) {
          telemetry.setUserTelemetryRequestState(true);
        }
        dialog.cancel();
      }
    });
    builder.setNeutralButton(R.string.mapbox_attributionTelemetryNeutral, new DialogInterface.OnClickListener() {
      @Override
      public void onClick(@NonNull DialogInterface dialog, int which) {
        showWebPage(context.getResources().getString(R.string.mapbox_telemetryLink));
        dialog.cancel();
      }
    });
    builder.setNegativeButton(R.string.mapbox_attributionTelemetryNegative, new DialogInterface.OnClickListener() {
      @Override
      public void onClick(@NonNull DialogInterface dialog, int which) {
        TelemetryDefinition telemetry = Mapbox.getTelemetry();
        if (telemetry != null) {
          telemetry.setUserTelemetryRequestState(false);
        }
        dialog.cancel();
      }
    });
    builder.show();
  }

  private void showMapAttributionWebPage(int which) {
    Attribution[] attributions = attributionSet.toArray(new Attribution[attributionSet.size()]);
    String url = attributions[which].getUrl();
    if (url.contains(MAP_FEEDBACK_URL_OLD) || url.contains(MAP_FEEDBACK_URL)) {
      url = buildMapFeedbackMapUrl(Mapbox.getAccessToken());
    }
    showWebPage(url);
  }

  @NonNull
  String buildMapFeedbackMapUrl(@Nullable String accessToken) {
    // TODO Add Android Maps SDK version to the query parameter, currently the version API is not available.
    // TODO Keep track of this issue at [#15632](https://github.com/mapbox/mapbox-gl-native/issues/15632)

    Uri.Builder builder = Uri.parse(MAP_FEEDBACK_URL).buildUpon();

    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    if (cameraPosition != null) {
      builder.encodedFragment(String.format(Locale.getDefault(), MAP_FEEDBACK_URL_LOCATION_FRAGMENT_FORMAT,
              cameraPosition.target.getLongitude(), cameraPosition.target.getLatitude(),
              cameraPosition.zoom, cameraPosition.bearing, (int) cameraPosition.tilt));
    }

    String packageName = context.getApplicationContext().getPackageName();
    if (packageName != null) {
      builder.appendQueryParameter("referrer", packageName);
    }

    if (accessToken != null) {
      builder.appendQueryParameter("access_token", accessToken);
    }

    Style style = mapboxMap.getStyle();
    if (style != null) {
      String styleUri = style.getUri();
      Pattern pattern = Pattern.compile(MAP_FEEDBACK_STYLE_URI_REGEX);
      Matcher matcher = pattern.matcher(styleUri);

      if (matcher.find()) {
        String styleOwner = matcher.group(2);
        String styleId = matcher.group(3);

        builder.appendQueryParameter("owner", styleOwner)
                .appendQueryParameter("id", styleId);
      }
    }

    return builder.build().toString();
  }

  private void showWebPage(@NonNull String url) {
    try {
      Intent intent = new Intent(Intent.ACTION_VIEW);
      intent.setData(Uri.parse(url));
      context.startActivity(intent);
    } catch (ActivityNotFoundException exception) {
      // explicitly handling if the device hasn't have a web browser installed. #8899
      Toast.makeText(context, R.string.mapbox_attributionErrorNoBrowser, Toast.LENGTH_LONG).show();
      MapStrictMode.strictModeViolation(exception);
    }
  }

  private static class AttributionBuilder {

    private final MapboxMap mapboxMap;
    @NonNull
    private final WeakReference<Context> context;

    AttributionBuilder(MapboxMap mapboxMap, Context context) {
      this.mapboxMap = mapboxMap;
      this.context = new WeakReference<>(context);
    }

    private Set<Attribution> build() {
      Context context = this.context.get();
      if (context == null) {
        return Collections.emptySet();
      }

      List<String> attributions = new ArrayList<>();
      String attribution;

      Style style = mapboxMap.getStyle();
      if (style != null) {
        for (Source source : style.getSources()) {
          attribution = source.getAttribution();
          if (!attribution.isEmpty()) {
            attributions.add(attribution);
          }
        }
      }

      return new AttributionParser.Options(context)
        .withCopyrightSign(true)
        .withImproveMap(true)
        .withTelemetryAttribution(true)
        .withAttributionData(attributions.toArray(new String[attributions.size()]))
        .build().getAttributions();
    }
  }
}
