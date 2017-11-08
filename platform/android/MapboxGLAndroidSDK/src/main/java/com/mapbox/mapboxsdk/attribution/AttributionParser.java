package com.mapbox.mapboxsdk.attribution;

import android.text.Html;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.style.URLSpan;

import java.util.LinkedHashSet;
import java.util.Set;

public class AttributionParser {

  private final Set<Attribution> attributions = new LinkedHashSet<>();

  private String attributionDataString;
  private boolean withImproveMap;
  private boolean withCopyrightSign;
  private boolean withTelemetryAttribution;
  private boolean withMapboxAttribution;

  AttributionParser(String attributionDataString, boolean withImproveMap, boolean withCopyrightSign,
                    boolean withTelemetryAttribution, boolean withMapboxAttribution) {
    this.attributionDataString = attributionDataString;
    this.withImproveMap = withImproveMap;
    this.withCopyrightSign = withCopyrightSign;
    this.withTelemetryAttribution = withTelemetryAttribution;
    this.withMapboxAttribution = withMapboxAttribution;
  }

  public Set<Attribution> getAttributions() {
    return attributions;
  }

  public String getAttributionString() {
    StringBuilder stringBuilder = new StringBuilder(withCopyrightSign ? "" : "© ");
    int counter = 0;
    for (Attribution attribution : attributions) {
      counter++;
      stringBuilder.append(attribution.getTitle());
      if (counter != attributions.size()) {
        stringBuilder.append(" / ");
      }
    }
    return stringBuilder.toString();
  }

  void parse() {
    parseAttributions();
    addAdditionalAttributions();
  }

  private void parseAttributions() {
    SpannableStringBuilder htmlBuilder = (SpannableStringBuilder) fromHtml(attributionDataString);
    URLSpan[] urlSpans = htmlBuilder.getSpans(0, htmlBuilder.length(), URLSpan.class);
    for (URLSpan urlSpan : urlSpans) {
      parseUrlSpan(htmlBuilder, urlSpan);
    }
  }

  private void parseUrlSpan(SpannableStringBuilder htmlBuilder, URLSpan urlSpan) {
    String url = urlSpan.getURL();
    if (isUrlSpanValid(url)) {
      String title = parseAnchorValue(htmlBuilder, urlSpan);
      attributions.add(new Attribution(title, url));
    }
  }

  private boolean isUrlSpanValid(String url) {
    return isValidForImproveThisMap(url) && isValidForMapbox(url);
  }

  private boolean isValidForImproveThisMap(String url) {
    return withImproveMap || !url.equals("https://www.mapbox.com/map-feedback/");
  }

  private boolean isValidForMapbox(String url) {
    return withMapboxAttribution || !url.equals("https://www.mapbox.com/about/maps/");
  }

  private String parseAnchorValue(SpannableStringBuilder htmlBuilder, URLSpan urlSpan) {
    int start = htmlBuilder.getSpanStart(urlSpan);
    int end = htmlBuilder.getSpanEnd(urlSpan);
    int length = end - start;
    char[] charKey = new char[length];
    htmlBuilder.getChars(start, end, charKey, 0);
    return stripCopyright(String.valueOf(charKey));
  }

  private String stripCopyright(String anchor) {
    if (!withCopyrightSign && anchor.startsWith("© ")) {
      anchor = anchor.substring(2, anchor.length());
    }
    return anchor;
  }

  private void addAdditionalAttributions() {
    if (withTelemetryAttribution) {
      String telemetryKey = "Telemetry Settings";
      String telemetryLink = "https://www.mapbox.com/telemetry/";
      attributions.add(new Attribution(telemetryKey, telemetryLink));
    }
  }

  /**
   * Convert a string to a spanned html representation.
   *
   * @param html the string to convert
   * @return the spanned html representation
   */
  private static Spanned fromHtml(String html) {
    Spanned result;
    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.N) {
      result = Html.fromHtml(html, Html.FROM_HTML_MODE_LEGACY);
    } else {
      result = Html.fromHtml(html);
    }
    return result;
  }

  public static class Options {
    private boolean withImproveMap = true;
    private boolean withCopyrightSign = true;
    private boolean withTelemetryAttribution = false;
    private boolean withMapboxAttribution = true;
    private String[] attributionDataStringArray;

    public Options withAttributionData(String... attributionData) {
      this.attributionDataStringArray = attributionData;
      return this;
    }

    public Options withImproveMap(boolean withImproveMap) {
      this.withImproveMap = withImproveMap;
      return this;
    }

    public Options withCopyrightSign(boolean withCopyrightSign) {
      this.withCopyrightSign = withCopyrightSign;
      return this;
    }

    public Options withTelemetryAttribution(boolean withTelemetryAttribution) {
      this.withTelemetryAttribution = withTelemetryAttribution;
      return this;
    }

    public Options withMapboxAttribution(boolean withMapboxAttribution) {
      this.withMapboxAttribution = withMapboxAttribution;
      return this;
    }

    public AttributionParser build() {
      if (attributionDataStringArray == null) {
        throw new IllegalStateException("Using builder without providing attribution data");
      }

      String fullAttributionString = parseAttribution(attributionDataStringArray);
      AttributionParser attributionParser = new AttributionParser(
        fullAttributionString,
        withImproveMap,
        withCopyrightSign,
        withTelemetryAttribution,
        withMapboxAttribution
      );
      attributionParser.parse();
      return attributionParser;
    }

    private String parseAttribution(String[] attribution) {
      StringBuilder builder = new StringBuilder();
      for (String attr : attribution) {
        if (!attr.isEmpty()) {
          builder.append(attr);
        }
      }
      return builder.toString();
    }
  }
}
