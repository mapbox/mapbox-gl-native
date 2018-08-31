package com.mapbox.mapboxsdk.attribution;

import android.content.Context;
import android.text.Html;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.style.URLSpan;
import com.mapbox.mapboxsdk.R;

import java.lang.ref.WeakReference;
import java.util.LinkedHashSet;
import java.util.Set;

/**
 * Responsible for parsing attribution data coming from Sources and MapSnapshot.
 * <p>
 * Exposes multiple configuration options to manipulate data being parsed.
 * Use the Options object to build these configurations.
 * </p>
 */
public class AttributionParser {

  private static final String IMPROVE_THIS_MAP = "Improve this map";
  private final WeakReference<Context> context;
  private final Set<Attribution> attributions = new LinkedHashSet<>();
  private final String attributionData;
  private final boolean withImproveMap;
  private final boolean withCopyrightSign;
  private final boolean withTelemetryAttribution;
  private final boolean withMapboxAttribution;

  AttributionParser(WeakReference<Context> context, String attributionData, boolean withImproveMap,
                    boolean withCopyrightSign, boolean withTelemetryAttribution, boolean withMapboxAttribution) {
    this.context = context;
    this.attributionData = attributionData;
    this.withImproveMap = withImproveMap;
    this.withCopyrightSign = withCopyrightSign;
    this.withTelemetryAttribution = withTelemetryAttribution;
    this.withMapboxAttribution = withMapboxAttribution;
  }

  /**
   * Get parsed attributions.
   *
   * @return the attributions
   */
  public Set<Attribution> getAttributions() {
    return attributions;
  }

  /**
   * Get parsed attribution string.
   *
   * @return the parsed attribution string
   */
  public String createAttributionString() {
    return createAttributionString(false);
  }

  /**
   * Get parsed attribution string.
   *
   * @param shortenedOutput if attribution string should contain shortened output
   * @return the parsed attribution string
   */
  public String createAttributionString(boolean shortenedOutput) {
    StringBuilder stringBuilder = new StringBuilder(withCopyrightSign ? "" : "© ");
    int counter = 0;
    for (Attribution attribution : attributions) {
      counter++;
      stringBuilder.append(!shortenedOutput ? attribution.getTitle() : attribution.getTitleAbbreviated());
      if (counter != attributions.size()) {
        stringBuilder.append(" / ");
      }
    }
    return stringBuilder.toString();
  }

  /**
   * Main attribution for configuration
   */
  protected void parse() {
    parseAttributions();
    addAdditionalAttributions();
  }

  /**
   * Parse attributions
   */
  private void parseAttributions() {
    SpannableStringBuilder htmlBuilder = (SpannableStringBuilder) fromHtml(attributionData);
    URLSpan[] urlSpans = htmlBuilder.getSpans(0, htmlBuilder.length(), URLSpan.class);
    for (URLSpan urlSpan : urlSpans) {
      parseUrlSpan(htmlBuilder, urlSpan);
    }
  }

  /**
   * Parse an URLSpan containing an attribution.
   *
   * @param htmlBuilder the html builder
   * @param urlSpan     the url span to be parsed
   */
  private void parseUrlSpan(SpannableStringBuilder htmlBuilder, URLSpan urlSpan) {
    String url = urlSpan.getURL();
    if (isUrlValid(url)) {
      String anchor = parseAnchorValue(htmlBuilder, urlSpan);
      if (isImproveThisMapAnchor(anchor)) {
        anchor = translateImproveThisMapAnchor(anchor);
      }
      attributions.add(new Attribution(anchor, url));
    }
  }

  /**
   * Invoked to validate if an url is valid to be included in the final attribution.
   *
   * @param url the url to be validated
   * @return if the url is valid
   */
  private boolean isUrlValid(String url) {
    return isValidForImproveThisMap(url) && isValidForMapbox(url);
  }

  /**
   * Invoked to validate if an anchor is equal to Improve this map coming from tilesets.
   *
   * @param anchor the anchor to be validated
   * @return if the url is valid
   */
  private boolean isImproveThisMapAnchor(String anchor) {
    return anchor.equals(IMPROVE_THIS_MAP);
  }

  /**
   * Invoked to replace the english Improve this map with localized variant.
   *
   * @param anchor the anchor to be translated
   * @return the translated anchor
   */
  private String translateImproveThisMapAnchor(String anchor) {
    Context context = this.context.get();
    if (context != null) {
      anchor = context.getString(R.string.mapbox_telemetryImproveMap);
    }
    return anchor;
  }

  /**
   * Invoked to validate if an url is valid for the improve map configuration.
   *
   * @param url the url to be validated
   * @return if the url is valid for improve this map
   */
  private boolean isValidForImproveThisMap(String url) {
    return withImproveMap || !url.equals(Attribution.IMPROVE_MAP_URL);
  }

  /**
   * Invoked to validate if an url is valid for the Mapbox configuration.
   *
   * @param url the url to be validated
   * @return if the url is valid for Mapbox
   */
  private boolean isValidForMapbox(String url) {
    return withMapboxAttribution || !url.equals(Attribution.MAPBOX_URL);
  }

  /**
   * Parse the attribution by parsing the anchor value of html href tag.
   *
   * @param htmlBuilder the html builder
   * @param urlSpan     the current urlSpan
   * @return the parsed anchor value
   */
  private String parseAnchorValue(SpannableStringBuilder htmlBuilder, URLSpan urlSpan) {
    int start = htmlBuilder.getSpanStart(urlSpan);
    int end = htmlBuilder.getSpanEnd(urlSpan);
    int length = end - start;
    char[] charKey = new char[length];
    htmlBuilder.getChars(start, end, charKey, 0);
    return stripCopyright(String.valueOf(charKey));
  }

  /**
   * Utility to strip the copyright sign from an attribution
   *
   * @param anchor the attribution string to strip
   * @return the stripped attribution string without the copyright sign
   */
  private String stripCopyright(String anchor) {
    if (!withCopyrightSign && anchor.startsWith("© ")) {
      anchor = anchor.substring(2, anchor.length());
    }
    return anchor;
  }

  /**
   * Invoked to manually add attributions
   */
  private void addAdditionalAttributions() {
    if (withTelemetryAttribution) {
      Context context = this.context.get();
      attributions.add(
        new Attribution(
          context != null ? context.getString(R.string.mapbox_telemetrySettings) : Attribution.TELEMETRY,
          Attribution.TELEMETRY_URL
        )
      );
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

  /**
   * Builder to configure using an AttributionParser.
   * <p>
   * AttributionData, set with {@link #withAttributionData(String...)}, is the only required property to build
   * the underlying AttributionParser. Other properties include trimming the copyright sign, adding telemetry
   * attribution or hiding attribution as improve this map and Mapbox.
   * </p>
   */
  public static class Options {
    private WeakReference<Context> context;
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

    public Options withContext(Context context) {
      this.context = new WeakReference<>(context);
      return this;
    }

    public AttributionParser build() {
      if (attributionDataStringArray == null) {
        throw new IllegalStateException("Using builder without providing attribution data");
      }

      String fullAttributionString = parseAttribution(attributionDataStringArray);
      AttributionParser attributionParser = new AttributionParser(
        context,
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
