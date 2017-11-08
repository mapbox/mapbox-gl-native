package com.mapbox.mapboxsdk.utils;

import android.text.Html;
import android.text.SpannableStringBuilder;
import android.text.style.URLSpan;
import com.mapbox.mapboxsdk.maps.Attribution;

import java.util.ArrayList;
import java.util.List;

public class AttributionUtils {

  public static List<Attribution> parseAttribution(String[] attribution) {
    StringBuilder builder = new StringBuilder();
    for (String attr : attribution) {
      if (!attr.isEmpty()) {
        builder.append(attr);
      }
    }
    return parseAttribution(builder.toString());
  }

  public static List<Attribution> parseAttribution(String attribution) {
    List<Attribution> attributions = new ArrayList<>();
    SpannableStringBuilder htmlBuilder = (SpannableStringBuilder) Html.fromHtml(attribution);
    URLSpan[] urlSpans = htmlBuilder.getSpans(0, htmlBuilder.length(), URLSpan.class);
    for (URLSpan urlSpan : urlSpans) {
      attributions.add(new Attribution(resolveAnchorValue(htmlBuilder, urlSpan), urlSpan.getURL()));
    }
    return attributions;
  }

  private static String resolveAnchorValue(SpannableStringBuilder htmlBuilder, URLSpan urlSpan) {
    int start = htmlBuilder.getSpanStart(urlSpan);
    int end = htmlBuilder.getSpanEnd(urlSpan);
    int length = end - start;
    char[] charKey = new char[length];
    htmlBuilder.getChars(start, end, charKey, 0);
    return String.valueOf(charKey);
  }
}
